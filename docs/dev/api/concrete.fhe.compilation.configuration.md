<!-- markdownlint-disable -->

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L0"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

# <kbd>module</kbd> `concrete.fhe.compilation.configuration`
Declaration of `Configuration` class. 

**Global Variables**
---------------
- **MAXIMUM_TLU_BIT_WIDTH**
- **DEFAULT_P_ERROR**
- **DEFAULT_GLOBAL_P_ERROR**


---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L24"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ParameterSelectionStrategy`
ParameterSelectionStrategy, to set optimization strategy. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L51"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MultiParameterStrategy`
MultiParamStrategy, to set optimization strategy for multi-parameter. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L77"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `Exactness`
Exactness, to specify for specific operator the implementation preference (default and local). 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L86"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ApproximateRoundingConfig`
Controls the behavior of approximate rounding. 

In the following `k` is the ideal rounding output precision. Often the precision used after rounding is `k`+1 to avoid overflow. `logical_clipping`, `approximate_clipping_start_precision` can be used to stay at precision `k`, either logically or physically at the successor TLU. See examples in https://github.com/zama-ai/concrete/blob/main/docs/tutorial/rounding.md. 

<a href="../../tempdirectoryforapidocs/<string>"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `__init__`

```python
__init__(
    logical_clipping: bool = True,
    approximate_clipping_start_precision: int = 5,
    reduce_precision_after_approximate_clipping: bool = True,
    symetrize_deltas: bool = True
) → None
```









---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L125"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `ComparisonStrategy`
ComparisonStrategy, to specify implementation preference for comparisons. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L497"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `BitwiseStrategy`
BitwiseStrategy, to specify implementation preference for bitwise operations. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L695"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MultivariateStrategy`
MultivariateStrategy, to specify implementation preference for multivariate operations. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L804"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `MinMaxStrategy`
MinMaxStrategy, to specify implementation preference for minimum and maximum operations. 





---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L939"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

## <kbd>class</kbd> `Configuration`
Configuration class, to allow the compilation process to be customized. 

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L988"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

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
    if_then_else_chunk_size: int = 3,
    additional_processors: Optional[List[GraphProcessor]] = None,
    rounding_exactness: Exactness = <Exactness.EXACT: 0>,
    approximate_rounding_config: Optional[ApproximateRoundingConfig] = None
)
```








---

<a href="../../tempdirectoryforapidocs/.venvtrash/lib/python3.10/site-packages/concrete/fhe/compilation/configuration.py#L1141"><img align="right" style="float:right;" src="https://img.shields.io/badge/-source-cccccc?style=flat-square"></a>

### <kbd>method</kbd> `fork`

```python
fork(
    verbose: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_graph: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_bit_width_constraints: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_bit_width_assignments: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_assigned_graph: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_mlir: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_optimizer: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_statistics: Optional[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    dump_artifacts_on_unexpected_failures: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    enable_unsafe_features: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    use_insecure_key_cache: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    insecure_key_cache_location: Optional[Keep, Path, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    loop_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    dataflow_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    auto_parallelize: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    compress_evaluation_keys: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    p_error: Optional[Keep, float] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    global_p_error: Optional[Keep, float] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    auto_adjust_rounders: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    auto_adjust_truncators: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    single_precision: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    parameter_selection_strategy: Union[Keep, ParameterSelectionStrategy, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    multi_parameter_strategy: Union[Keep, MultiParameterStrategy, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    show_progress: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    progress_title: Union[Keep, str] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    progress_tag: Union[Keep, bool, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    fhe_simulation: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    fhe_execution: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    compiler_debug_mode: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    compiler_verbose_mode: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    comparison_strategy_preference: Optional[Keep, ComparisonStrategy, str, List[Union[ComparisonStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    bitwise_strategy_preference: Optional[Keep, BitwiseStrategy, str, List[Union[BitwiseStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    shifts_with_promotion: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    multivariate_strategy_preference: Optional[Keep, MultivariateStrategy, str, List[Union[MultivariateStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    min_max_strategy_preference: Optional[Keep, MinMaxStrategy, str, List[Union[MinMaxStrategy, str]]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    composable: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    use_gpu: Union[Keep, bool] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    relu_on_bits_threshold: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    relu_on_bits_chunk_size: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    if_then_else_chunk_size: Union[Keep, int] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    additional_processors: Optional[Keep, List[GraphProcessor]] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    rounding_exactness: Union[Keep, Exactness] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>,
    approximate_rounding_config: Optional[Keep, ApproximateRoundingConfig] = <concrete.fhe.compilation.configuration.Configuration.Keep object at 0x142f83c10>
) → Configuration
```

Get a new configuration from another one specified changes. 

See Configuration. 


