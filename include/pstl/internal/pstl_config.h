/*
    Copyright (c) 2017 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.




*/

#ifndef __PSTL_config_H
#define __PSTL_config_H

#define PSTL_VERSION 102
#define PSTL_VERSION_MAJOR (PSTL_VERSION/100)
#define PSTL_VERSION_MINOR (PSTL_VERSION - PSTL_VERSION_MAJOR * 100)

#if _WIN32 && __PSTL_SHARED_LINKAGE
#if __PSTL_EXPORTS
#define __PSTL_API __declspec(dllexport)
#else
#define __PSTL_API __declspec(dllimport)
#endif
#else
#define __PSTL_API
#endif

#ifndef __PSTL_HEADER_ONLY
#define __PSTL_HEADER_ONLY 1
#endif

// Check the user-defined macro for parallel policies
#if defined(PSTL_USE_PARALLEL_POLICIES)
#undef __PSTL_USE_PAR_POLICIES
#define __PSTL_USE_PAR_POLICIES PSTL_USE_PARALLEL_POLICIES
// Check the internal macro for parallel policies
#elif !defined(__PSTL_USE_PAR_POLICIES) 
#define __PSTL_USE_PAR_POLICIES 1
#endif

#if __PSTL_USE_PAR_POLICIES
#if !defined(__PSTL_USE_TBB)
#define __PSTL_USE_TBB 1
#endif
#if !defined(__PSTL_USE_OPENMP)
#define __PSTL_USE_OPENMP 1
#endif
#else
#undef __PSTL_USE_OPENMP
#undef __PSTL_USE_TBB
#endif

// Portability "#pragma" definition
#ifdef _MSC_VER
#define __PSTL_PRAGMA(x) __pragma(x)
#else
#define __PSTL_PRAGMA(x) _Pragma (#x)
#endif

#define __PSTL_STRING_AUX(x) #x
#define __PSTL_STRING(x) __PSTL_STRING_AUX(x)
#define __PSTL_STRING_CONCAT(x, y) x#y

// note that when ICC or Clang is in use, __PSTL_GCC_VERSION might not fully match
// the actual GCC version on the system.
#define __PSTL_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

// Enable SIMD for compilers that support OpenMP 4.0
#if (_OPENMP >= 201307) || (__INTEL_COMPILER >= 1600) || (__PSTL_GCC_VERSION >= 40900)
#define __PSTL_PRAGMA_SIMD __PSTL_PRAGMA(omp simd)
#define __PSTL_PRAGMA_SIMD_REDUCTION(PRM) __PSTL_PRAGMA(omp simd reduction(PRM))
// Intel compiler supports "simd"
#elif defined(__INTEL_COMPILER)
#define __PSTL_PRAGMA_SIMD __PSTL_PRAGMA(simd)
#define __PSTL_PRAGMA_SIMD_REDUCTION(PRM) __PSTL_PRAGMA(simd reduction(PRM))
// GCC has "ivdep" in its namespace
#elif defined(__GNUC__) && defined(__GNUC_MINOR__) && \
      ( ( (__GNUC__ == 4) && (__GNUC_MINOR__ == 9) ) || (__GNUC__ >= 5) )
#define __PSTL_PRAGMA_SIMD __PSTL_PRAGMA(GCC ivdep)
#define __PSTL_PRAGMA_SIMD_REDUCTION(PRM)
// Clang has another syntax
#elif defined(__clang__)
#if defined(__clang_major__) && defined(__clang_minor__) && \
    ( (__clang_major__ >= 4 ) ||  (__clang_major__ >= 3 && __clang_minor__ > 7) )
#define __PSTL_PRAGMA_SIMD __PSTL_PRAGMA(clang loop vectorize(assume_safety))
#else
#define __PSTL_PRAGMA_SIMD __PSTL_PRAGMA(clang loop vectorize(enable))
#endif
#else //no simd
#define __PSTL_PRAGMA_SIMD
#define __PSTL_PRAGMA_SIMD_REDUCTION(PRM)
#endif //Enable SIMD

// Should be defined to 1 for environments with a vendor implementation of C++17 execution policies
#define __PSTL_CPP17_EXECUTION_POLICIES_PRESENT 0

#define __PSTL_CPP14_2RANGE_MISMATCH_EQUAL_PRESENT (_MSC_VER >= 1900 || __cplusplus >= 201300L || __cpp_lib_robust_nonmodifying_seq_ops == 201304)
#define __PSTL_CPP14_MAKE_REVERSE_ITERATOR_PRESENT (_MSC_VER >= 1900 || __cplusplus >= 201402L || __cpp_lib_make_reverse_iterator == 201402)
#define __PSTL_CPP14_INTEGER_SEQUENCE_PRESENT (_MSC_VER >= 1900 || __cplusplus >= 201402L)
#define __PSTL_CPP14_VARIABLE_TEMPLATES_PRESENT \
    (!__INTEL_COMPILER || __INTEL_COMPILER >= 1700) && (_MSC_FULL_VER >= 190023918 || __cplusplus >= 201402L)

#define __PSTL_EARLYEXIT_PRESENT  (__INTEL_COMPILER >= 1800)
#define __PSTL_MONOTONIC_PRESENT (__INTEL_COMPILER >= 1800)

#if __PSTL_EARLYEXIT_PRESENT
#define __PSTL_PRAGMA_SIMD_EARLYEXIT __PSTL_PRAGMA(omp simd early_exit)
#else
#define __PSTL_PRAGMA_SIMD_EARLYEXIT
#endif

#if __PSTL_MONOTONIC_PRESENT
#define __PSTL_PRAGMA_SIMD_ORDERED_MONOTONIC(PRM) __PSTL_PRAGMA(omp ordered simd monotonic(PRM))
#define __PSTL_PRAGMA_SIMD_ORDERED_MONOTONIC_2ARGS(PRM1, PRM2) __PSTL_PRAGMA(omp ordered simd monotonic(PRM1, PRM2))
#else
#define __PSTL_PRAGMA_SIMD_ORDERED_MONOTONIC(PRM)
#define __PSTL_PRAGMA_SIMD_ORDERED_MONOTONIC_2ARGS(PRM1, PRM2)
#endif

#if (__INTEL_COMPILER >= 1600)
#define __PSTL_PRAGMA_VECTOR_UNALIGNED __PSTL_PRAGMA(vector unaligned)
#else
#define __PSTL_PRAGMA_VECTOR_UNALIGNED
#endif

// Check the user-defined macro to use non-temporal stores
#if defined(PSTL_USE_NONTEMPORAL_STORES) && (__INTEL_COMPILER >= 1600)
#define __PSTL_USE_NONTEMPORAL_STORES_IF_ALLOWED __PSTL_PRAGMA(vector nontemporal)
#else
#define __PSTL_USE_NONTEMPORAL_STORES_IF_ALLOWED
#endif

#if _MSC_VER || __INTEL_COMPILER //the preprocessors don't type a message location
#define __PSTL_PRAGMA_LOCATION __FILE__ ":" __PSTL_STRING(__LINE__) ": warning: "
#else
#define __PSTL_PRAGMA_LOCATION
#endif

#define __PSTL_PRAGMA_MESSAGE(x)

#if defined(__GLIBCXX__)
#define __PSTL_CPP11_STD_ROTATE_BROKEN (__PSTL_GCC_VERSION < 50100) //GCC 5.1 release
#endif

#endif /* __PSTL_config_H */
