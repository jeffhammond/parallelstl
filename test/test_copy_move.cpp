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

// Tests for copy, move and copy_n

#include "test/pstl_test_config.h"

#include "pstl/execution"
#include "pstl/algorithm"
#include "test/utils.h"

using namespace TestUtils;

const size_t GuardSize = 5;

struct run_copy {

#if __PSTL_TEST_SIMD_LAMBDA_ICC_17_VC141_DEBUG_32_BROKEN || __PSTL_TEST_SIMD_LAMBDA_ICC_16_VC14_DEBUG_32_BROKEN //dummy specialization by policy type, in case of broken configuration 
    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size, typename T>
    void operator()(pstl::execution::unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {}

    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size, typename T>
    void operator()(pstl::execution::parallel_unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {}
#endif

    template<typename Policy, typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size, typename T>
    void operator()(Policy&& exec, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {
        // Cleaning
        std::fill_n(expected_first, size, trash);
        std::fill_n(out_first, size, trash);

        // Run copy
        copy(first, last, expected_first);
        auto k = copy(exec, first, last, out_first);
        for (size_t j = 0; j < GuardSize; ++j)
            ++k;
        EXPECT_EQ_N(expected_first, out_first, size, "wrong effect from copy");
        EXPECT_TRUE(out_last == k, "wrong return value from copy");

        // Cleaning
        std::fill_n(out_first, size, trash);
        // Run copy_n
        k = copy_n(exec, first, n, out_first);
        for (size_t j = 0; j < GuardSize; ++j)
            ++k;
        EXPECT_EQ_N(expected_first, out_first, size, "wrong effect from copy_n");
        EXPECT_TRUE(out_last == k, "wrong return value from copy_n");
    }
};

template <typename T>
struct run_move {

#if __PSTL_TEST_SIMD_LAMBDA_ICC_17_VC141_DEBUG_32_BROKEN || __PSTL_TEST_SIMD_LAMBDA_ICC_16_VC14_DEBUG_32_BROKEN//dummy specialization by policy type, in case of broken configuration 
    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(pstl::execution::unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {}

    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(pstl::execution::parallel_unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {}
#endif

    template<typename Policy, typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(Policy&& exec, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, T trash) {
        // Cleaning
        std::fill_n(expected_first, size, trash);
        std::fill_n(out_first, size, trash);

        // Run move
        move(first, last, expected_first);
        auto k = move(exec, first, last, out_first);
        for (size_t j = 0; j < GuardSize; ++j)
            ++k;
        EXPECT_EQ_N(expected_first, out_first, size, "wrong effect from move");
        EXPECT_TRUE(out_last == k, "wrong return value from move");
    }
};

template <typename T>
struct run_move<Wrapper<T>> {

#if __PSTL_TEST_SIMD_LAMBDA_ICC_17_VC141_DEBUG_32_BROKEN || __PSTL_TEST_SIMD_LAMBDA_ICC_16_VC14_DEBUG_32_BROKEN//dummy specialization by policy type, in case of broken configuration 
    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(pstl::execution::unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, Wrapper<T> trash) {}

    template<typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(pstl::execution::parallel_unsequenced_policy, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, Wrapper<T> trash) {}
#endif

    template<typename Policy, typename InputIterator, typename OutputIterator, typename OutputIterator2, typename Size>
    void operator()(Policy&& exec, InputIterator first, InputIterator last, OutputIterator out_first, OutputIterator out_last,
        OutputIterator2 expected_first, Size size, Size n, Wrapper<T> trash) {
        // Cleaning
        std::fill_n(out_first, size, trash);
        Wrapper<T>::SetMoveCount(0);

        // Run move
        auto k = move(exec, first, last, out_first);
        for (size_t j = 0; j < GuardSize; ++j)
            ++k;
        EXPECT_TRUE(Wrapper<T>::MoveCount() == size, "wrong effect from move");
        EXPECT_TRUE(out_last == k, "wrong return value from move");
    }
};

template<typename T, typename Convert>
void test(T trash, Convert convert) {
    // Try sequences of various lengths.
    for (size_t n = 0; n <= 100000; n = n <= 16 ? n + 1 : size_t(3.1415 * n)) {
        // count is number of output elements, plus a handful
        // more for sake of detecting buffer overruns.
        Sequence<T> in(n, [&](size_t k) -> T {
            T val = convert(n^k);
            return val;
        });

        size_t outN = n + GuardSize;
        Sequence<T> out(outN, [=](size_t) {return trash; });
        Sequence<T> expected(outN, [=](size_t) {return trash; });
        invoke_on_all_policies(run_copy(), in.begin(), in.end(), out.begin(), out.end(), expected.begin(), outN, n, trash);
        invoke_on_all_policies(run_copy(), in.cbegin(), in.cend(), out.begin(), out.end(), expected.begin(), outN, n, trash);
        invoke_on_all_policies(run_move<T>(), in.begin(), in.end(), out.begin(), out.end(), expected.begin(), n, n, trash);

        // For this test const iterator isn't suitable
        // because const rvalue-reference call copy assignment operator
    }
}

int32_t main() {
    test<int32_t>(-666,
        [](size_t j) {return int32_t(j); });
    test<Wrapper<float64_t>>(Wrapper<float64_t>(-666.0),
        [](int32_t j) { return Wrapper<float64_t>(j); });

#if !__PSTL_TEST_ICC_16_17_64_TIMEOUT
    test<float64_t>(-666.0,
        [](size_t j) {return float64_t(j); });
    test<Number>(Number(42, OddTag()),
        [](int32_t j) {return Number(j, OddTag()); });
#endif
    std::cout << "done" << std::endl;
    return 0;
}