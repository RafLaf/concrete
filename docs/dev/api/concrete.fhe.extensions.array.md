<!-- markdownlint-disable -->

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/extensions/array.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.fhe.extensions.array`
Declaration of `array` function, to simplify creation of encrypted arrays. 


---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/extensions/array.py#L16"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>function</kbd> `array`

```python
array(values: Any) → Union[ndarray, Tracer]
```

Create an encrypted array from either encrypted or clear values. 



**Args:**
  values (Any):  array like object compatible with numpy to construct the resulting encrypted array 



**Returns:**
  Union[np.ndarray, Tracer]:  Tracer that respresents the operation during tracing  ndarray with values otherwise 


