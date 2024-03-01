"""
Declaration of `MultiCompiler` class.
"""

# pylint: disable=import-error,no-name-in-module

import inspect
import os
import traceback
from copy import deepcopy
from pathlib import Path
from typing import Any, Callable, Dict, Iterable, List, Optional, Tuple, Union

import numpy as np
from concrete.compiler import CompilationContext

from ..extensions import AutoRounder, AutoTruncator
from ..mlir import GraphConverter
from ..representation import Graph
from ..tracing import Tracer
from ..values import ValueDescription
from .artifacts import CircuitDebugArtifacts, ProgramDebugArtifacts
from .circuit import Circuit
from .compiler import EncryptionStatus
from .configuration import Configuration
from .program import ExecutionRt, Program, ProgramCircuit
from .utils import fuse, get_terminal_size

DEFAULT_OUTPUT_DIRECTORY: Path = Path(".artifacts")

# pylint: enable=import-error,no-name-in-module


class CircuitDef:
    """
    An object representing the definition of a circuit as used in a multi-circuit program.
    """

    name: str
    function: Callable
    parameter_encryption_statuses: Dict[str, EncryptionStatus]
    inputset: List[Any]
    graph: Optional[Graph]
    artifacts: Optional[CircuitDebugArtifacts]
    _is_direct: bool
    _parameter_values: Dict[str, ValueDescription]

    def __init__(
        self,
        function: Callable,
        parameter_encryption_statuses: Dict[str, Union[str, EncryptionStatus]],
    ):
        signature = inspect.signature(function)

        missing_args = list(signature.parameters)
        for arg in parameter_encryption_statuses.keys():
            if arg in signature.parameters:
                missing_args.remove(arg)

        if len(missing_args) != 0:
            parameter_str = repr(missing_args[0])
            for arg in missing_args[1:-1]:
                parameter_str += f", {repr(arg)}"
            if len(missing_args) != 1:
                parameter_str += f" and {repr(missing_args[-1])}"

            message = (
                f"Encryption status{'es' if len(missing_args) > 1 else ''} "
                f"of parameter{'s' if len(missing_args) > 1 else ''} "
                f"{parameter_str} of function '{function.__name__}' "
                f"{'are' if len(missing_args) > 1 else 'is'} not provided"
            )
            raise ValueError(message)

        additional_args = list(parameter_encryption_statuses)
        for arg in signature.parameters.keys():
            if arg in parameter_encryption_statuses:
                additional_args.remove(arg)

        if len(additional_args) != 0:
            parameter_str = repr(additional_args[0])
            for arg in additional_args[1:-1]:
                parameter_str += f", {repr(arg)}"
            if len(additional_args) != 1:
                parameter_str += f" and {repr(additional_args[-1])}"

            message = (
                f"Encryption status{'es' if len(additional_args) > 1 else ''} "
                f"of {parameter_str} {'are' if len(additional_args) > 1 else 'is'} provided but "
                f"{'they are' if len(additional_args) > 1 else 'it is'} not a parameter "
                f"of function '{function.__name__}'"
            )
            raise ValueError(message)

        self.function = function  # type: ignore
        self.parameter_encryption_statuses = {
            param: EncryptionStatus(status.lower())
            for param, status in parameter_encryption_statuses.items()
        }
        self.artifacts = None
        self.inputset = []
        self.graph = None
        self.name = function.__name__
        self._is_direct = False
        self._parameter_values = {}

    def _trace(self, sample: Union[Any, Tuple[Any, ...]]):
        """
        Trace the function and fuse the resulting graph with a sample input.

        Args:
            sample (Union[Any, Tuple[Any, ...]]):
                sample to use for tracing
        """

        if self.artifacts is not None:
            self.artifacts.add_source_code(self.function)
            for param, encryption_status in self.parameter_encryption_statuses.items():
                self.artifacts.add_parameter_encryption_status(param, encryption_status)

        parameters = {
            param: ValueDescription.of(arg, is_encrypted=(status == EncryptionStatus.ENCRYPTED))
            for arg, (param, status) in zip(
                (
                    sample
                    if len(self.parameter_encryption_statuses) > 1 or isinstance(sample, tuple)
                    else (sample,)
                ),
                self.parameter_encryption_statuses.items(),
            )
        }

        self.graph = Tracer.trace(self.function, parameters)
        if self.artifacts is not None:
            self.artifacts.add_graph("initial", self.graph)

        fuse(self.graph, self.artifacts)

    def _evaluate(
        self,
        action: str,
        inputset: Optional[Union[Iterable[Any], Iterable[Tuple[Any, ...]]]],
        configuration: Configuration,
        artifacts: CircuitDebugArtifacts,
    ):
        """
        Trace, fuse, measure bounds, and update values in the resulting graph in one go.

        Args:
            action (str):
                action being performed (e.g., "trace", "compile")

            inputset (Optional[Union[Iterable[Any], Iterable[Tuple[Any, ...]]]]):
                optional inputset to extend accumulated inputset before bounds measurement
        """

        if self._is_direct:
            self.graph = Tracer.trace(self.function, self._parameter_values, is_direct=True)
            artifacts.add_graph("initial", self.graph)  # pragma: no cover
            fuse(self.graph, artifacts)
            artifacts.add_graph("final", self.graph)  # pragma: no cover
            return

        if inputset is not None:
            previous_inputset_length = len(self.inputset)
            for index, sample in enumerate(iter(inputset)):
                self.inputset.append(sample)

                if not isinstance(sample, tuple):
                    sample = (sample,)

                if len(sample) != len(self.parameter_encryption_statuses):
                    self.inputset = self.inputset[:previous_inputset_length]

                    expected = (
                        "a single value"
                        if len(self.parameter_encryption_statuses) == 1
                        else f"a tuple of {len(self.parameter_encryption_statuses)} values"
                    )
                    actual = (
                        "a single value" if len(sample) == 1 else f"a tuple of {len(sample)} values"
                    )

                    message = (
                        f"Input #{index} of your inputset is not well formed "
                        f"(expected {expected} got {actual})"
                    )
                    raise ValueError(message)

        if configuration.auto_adjust_rounders:
            AutoRounder.adjust(self.function, self.inputset)

        if configuration.auto_adjust_truncators:
            AutoTruncator.adjust(self.function, self.inputset)

        if self.graph is None:
            try:
                first_sample = next(iter(self.inputset))
            except StopIteration as error:
                message = (
                    f"{action} function '{self.function.__name__}' "
                    f"without an inputset is not supported"
                )
                raise RuntimeError(message) from error

            self._trace(first_sample)
            assert self.graph is not None

        bounds = self.graph.measure_bounds(self.inputset)
        self.graph.update_with_bounds(bounds)

        artifacts.add_graph("final", self.graph)

    def __call__(
        self,
        *args: Any,
        **kwargs: Any,
    ) -> Union[
        np.bool_,
        np.integer,
        np.floating,
        np.ndarray,
        Tuple[Union[np.bool_, np.integer, np.floating, np.ndarray], ...],
    ]:
        if len(kwargs) != 0:
            message = f"Calling function '{self.function.__name__}' with kwargs is not supported"
            raise RuntimeError(message)

        sample = args[0] if len(args) == 1 else args

        if self.graph is None:
            self._trace(sample)
            assert self.graph is not None

        self.inputset.append(sample)
        return self.graph(*args)


