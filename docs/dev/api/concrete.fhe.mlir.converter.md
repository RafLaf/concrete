<!-- markdownlint-disable -->

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.fhe.mlir.converter`
Declaration of `Converter` class. 

**Global Variables**
---------------
- **MAXIMUM_TLU_BIT_WIDTH**


---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L31"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `Converter`
Converter class, to convert a computation graph to MLIR. 




---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L243"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `add`

```python
add(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L247"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `array`

```python
array(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L251"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `assign_static`

```python
assign_static(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L260"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `bitwise_and`

```python
bitwise_and(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L268"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `bitwise_or`

```python
bitwise_or(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L276"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `bitwise_xor`

```python
bitwise_xor(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L284"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `broadcast_to`

```python
broadcast_to(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L288"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `concatenate`

```python
concatenate(ctx: Context, node: Node, preds: List[Conversion])
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L295"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `constant`

```python
constant(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L299"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `conv1d`

```python
conv1d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L303"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `conv2d`

```python
conv2d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L316"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `conv3d`

```python
conv3d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L36"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `convert`

```python
convert(
    graph: Graph,
    configuration: Configuration,
    mlir_context: <locals>Context
) → Module
```

Convert a computation graph to MLIR. 



**Args:**
  graph (Graph):  graph to convert 

 configuration (Configuration):  configuration to use 

 mlir_context (MlirContext):  MLIR Context to use for module generation 

Return:  MlirModule:  In-memory MLIR module corresponding to the graph 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L320"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `copy`

```python
copy(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L324"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `dot`

```python
dot(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L328"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `dynamic_tlu`

```python
dynamic_tlu(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L332"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `equal`

```python
equal(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L340"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `expand_dims`

```python
expand_dims(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L344"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `extract_bit_pattern`

```python
extract_bit_pattern(
    ctx: Context,
    node: Node,
    preds: List[Conversion]
) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L348"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `greater`

```python
greater(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L356"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `greater_equal`

```python
greater_equal(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L364"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `identity`

```python
identity(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L368"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `index_static`

```python
index_static(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L376"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `left_shift`

```python
left_shift(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L390"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `less`

```python
less(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L398"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `less_equal`

```python
less_equal(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L406"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `matmul`

```python
matmul(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L410"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `maximum`

```python
maximum(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L418"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `maxpool1d`

```python
maxpool1d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L422"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `maxpool2d`

```python
maxpool2d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L432"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `maxpool3d`

```python
maxpool3d(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L436"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `minimum`

```python
minimum(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L444"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `multiply`

```python
multiply(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L448"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `negative`

```python
negative(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L206"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `node`

```python
node(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```

Convert a computation graph node into MLIR. 



**Args:**
  ctx (Context):  conversion context 

 node (Node):  node to convert 

 preds (List[Conversion]):  conversions of ordered predecessors of the node 

Return:  Conversion:  conversion object corresponding to node 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L452"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `not_equal`

```python
not_equal(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L460"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `ones`

```python
ones(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L175"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `process`

```python
process(graph: Graph, configuration: Configuration)
```

Process a computation graph for MLIR conversion. 



**Args:**
  graph (Graph):  graph to process 

 configuration (Configuration):  configuration to use 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L464"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `relu`

```python
relu(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L468"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `reshape`

```python
reshape(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L472"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `right_shift`

```python
right_shift(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L486"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `round_bit_pattern`

```python
round_bit_pattern(
    ctx: Context,
    node: Node,
    preds: List[Conversion]
) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L105"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `simplify_tag`

```python
simplify_tag(configuration: Configuration, tag: str) → str
```

Keep only `n` higher tag parts. 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L522"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `squeeze`

```python
squeeze(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L100"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `stdout_with_ansi_support`

```python
stdout_with_ansi_support() → bool
```

Detect if ansi characters can be used (e.g. not the case in notebooks). 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L509"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `subtract`

```python
subtract(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L513"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `sum`

```python
sum(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L593"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `tlu`

```python
tlu(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L538"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>classmethod</kbd> `tlu_adjust`

```python
tlu_adjust(table, variable_input, target_bit_width, clipping, reduce_precision)
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L117"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>classmethod</kbd> `trace_progress`

```python
trace_progress(
    configuration: Configuration,
    progress_index: int,
    nodes: List[Node]
)
```

Add a trace_message for progress. 



**Args:**
  configuration:  configuration for title, tags options 

 progress_index:  index of the next node to process 

 nodes:  all nodes 

---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L755"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `transpose`

```python
transpose(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L763"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `truncate_bit_pattern`

```python
truncate_bit_pattern(
    ctx: Context,
    node: Node,
    preds: List[Conversion]
) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L767"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `where`

```python
where(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/mlir/converter.py#L771"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `zeros`

```python
zeros(ctx: Context, node: Node, preds: List[Conversion]) → Conversion
```






