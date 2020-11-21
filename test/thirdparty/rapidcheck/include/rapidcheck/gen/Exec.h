#pragma once

#include "rapidcheck/detail/FunctionTraits.h"

namespace rc {
namespace gen {

/// Creates a generator from a callable that returns a plain value of type `T`.
/// Inside this callable, `operator*` of `Gen` can be used to pick values from
/// other generators which together then form the final value. This way, the
/// parameters of one generator can also depend on the value of a previous
/// generator. Because of this, values from one generator are always assumed to
/// be dependant on those generated by previous generators which is reflected
/// when shrinking
template <typename Callable>
Gen<Decay<rc::detail::ReturnType<Callable>>> exec(Callable &&callable);

} // namespace gen
} // namespace rc

#include "Exec.hpp"
