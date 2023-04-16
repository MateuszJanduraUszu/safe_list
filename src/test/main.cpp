// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <safe_list.hpp>
#include <gtest/gtest.h>

namespace tests {
    using ::mjx::safe_list;

    struct _Sample_data {
        static constexpr size_t _Size      = 10;
        static constexpr int _Array[_Size] = {
            251, 515, 25, 16232, 5156, 2551, 251, 5621, 6722, 915
        };

        static constexpr const int* _Begin() noexcept {
            return _Array;
        }

        static constexpr const int* _End() noexcept {
            return _Array + _Size;
        }

        static constexpr ::std::initializer_list<int> _Init_list() noexcept {
            return ::std::initializer_list<int>(_Array, _Array + _Size);
        }
    };

    inline namespace accessors {
        TEST(accessors, non_empty_list_front) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.pop_front();
            _ASSERTE(_List.front() != nullptr);
            GTEST_EXPECT_TRUE(*_List.front() == _Sample_data::_Array[1]);
        }

        TEST(accessors, empty_list_front) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.clear();
            GTEST_EXPECT_TRUE(_List.front() == nullptr);
        }

        TEST(accessors, non_empty_list_back) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.pop_back();
            _ASSERTE(_List.back() != nullptr);
            GTEST_EXPECT_TRUE(*_List.back() == _Sample_data::_Array[_Sample_data::_Size - 2]);
        }

        TEST(accessors, empty_list_back) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.clear();
            GTEST_EXPECT_TRUE(_List.back() == nullptr);
        }
    } // namespace accessors

    inline namespace capacity {
        TEST(capacity, ctor_non_empty_list) {
            safe_list<int> _List(10, 251);
            GTEST_EXPECT_TRUE(!_List.empty() && _List.size() == 10);
        }

        TEST(capacity, non_empty_list) {
            safe_list<int> _List;
            _ASSERTE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            GTEST_EXPECT_TRUE(!_List.empty());
        }

        TEST(capacity, empty_list) {
            safe_list<int> _List;
            _ASSERTE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            _List.clear();
            GTEST_EXPECT_TRUE(_List.empty());
        }

        TEST(capacity, correct_size) {
            safe_list<int> _List;
            _ASSERTE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            _List.pop_back();
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size - 1);
        }
    } // namespace capacity
} // namespace tests

int main() {
    ::testing::InitGoogleTest();
    return ::RUN_ALL_TESTS();
}