class DebugManager:
    """
    A debug manager, allowing streamlined debugging.
    """

    configuration: Configuration
    begin_call: Callable

    def __init__(self, config: Configuration):
        self.configuration = config
        is_first = [True]

        def begin_call():
            if is_first[0]:
                print()
                is_first[0] = False

        self.begin_call = begin_call

    def debug_table(self, title: str, activate: bool = True):
        """
        Returns a context manager that prints a table around what is printed inside the scope.
        """

        class DebugTableCm:
            def __init__(self, title):
                self.title = title
                self.columns = get_terminal_size()

            def __enter__(self):
                print(f"{self.title}")
                print("-" * self.columns)

            def __exit__(self, _exc_type, _exc_value, _exc_tb):
                print("-" * self.columns)
                print()

        class EmptyCm:
            def __enter__(self):
                pass

            def __exit__(self, _exc_type, _exc_value, _exc_tb):
                pass

        if activate:
            self.begin_call()
            return DebugTableCm(title)
        else:
            return EmptyCm()

    def show_graph(self):
        return (
            self.configuration.show_graph
            if self.configuration.show_graph is not None
            else self.configuration.verbose
        )

    def show_bit_width_constraints(self):
        return (
            self.configuration.show_bit_width_constraints
            if self.configuration.show_bit_width_constraints is not None
            else self.configuration.verbose
        )

    def show_bit_width_assignments(self):
        return (
            self.configuration.show_bit_width_assignments
            if self.configuration.show_bit_width_assignments is not None
            else self.configuration.verbose
        )

    def show_assigned_graph(self):
        return (
            self.configuration.show_assigned_graph
            if self.configuration.show_assigned_graph is not None
            else self.configuration.verbose
        )

    def show_mlir(self):
        return (
            self.configuration.show_mlir
            if self.configuration.show_mlir is not None
            else self.configuration.verbose
        )

    def show_optimizer(self):
        return (
            self.configuration.show_optimizer
            if self.configuration.show_optimizer is not None
            else self.configuration.verbose
        )

    def show_statistics(self):
        return (
            self.configuration.show_statistics
            if self.configuration.show_statistics is not None
            else self.configuration.verbose
        )

    def debug_computation_graph(self, name, circuit_graph):
        if (
            self.show_graph()
            or self.show_bit_width_constraints()
            or self.show_bit_width_assignments()
            or self.show_assigned_graph()
            or self.show_mlir()
            or self.show_optimizer()
            or self.show_statistics()
        ):
            if self.show_graph():
                with self.debug_table(f"Computation Graph for {name}"):
                    print(circuit_graph.format())

    def debug_bit_width_constaints(self, name, circuit_graph):
        if self.show_bit_width_constraints():
            with self.debug_table(f"Bit-Width Constraints for {name}"):
                print(circuit_graph.format_bit_width_constraints())

    def debug_bit_width_assignments(self, name, circuit_graph):
        if self.show_bit_width_assignments():
            with self.debug_table(f"Bit-Width Assignments for {name}"):
                print(circuit_graph.format_bit_width_assignments())

    def debug_assigned_graph(self, name, circuit_graph):
        if self.show_assigned_graph():
            with self.debug_table(f"Bit-Width Assigned Computation Graph for {name}"):
                print(circuit_graph.format(show_assigned_bit_widths=True))

    def debug_mlir(self, mlir_str):
        if self.show_mlir():
            with self.debug_table("MLIR"):
                print(mlir_str)

    def debug_statistics(self, program):
        if self.show_statistics():

            def pretty(d, indent=0):  # pragma: no cover
                if indent > 0:
                    print("{")

                for key, value in d.items():
                    if isinstance(value, dict) and len(value) == 0:
                        continue
                    print("    " * indent + str(key) + ": ", end="")
                    if isinstance(value, dict):
                        pretty(value, indent + 1)
                    else:
                        print(value)
                if indent > 0:
                    print("    " * (indent - 1) + "}")

            with self.debug_table("Statistics for program"):
                pretty(program.statistics)


