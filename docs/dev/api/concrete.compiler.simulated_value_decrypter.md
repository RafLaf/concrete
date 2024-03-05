<!-- markdownlint-disable -->

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/compiler/simulated_value_decrypter.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.compiler.simulated_value_decrypter`
SimulatedValueDecrypter. 



---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/compiler/simulated_value_decrypter.py#L19"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `SimulatedValueDecrypter`
A helper class to decrypt `Value`s. 

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/compiler/simulated_value_decrypter.py#L22"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(value_decrypter: SimulatedValueDecrypter)
```

Wrap the native C++ object. 



**Args:**
  value_decrypter (_SimulatedValueDecrypter):  object to wrap 



**Raises:**
  TypeError:  if `value_decrypter` is not of type `_SimulatedValueDecrypter` 




---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/compiler/simulated_value_decrypter.py#L52"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `decrypt`

```python
decrypt(position: int, value: Value) → Union[int, ndarray]
```

Decrypt value. 



**Args:**
  position (int):  position of the argument within the circuit 

 value (Value):  value to decrypt 



**Returns:**
  Union[int, np.ndarray]:  decrypted value 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/compiler/simulated_value_decrypter.py#L42"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `new`

```python
new(client_parameters: ClientParameters, circuit_name: str = 'main')
```

Create a value decrypter. 


