<!-- markdownlint-disable -->

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.fhe.compilation.configuration`
Declaration of `Configuration` class. 

**Global Variables**
---------------
- **MAXIMUM_TLU_BIT_WIDTH**
- **DEFAULT_P_ERROR**
- **DEFAULT_GLOBAL_P_ERROR**


---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L22"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ParameterSelectionStrategy`
ParameterSelectionStrategy, to set optimization strategy. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L49"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MultiParameterStrategy`
MultiParamStrategy, to set optimization strategy for multi-parameter. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L75"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ComparisonStrategy`
ComparisonStrategy, to specify implementation preference for comparisons. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L447"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `BitwiseStrategy`
BitwiseStrategy, to specify implementation preference for bitwise operations. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L645"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MultivariateStrategy`
MultivariateStrategy, to specify implementation preference for multivariate operations. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L754"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MinMaxStrategy`
MinMaxStrategy, to specify implementation preference for minimum and maximum operations. 





---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L889"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `Configuration`
Configuration class, to allow the compilation process to be customized. 

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L935"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(
    verbose: bool = False,
    show_graph: Optional[bool] = None,
    show_bit_width_constraints: Optional[bool] = None,
    show_bit_width_assignments: Optional[bool] = None,
    show_assigned_graph: Optional[bool] = None,
    show_mlir: Optional[bool] = None,
    show_optimizer: Optional[bool] = None,
    show_statistics: Optional[bool] = None,
    dump_artifacts_on_unexpected_failures: bool = True,
    enable_unsafe_features: bool = False,
    use_insecure_key_cache: bool = False,
    insecure_key_cache_location: Optional[Path, str] = None,
    loop_parallelize: bool = True,
    dataflow_parallelize: bool = False,
    auto_parallelize: bool = False,
    compress_evaluation_keys: bool = False,
    p_error: Optional[float] = None,
    global_p_error: Optional[float] = None,
    auto_adjust_rounders: bool = False,
    auto_adjust_truncators: bool = False,
    single_precision: bool = False,
    parameter_selection_strategy: Union[ParameterSelectionStrategy, str] = <ParameterSelectionStrategy.MULTI: 'multi'>,
    multi_parameter_strategy: Union[MultiParameterStrategy, str] = <MultiParameterStrategy.PRECISION: 'precision'>,
    show_progress: bool = False,
    progress_title: str = '',
    progress_tag: Union[bool, int] = False,
    fhe_simulation: bool = False,
    fhe_execution: bool = True,
    compiler_debug_mode: bool = False,
    compiler_verbose_mode: bool = False,
    comparison_strategy_preference: Optional[ComparisonStrategy, str, List[Union[ComparisonStrategy, str]]] = None,
    bitwise_strategy_preference: Optional[BitwiseStrategy, str, List[Union[BitwiseStrategy, str]]] = None,
    shifts_with_promotion: bool = True,
    multivariate_strategy_preference: Optional[MultivariateStrategy, str, List[Union[MultivariateStrategy, str]]] = None,
    min_max_strategy_preference: Optional[MinMaxStrategy, str, List[Union[MinMaxStrategy, str]]] = None,
    composable: bool = False,
    use_gpu: bool = False,
    relu_on_bits_threshold: int = 7,
    relu_on_bits_chunk_size: int = 3,
    if_then_else_chunk_size: int = 3
)
```








---

<a href="../../../../concrete-ml/.venv/lib/python3.9/site-packages/concrete/fhe/compilation/configuration.py#L1080"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `fork`

```python
fork(
    verbose: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_graph: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_bit_width_constraints: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_bit_width_assignments: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_assigned_graph: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_mlir: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_optimizer: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_statistics: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    dump_artifacts_on_unexpected_failures: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    enable_unsafe_features: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    use_insecure_key_cache: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    insecure_key_cache_location: Optional[Keep, Path, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    loop_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    dataflow_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    auto_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    compress_evaluation_keys: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    p_error: Optional[Keep, float] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    global_p_error: Optional[Keep, float] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    auto_adjust_rounders: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    auto_adjust_truncators: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    single_precision: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    parameter_selection_strategy: Union[Keep, ParameterSelectionStrategy, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    multi_parameter_strategy: Union[Keep, MultiParameterStrategy, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    show_progress: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    progress_title: Union[Keep, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    progress_tag: Union[Keep, bool, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    fhe_simulation: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    fhe_execution: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    compiler_debug_mode: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    compiler_verbose_mode: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    comparison_strategy_preference: Optional[Keep, ComparisonStrategy, str, List[Union[ComparisonStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    bitwise_strategy_preference: Optional[Keep, BitwiseStrategy, str, List[Union[BitwiseStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    shifts_with_promotion: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    multivariate_strategy_preference: Optional[Keep, MultivariateStrategy, str, List[Union[MultivariateStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    min_max_strategy_preference: Optional[Keep, MinMaxStrategy, str, List[Union[MinMaxStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    composable: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    use_gpu: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    relu_on_bits_threshold: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    relu_on_bits_chunk_size: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>,
    if_then_else_chunk_size: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x14124a0a0>
) → Configuration
```

Get a new configuration from another one specified changes. 

See Configuration. 


