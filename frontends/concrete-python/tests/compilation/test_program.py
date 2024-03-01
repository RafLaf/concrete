"""
Tests of everything related to multi-circuit.
"""
import pytest
from concrete import fhe
import numpy as np

def test_empty_program():
    with pytest.raises(RuntimeError, match="Tried to define a program without any @fhe.circuits"):
        @fhe.program()
        class Program:
            def square(x):
                return x**2
            
def test_call_clear_circuits():
    @fhe.program()
    class Program:
        @fhe.circuit({"x": "encrypted"})
        def square(x):
            return x ** 2
        @fhe.circuit({"x": "encrypted", "y": "encrypted"})
        def add_sub(x, y):
            return (x + y), (x - y)
        @fhe.circuit({"x": "encrypted", "y": "encrypted"})
        def mul(x, y):
            return x * y

    assert(Program.square(2) == 4)
    assert(Program.add_sub(2, 3) == (5, -1))
    assert(Program.mul(3, 4) == 12)

def test_compile():
    @fhe.program()
    class Program:
        @fhe.circuit({"x": "encrypted"})
        def inc(x):
            return x + 1
        @fhe.circuit({"x": "encrypted"})
        def dec(x):
            return x - 1

    inputset = [
        np.random.randint(1, 20, size=())
        for _ in range(100)
    ]
    configuration = fhe.Configuration(
        p_error=0.1,
        parameter_selection_strategy="v0",
        composable=True
    )
    Program.compile(
        {"inc": inputset, "dec": inputset},
        configuration,
    )

def test_encrypted_execution():
    @fhe.program()
    class Program:
        @fhe.circuit({"x": "encrypted"})
        def inc(x):
            return x + 1
        @fhe.circuit({"x": "encrypted"})
        def dec(x):
            return x - 1

    inputset = [
        np.random.randint(1, 20, size=())
        for _ in range(100)
    ]
    configuration = fhe.Configuration(
        p_error=0.1,
        parameter_selection_strategy="v0",
        composable=True
    )
    program = Program.compile(
        {"inc": inputset, "dec": inputset},
        configuration,
    )

    x = 5
    x_enc = program.inc.encrypt(x)
    x_inc_enc = program.inc.run(x_enc)
    x_inc = program.inc.decrypt(x_inc_enc)
    assert(x_inc == 6)

    x_inc_dec_enc = program.dec.run(x_inc_enc)
    x_inc_dec = program.dec.decrypt(x_inc_dec_enc)
    assert(x_inc_dec == 5)

    for _ in range(10):
        x_enc = program.inc.run(x_enc)
    x_dec = program.inc.decrypt(x_enc)
    assert(x_dec == 15)
    