class ProgramCompiler:
    """
    Compiler class for multiple circuits, to glue the compilation pipeline.
    """

    default_configuration: Configuration
    circuits: Dict[str, CircuitDef]
    compilation_context: CompilationContext

    def __init__(self, circuits):
        self.configuration = Configuration(composable=True)
        self.circuits = circuits
        self.compilation_context = CompilationContext.new()

    # pylint: disable=too-many-branches,too-many-statement

    def compile(
        self,
        inputsets: Optional[Dict[str, Union[Iterable[Any], Iterable[Tuple[Any, ...]]]]] = None,
        configuration: Optional[Configuration] = None,
        program_artifacts: Optional[ProgramDebugArtifacts] = None,
        **kwargs,
    ) -> Program:
        """
        Compile the circuits using an ensemble of inputsets.

        Args:
            inputsets (Optional[Dict[str, Union[Iterable[Any], Iterable[Tuple[Any, ...]]]]]):
                optional inputsets to extend accumulated inputsets before bounds measurement

            configuration(Optional[Configuration], default = None):
                configuration to use

            artifacts (Optional[ProgramDebugArtifacts], default = None):
                artifacts to store information about the process

            kwargs (Dict[str, Any]):
                configuration options to overwrite

        Returns:
            Program:
                compiled program
        """

        configuration = configuration if configuration is not None else self.default_configuration
        configuration = deepcopy(configuration)
        if len(kwargs) != 0:
            configuration = configuration.fork(**kwargs)
        if not configuration.composable:
            raise RuntimeError("Programs can only be compiled with `composable` activated.")

        program_artifacts = (
            program_artifacts
            if program_artifacts is not None
            else ProgramDebugArtifacts(list(self.circuits.keys()))
        )

        dbg = DebugManager(configuration)

        try:
            # Trace and fuse the circuits
            for name, circuit in self.circuits.items():
                inputset = inputsets[name]
                circuit_artifacts = program_artifacts.circuits[name]
                circuit._evaluate("Compiling", inputset, self.configuration, circuit_artifacts)
                assert circuit.graph is not None
                dbg.debug_computation_graph(name, circuit.graph)

            # Convert the graphs to an mlir module
            mlir_context = self.compilation_context.mlir_context()
            graphs = {name: circuit.graph for (name, circuit) in self.circuits.items()}
            mlir_module = GraphConverter().convert_many(graphs, self.configuration, mlir_context)
            mlir_str = str(mlir_module).strip()
            dbg.debug_mlir(mlir_str)
            program_artifacts.add_mlir_to_compile(mlir_str)

            # Debug some circuit informations
            for name, circuit in self.circuits.items():
                dbg.debug_bit_width_constaints(name, circuit.graph)
                dbg.debug_bit_width_assignments(name, circuit.graph)
                dbg.debug_assigned_graph(name, circuit.graph)

            # Compile to a program!
            with dbg.debug_table("Optimizer", activate=dbg.show_optimizer()):
                output = Program(graphs, mlir_module, self.compilation_context, configuration)
                if isinstance(output.rt, ExecutionRt):
                    client_parameters = output.rt.client.specs.client_parameters
                    program_artifacts.add_client_parameters(client_parameters.serialize())

            dbg.debug_statistics(output)

        except Exception:  # pragma: no cover
            # this branch is reserved for unexpected issues and hence it shouldn't be tested
            # if it could be tested, we would have fixed the underlying issue

            # if the user desires so,
            # we need to export all the information we have about the compilation

            if self.configuration.dump_artifacts_on_unexpected_failures:
                program_artifacts.export()

                # traceback_path = self.artifacts.output_directory.joinpath("traceback.txt")
                # with open(traceback_path, "w", encoding="utf-8") as f:
                #     f.write(traceback.format_exc())

            raise

        return output

    # pylint: enable=too-many-branches,too-many-statements

    def __getattr__(self, item):
        if not item in list(self.circuits.keys()):
            raise AttributeError(f"No attribute {item}")
        else:
            return self.circuits[item]
