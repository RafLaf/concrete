"""
Declaration of `Program` classes.
"""

# pylint: disable=import-error,no-member,no-name-in-module

from pathlib import Path
from typing import Any, Dict, List, NamedTuple, Optional, Tuple, Union

import numpy as np
from concrete.compiler import (
    CompilationContext,
    Parameter,
    SimulatedValueDecrypter,
    SimulatedValueExporter,
)
from mlir.ir import Module as MlirModule

from ..internal.utils import assert_that
from ..representation import Graph
from .client import Client
from .configuration import Configuration
from .keys import Keys
from .server import Server
from .utils import validate_input_args
from .value import Value

# pylint: enable=import-error,no-member,no-name-in-module


class ExecutionRt(NamedTuple):
    """
    Runtime object class for execution.
    """

    client: Client
    server: Server


class SimulationRt(NamedTuple):
    """
    Runtime object class for simulation.
    """

    server: Server


class ProgramCircuit:
    """
    Circuit class, allowing to run or simulate one circuit of a program.
    """

    rt: Union[ExecutionRt, SimulationRt]
    graph: Graph
    name: str

    def __init__(self, name: str, rt: Union[ExecutionRt, SimulationRt], graph: Graph):
        self.name = name
        self.rt = rt
        self.graph = graph

    def draw(
        self,
        *,
        horizontal: bool = False,
        save_to: Optional[Union[Path, str]] = None,
        show: bool = False,
    ) -> Path:
        """
        Draw the graph of the circuit.

        That this function requires the python `pygraphviz` package
        which itself requires the installation of `graphviz` packages

        (see https://pygraphviz.github.io/documentation/stable/install.html)

        Args:
            horizontal (bool, default = False):
                whether to draw horizontally

            save_to (Optional[Path], default = None):
                path to save the drawing
                a temporary file will be used if it's None

            show (bool, default = False):
                whether to show the drawing using matplotlib

        Returns:
            Path:
                path to the drawing
        """
        return self.graph.draw(horizontal=horizontal, save_to=save_to, show=show)

    def __str__(self):
        return self.graph.format()

    def simulate(self, *args: Any) -> Any:
        """
        Simulate execution of the circuit.

        Args:
            *args (Any):
                inputs to the circuit

        Returns:
            Any:
                result of the simulation
        """
        assert isinstance(self.rt, SimulationRt)

        ordered_validated_args = validate_input_args(
            self.rt.server.client_specs, *args, circuit_name=self.name
        )

        exporter = SimulatedValueExporter.new(
            self.rt.server.client_specs.client_parameters, self.name
        )
        exported = [
            None
            if arg is None
            else Value(
                exporter.export_tensor(position, arg.flatten().tolist(), list(arg.shape))
                if isinstance(arg, np.ndarray) and arg.shape != ()
                else exporter.export_scalar(position, int(arg))
            )
            for position, arg in enumerate(ordered_validated_args)
        ]

        results = self.rt.server.run(*exported, circuit_name=self.name)
        if not isinstance(results, tuple):
            results = (results,)

        decrypter = SimulatedValueDecrypter.new(
            self.rt.server.client_specs.client_parameters, self.name
        )
        decrypted = tuple(
            decrypter.decrypt(position, result.inner) for position, result in enumerate(results)
        )

        return decrypted if len(decrypted) != 1 else decrypted[0]

    def encrypt(
        self,
        *args: Optional[Union[int, np.ndarray, List]],
    ) -> Optional[Union[Value, Tuple[Optional[Value], ...]]]:
        """
        Encrypt argument(s) to for evaluation.

        Args:
            *args (Optional[Union[int, numpy.ndarray, List]]):
                argument(s) for evaluation

        Returns:
            Optional[Union[Value, Tuple[Optional[Value], ...]]]:
                encrypted argument(s) for evaluation
        """
        assert isinstance(self.rt, ExecutionRt)
        return self.rt.client.encrypt(*args, circuit_name=self.name)

    def run(
        self,
        *args: Optional[Union[Value, Tuple[Optional[Value], ...]]],
    ) -> Union[Value, Tuple[Value, ...]]:
        """
        Evaluate the circuit.

        Args:
            *args (Value):
                argument(s) for evaluation

        Returns:
            Union[Value, Tuple[Value, ...]]:
                result(s) of evaluation
        """
        assert isinstance(self.rt, ExecutionRt)
        return self.rt.server.run(
            *args, evaluation_keys=self.rt.client.evaluation_keys, circuit_name=self.name
        )

    def decrypt(
        self,
        *results: Union[Value, Tuple[Value, ...]],
    ) -> Optional[Union[int, np.ndarray, Tuple[Optional[Union[int, np.ndarray]], ...]]]:
        """
        Decrypt result(s) of evaluation.

        Args:
            *results (Union[Value, Tuple[Value, ...]]):
                result(s) of evaluation

        Returns:
            Optional[Union[int, np.ndarray, Tuple[Optional[Union[int, np.ndarray]], ...]]]:
                decrypted result(s) of evaluation
        """
        assert isinstance(self.rt, ExecutionRt)
        return self.rt.client.decrypt(*results, circuit_name=self.name)

    def encrypt_run_decrypt(self, *args: Any) -> Any:
        """
        Encrypt inputs, run the circuit, and decrypt the outputs in one go.

        Args:
            *args (Union[int, numpy.ndarray]):
                inputs to the circuit

        Returns:
            Union[int, np.ndarray, Tuple[Union[int, np.ndarray], ...]]:
                clear result of homomorphic evaluation
        """
        return self.decrypt(self.run(self.encrypt(*args)))

    @property
    def size_of_inputs(self) -> int:
        """
        Get size of the inputs of the circuit.
        """
        return self.rt.server.size_of_inputs(self.name)  # pragma: no cover

    @property
    def size_of_outputs(self) -> int:
        """
        Get size of the outputs of the circuit.
        """
        return self.rt.server.size_of_outputs(self.name)  # pragma: no cover

    # Programmable Bootstrap Statistics

    @property
    def programmable_bootstrap_count(self) -> int:
        """
        Get the number of programmable bootstraps in the circuit.
        """
        return self.rt.server.programmable_bootstrap_count(self.name)  # pragma: no cover

    @property
    def programmable_bootstrap_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of programmable bootstraps per bit width in the circuit.
        """
        return self.rt.server.programmable_bootstrap_count_per_parameter(
            self.name
        )  # pragma: no cover

    @property
    def programmable_bootstrap_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of programmable bootstraps per tag in the circuit.
        """
        return self.rt.server.programmable_bootstrap_count_per_tag(self.name)  # pragma: no cover

    @property
    def programmable_bootstrap_count_per_tag_per_parameter(self) -> Dict[str, Dict[int, int]]:
        """
        Get the number of programmable bootstraps per tag per bit width in the circuit.
        """
        return self.rt.server.programmable_bootstrap_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    # Key Switch Statistics

    @property
    def key_switch_count(self) -> int:
        """
        Get the number of key switches in the circuit.
        """
        return self.rt.server.key_switch_count(self.name)  # pragma: no cover

    @property
    def key_switch_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of key switches per parameter in the circuit.
        """
        return self.rt.server.key_switch_count_per_parameter(self.name)  # pragma: no cover

    @property
    def key_switch_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of key switches per tag in the circuit.
        """
        return self.rt.server.key_switch_count_per_tag(self.name)  # pragma: no cover

    @property
    def key_switch_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of key switches per tag per parameter in the circuit.
        """
        return self.rt.server.key_switch_count_per_tag_per_parameter(self.name)  # pragma: no cover

    # Packing Key Switch Statistics

    @property
    def packing_key_switch_count(self) -> int:
        """
        Get the number of packing key switches in the circuit.
        """
        return self.rt.server.packing_key_switch_count(self.name)  # pragma: no cover

    @property
    def packing_key_switch_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of packing key switches per parameter in the circuit.
        """
        return self.rt.server.packing_key_switch_count_per_parameter(self.name)  # pragma: no cover

    @property
    def packing_key_switch_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of packing key switches per tag in the circuit.
        """
        return self.rt.server.packing_key_switch_count_per_tag(self.name)  # pragma: no cover

    @property
    def packing_key_switch_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of packing key switches per tag per parameter in the circuit.
        """
        return self.rt.server.packing_key_switch_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    # Clear Addition Statistics

    @property
    def clear_addition_count(self) -> int:
        """
        Get the number of clear additions in the circuit.
        """
        return self.rt.server.clear_addition_count(self.name)  # pragma: no cover

    @property
    def clear_addition_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of clear additions per parameter in the circuit.
        """
        return self.rt.server.clear_addition_count_per_parameter(self.name)  # pragma: no cover

    @property
    def clear_addition_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of clear additions per tag in the circuit.
        """
        return self.rt.server.clear_addition_count_per_tag(self.name)  # pragma: no cover

    @property
    def clear_addition_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of clear additions per tag per parameter in the circuit.
        """
        return self.rt.server.clear_addition_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    # Encrypted Addition Statistics

    @property
    def encrypted_addition_count(self) -> int:
        """
        Get the number of encrypted additions in the circuit.
        """
        return self.rt.server.encrypted_addition_count(self.name)  # pragma: no cover

    @property
    def encrypted_addition_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of encrypted additions per parameter in the circuit.
        """
        return self.rt.server.encrypted_addition_count_per_parameter(self.name)  # pragma: no cover

    @property
    def encrypted_addition_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of encrypted additions per tag in the circuit.
        """
        return self.rt.server.encrypted_addition_count_per_tag(self.name)  # pragma: no cover

    @property
    def encrypted_addition_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of encrypted additions per tag per parameter in the circuit.
        """
        return self.rt.server.encrypted_addition_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    # Clear Multiplication Statistics

    @property
    def clear_multiplication_count(self) -> int:
        """
        Get the number of clear multiplications in the circuit.
        """
        return self.rt.server.clear_multiplication_count(self.name)  # pragma: no cover

    @property
    def clear_multiplication_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of clear multiplications per parameter in the circuit.
        """
        return self.rt.server.clear_multiplication_count_per_parameter(
            self.name
        )  # pragma: no cover

    @property
    def clear_multiplication_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of clear multiplications per tag in the circuit.
        """
        return self.rt.server.clear_multiplication_count_per_tag(self.name)  # pragma: no cover

    @property
    def clear_multiplication_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of clear multiplications per tag per parameter in the circuit.
        """
        return self.rt.server.clear_multiplication_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    # Encrypted Negation Statistics

    @property
    def encrypted_negation_count(self) -> int:
        """
        Get the number of encrypted negations in the circuit.
        """
        return self.rt.server.encrypted_negation_count(self.name)  # pragma: no cover

    @property
    def encrypted_negation_count_per_parameter(self) -> Dict[Parameter, int]:
        """
        Get the number of encrypted negations per parameter in the circuit.
        """
        return self.rt.server.encrypted_negation_count_per_parameter(self.name)  # pragma: no cover

    @property
    def encrypted_negation_count_per_tag(self) -> Dict[str, int]:
        """
        Get the number of encrypted negations per tag in the circuit.
        """
        return self.rt.server.encrypted_negation_count_per_tag(self.name)  # pragma: no cover

    @property
    def encrypted_negation_count_per_tag_per_parameter(self) -> Dict[str, Dict[Parameter, int]]:
        """
        Get the number of encrypted negations per tag per parameter in the circuit.
        """
        return self.rt.server.encrypted_negation_count_per_tag_per_parameter(
            self.name
        )  # pragma: no cover

    @property
    def statistics(self) -> Dict:
        """
        Get all statistics of the circuit.
        """
        attributes = [
            "size_of_inputs",
            "size_of_outputs",
            "programmable_bootstrap_count",
            "programmable_bootstrap_count_per_parameter",
            "programmable_bootstrap_count_per_tag",
            "programmable_bootstrap_count_per_tag_per_parameter",
            "key_switch_count",
            "key_switch_count_per_parameter",
            "key_switch_count_per_tag",
            "key_switch_count_per_tag_per_parameter",
            "packing_key_switch_count",
            "packing_key_switch_count_per_parameter",
            "packing_key_switch_count_per_tag",
            "packing_key_switch_count_per_tag_per_parameter",
            "clear_addition_count",
            "clear_addition_count_per_parameter",
            "clear_addition_count_per_tag",
            "clear_addition_count_per_tag_per_parameter",
            "encrypted_addition_count",
            "encrypted_addition_count_per_parameter",
            "encrypted_addition_count_per_tag",
            "encrypted_addition_count_per_tag_per_parameter",
            "clear_multiplication_count",
            "clear_multiplication_count_per_parameter",
            "clear_multiplication_count_per_tag",
            "clear_multiplication_count_per_tag_per_parameter",
            "encrypted_negation_count",
            "encrypted_negation_count_per_parameter",
            "encrypted_negation_count_per_tag",
            "encrypted_negation_count_per_tag_per_parameter",
        ]
        return {attribute: getattr(self, attribute) for attribute in attributes}


class Program:
    """
    Program class, to combine computation graphs, mlir, runtime objects into a single object.
    """

    configuration: Configuration
    graphs: Dict[str, Graph]
    mlir_module: MlirModule
    compilation_context: CompilationContext
    rt: Union[ExecutionRt, SimulationRt]

    def __init__(
        self,
        graphs: Dict[str, Graph],
        mlir: MlirModule,
        compilation_context: CompilationContext,
        configuration: Optional[Configuration] = None,
    ):
        assert configuration and (configuration.fhe_simulation or configuration.fhe_execution)

        self.configuration = configuration if configuration is not None else Configuration()
        self.graphs = graphs
        self.mlir_module = mlir
        self.compilation_context = compilation_context

        if self.configuration.fhe_simulation:
            server = Server.create(
                self.mlir_module,
                self.configuration,
                is_simulated=True,
                compilation_context=self.compilation_context,
            )
            self.rt = SimulationRt(server)
        else:
            server = Server.create(
                self.mlir_module, self.configuration, compilation_context=self.compilation_context
            )

            keyset_cache_directory = None
            if self.configuration.use_insecure_key_cache:
                assert_that(self.configuration.enable_unsafe_features)
                assert_that(self.configuration.insecure_key_cache_location is not None)
                keyset_cache_directory = self.configuration.insecure_key_cache_location

            client = Client(server.client_specs, keyset_cache_directory)
            self.rt = ExecutionRt(client, server)

    @property
    def mlir(self) -> str:
        """Textual representation of the MLIR module.

        Returns:
            str: textual representation of the MLIR module
        """
        return str(self.mlir_module).strip()

    @property
    def keys(self) -> Keys:
        """
        Get the keys of the circuit.
        """
        if isinstance(self.rt, ExecutionRt):
            return self.rt.client.keys
        else:
            return None

    @keys.setter
    def keys(self, new_keys: Keys):
        """
        Set the keys of the circuit.
        """
        if isinstance(self.rt, ExecutionRt):
            self.rt.client.keys = new_keys

    def keygen(
        self, force: bool = False, seed: Optional[int] = None, encryption_seed: Optional[int] = None
    ):
        """
        Generate keys required for homomorphic evaluation.

        Args:
            force (bool, default = False):
                whether to generate new keys even if keys are already generated

            seed (Optional[int], default = None):
                seed for private keys randomness

            encryption_seed (Optional[int], default = None):
                seed for encryption randomness
        """
        if isinstance(self.rt, ExecutionRt):
            self.rt.client.keygen(force, seed, encryption_seed)

    def cleanup(self):
        """
        Cleanup the temporary library output directory.
        """
        self.rt.server.cleanup()

    @property
    def size_of_secret_keys(self) -> int:
        """
        Get size of the secret keys of the program.
        """
        return self.rt.server.size_of_secret_keys  # pragma: no cover

    @property
    def size_of_bootstrap_keys(self) -> int:
        """
        Get size of the bootstrap keys of the program.
        """
        return self.rt.server.size_of_bootstrap_keys  # pragma: no cover

    @property
    def size_of_keyswitch_keys(self) -> int:
        """
        Get size of the key switch keys of the program.
        """
        return self.rt.server.size_of_keyswitch_keys  # pragma: no cover

    @property
    def p_error(self) -> int:
        """
        Get probability of error for each simple TLU (on a scalar).
        """
        return self.rt.server.p_error  # pragma: no cover

    @property
    def global_p_error(self) -> int:
        """
        Get the probability of having at least one simple TLU error during the entire execution.
        """
        return self.rt.server.global_p_error  # pragma: no cover

    @property
    def complexity(self) -> float:
        """
        Get complexity of the program.
        """
        return self.rt.server.complexity  # pragma: no cover

    @property
    def statistics(self) -> Dict:
        """
        Get all statistics of the program.
        """
        attributes = [
            "size_of_secret_keys",
            "size_of_bootstrap_keys",
            "size_of_keyswitch_keys",
            "p_error",
            "global_p_error",
            "complexity",
        ]
        statistics = {attribute: getattr(self, attribute) for attribute in attributes}
        statistics["circuits"] = {
            name: circuit.statistics for (name, circuit) in self.circuits().items()
        }
        return statistics

    def circuits(self) -> Dict[str, ProgramCircuit]:
        """
        Returns a dictionnary containing all the circuits of the program.
        """
        return {name: self.__getattr__(name) for name in self.graphs.keys()}

    def __getattr__(self, item):
        if not item in list(self.graphs.keys()):
            raise AttributeError(f"No attribute {item}")
        else:
            return ProgramCircuit(item, self.rt, self.graphs[item])
