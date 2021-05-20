// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef HAVE_CPP_STDLIB
#  include "opentelemetry/std/variant.h"
#else
// TODO: we use a LOCAL snapshot of Abseil that is known to compile with Visual Studio 2015,
// header-only, without compiling the actual Abseil binary. As Abseil moves on to new
// toolchains, it may drop support for Visual Studio 2015 in future versions. Perhaps a good
// option would be to determine if Abseil is available, then use the outside implementation,
// but it is not guaranteed to be still compatible with 2015. Thus, the local snapshot here.
#  ifdef _MSC_VER
// Abseil variant implementation contains some benign non-impacting warnings
// that should be suppressed if compiling with Visual Studio 2017 and above.
#    pragma warning(push)
#    pragma warning(disable : 4245)  // conversion from int to const unsigned _int64
#    pragma warning(disable : 4127)  // conditional expression is constant
#  endif
#  include "./absl/types/variant.h"
#  ifdef _MSC_VER
#    pragma warning(pop)
#  endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace nostd
{
using absl::get;
using absl::holds_alternative;
using absl::monostate;
using absl::variant;
using absl::variant_size;
using absl::visit;

// nostd::bad_variant_access
class bad_variant_access : public std::exception
{
public:
  virtual const char *what() const noexcept override { return "bad_variant_access"; }
};

[[noreturn]] inline void throw_bad_variant_access()
{
  throw bad_variant_access{};
}

#  if __EXCEPTIONS
#    define THROW_BAD_VARIANT_ACCESS opentelemetry::nostd::throw_bad_variant_access()
#  else
#    define THROW_BAD_VARIANT_ACCESS std::terminate()
#  endif

}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE

// TODO: when we compile with Abseil variant we do not have to link the entire Abseil library.
// The only missing funcion is the variant exception handler here, which we statically link
// as-needed. This *should not* clash with the standard handler. However, it would make sense
// to consider a build-time flag that allows to route the handling of ThrowBadVariantAccess to
// the actual Abseil, in case if a product using OpenTelemetry is compiled with Abseil. That
// way the standard Abseil exception handler can be used to catch OpenTelemetry exceptions.
namespace absl
{
namespace variant_internal
{
#  ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4211)  // nonstandard extension used: redefined extern to static
#  endif
static void ThrowBadVariantAccess()
{
  THROW_BAD_VARIANT_ACCESS;
};
#  ifdef _MSC_VER
#    pragma warning(pop)
#  endif
};  // namespace variant_internal
};  // namespace absl

#endif
