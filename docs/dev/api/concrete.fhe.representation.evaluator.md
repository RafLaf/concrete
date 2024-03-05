<!-- markdownlint-disable -->

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.fhe.representation.evaluator`
Declaration of various `Evaluator` classes, to make graphs picklable. 



---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L8"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ConstantEvaluator`
ConstantEvaluator class, to evaluate Operation.Constant nodes. 

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L13"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(properties)
```









---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L20"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `InputEvaluator`
InputEvaluator class, to evaluate Operation.Input nodes. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L29"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `GenericEvaluator`
GenericEvaluator class, to evaluate Operation.Generic nodes. 

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L34"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(operation, properties)
```









---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L42"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `GenericTupleEvaluator`
GenericEvaluator class, to evaluate Operation.Generic nodes where args are packed in a tuple. 

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/representation/evaluator.py#L47"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(operation, properties)
```









