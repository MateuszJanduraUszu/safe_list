// main.cpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#include <safe_list.hpp>
#include <gtest/gtest.h>

namespace tests {
    using ::mjx::safe_list;

    template <class _InIt1, class _InIt2>
    constexpr bool _Compare_arrays(_InIt1 _Left_first, const _InIt1 _Left_last, _InIt2 _Right) noexcept {
        for (; _Left_first != _Left_last; ++_Left_first, ++_Right) {
            if (*_Left_first != *_Right) {
                return false;
            }
        }

        return true;
    }

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
            GTEST_ASSERT_TRUE(_List.front() != nullptr);
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
            GTEST_ASSERT_TRUE(_List.back() != nullptr);
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
            GTEST_EXPECT_TRUE(!_List.empty());
            GTEST_EXPECT_TRUE(_List.size() == 10);
        }

        TEST(capacity, non_empty_list) {
            safe_list<int> _List;
            GTEST_ASSERT_TRUE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            GTEST_EXPECT_TRUE(!_List.empty());
        }

        TEST(capacity, empty_list) {
            safe_list<int> _List;
            GTEST_ASSERT_TRUE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            _List.clear();
            GTEST_EXPECT_TRUE(_List.empty());
        }

        TEST(capacity, correct_size) {
            safe_list<int> _List;
            GTEST_ASSERT_TRUE(_List.assign(_Sample_data::_Begin(), _Sample_data::_End()));
            _List.pop_back();
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size - 1);
        }
    } // namespace capacity

    inline namespace modifiers {
        TEST(modifiers, clear_non_empty) {
            safe_list<int> _List(_Sample_data::_Init_list());
            GTEST_ASSERT_TRUE(_List.push_back(749));
            _List.clear();
            GTEST_EXPECT_TRUE(_List.empty());
        }

        TEST(modifiers, insert_begin) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Array_size            = 3;
            constexpr int _Array[_Array_size]       = {8842, 6991, 5010};
            constexpr size_t _Expected_size         = _Sample_data::_Size + 7;
            constexpr int _Expected[_Expected_size] = {
                8842, 6991, 5010, 96121, 96121, 96121, 851, 251, 515, 25, 16232, 5156, 2551, 251, 5621, 6722, 915
            };
            GTEST_ASSERT_TRUE(_List.insert(_List.cbegin(), 851).valid());
            GTEST_ASSERT_TRUE(_List.insert(_List.cbegin(), size_t{3}, 96121).valid());
            GTEST_ASSERT_TRUE(_List.insert(_List.cbegin(), _Array, _Array + _Array_size).valid());
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, insert_inside) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Array_size            = 3;
            constexpr int _Array[_Array_size]       = {8842, 6991, 5010};
            constexpr size_t _Expected_size         = _Sample_data::_Size + 7;
            constexpr int _Expected[_Expected_size] = {
                251, 8842, 6991, 5010, 96121, 96121, 96121, 851, 515, 25, 16232, 5156, 2551, 251, 5621, 6722, 915
            };
            GTEST_ASSERT_TRUE(_List.insert(++_List.cbegin(), 851).valid());
            GTEST_ASSERT_TRUE(_List.insert(++_List.cbegin(), size_t{3}, 96121).valid());
            GTEST_ASSERT_TRUE(_List.insert(++_List.cbegin(), _Array, _Array + _Array_size).valid());
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, insert_end) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Array_size            = 3;
            constexpr int _Array[_Array_size]       = {8842, 6991, 5010};
            constexpr size_t _Expected_size         = _Sample_data::_Size + 7;
            constexpr int _Expected[_Expected_size] = {
                251, 515, 25, 16232, 5156, 2551, 251, 5621, 6722, 915, 851, 96121, 96121, 96121, 8842, 6991, 5010
            };
            GTEST_ASSERT_TRUE(_List.insert(_List.cend(), 851).valid());
            GTEST_ASSERT_TRUE(_List.insert(_List.cend(), size_t{3}, 96121).valid());
            GTEST_ASSERT_TRUE(_List.insert(_List.cend(), _Array, _Array + _Array_size).valid());
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, erase_empty_list) {
            safe_list<int> _List;
            GTEST_EXPECT_TRUE(!_List.erase(_List.cbegin()).valid()); // should points to a null-pointer
        }

        TEST(modifiers, erase_begin) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.erase(_List.cbegin());
            GTEST_EXPECT_TRUE(*_List.cbegin() == 515);
        }

        TEST(modifiers, erase_end) {
            safe_list<int> _List(_Sample_data::_Init_list());
            _List.erase(_List.cend());
            auto _Iter = _List.cbegin();
            while (_Iter._Get_node()->_Next) { // find last valid node
                ++_Iter;
            }

            GTEST_EXPECT_TRUE(*_Iter == 6722);
        }

        TEST(modifiers, erase_range) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Expected_size         = _Sample_data::_Size - 4;
            constexpr int _Expected[_Expected_size] = {251, 2551, 251, 5621, 6722, 915};
            auto _Last                              = _List.cbegin();
            for (uint8_t _Count = 0; _Count < 5; ++_Count) { // find 6-th node
                ++_Last;
            }

            _List.erase(++_List.cbegin(), _Last);
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, push_back_empty_list) {
            safe_list<int> _List;
            GTEST_ASSERT_TRUE(_List.push_back(4512));
            GTEST_ASSERT_TRUE(_List.push_back(6122));
            GTEST_ASSERT_TRUE(_List.front() != nullptr);
            GTEST_ASSERT_TRUE(_List.back() != nullptr);
            GTEST_EXPECT_TRUE(_List.size() == 2);
            GTEST_EXPECT_TRUE(*_List.front() == 4512);
            GTEST_EXPECT_TRUE(*_List.back() == 6122);
        }

        TEST(modifiers, push_back_non_empty_list) {
            safe_list<int> _List(_Sample_data::_Init_list());
            GTEST_ASSERT_TRUE(_List.push_back(4512));
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size + 1);
            GTEST_EXPECT_TRUE(*_List.back() == 4512);
            GTEST_ASSERT_TRUE(_List.push_back(6122));
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size + 2);
            GTEST_EXPECT_TRUE(*_List.back() == 6122);
        }

        TEST(modifiers, push_front_empty_list) {
            safe_list<int> _List;
            GTEST_ASSERT_TRUE(_List.push_front(4512));
            GTEST_ASSERT_TRUE(_List.push_front(6122));
            GTEST_ASSERT_TRUE(_List.front() != nullptr);
            GTEST_ASSERT_TRUE(_List.back() != nullptr);
            GTEST_EXPECT_TRUE(_List.size() == 2);
            GTEST_EXPECT_TRUE(*_List.front() == 6122);
            GTEST_EXPECT_TRUE(*_List.back() == 4512);
        }

        TEST(modifiers, push_front_non_empty_list) {
            safe_list<int> _List(_Sample_data::_Init_list());
            GTEST_ASSERT_TRUE(_List.push_front(4512));
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size + 1);
            GTEST_EXPECT_TRUE(*_List.front() == 4512);
            GTEST_ASSERT_TRUE(_List.push_front(6122));
            GTEST_EXPECT_TRUE(_List.size() == _Sample_data::_Size + 2);
            GTEST_EXPECT_TRUE(*_List.front() == 6122);
        }

        TEST(modifiers, pop_back) {
            safe_list<int> _List(_Sample_data::_Init_list());
            for (uint8_t _Count = 0; _Count < 5; ++_Count) {
                _List.pop_back();
            }

            GTEST_EXPECT_TRUE(_List.size() == 5);
            GTEST_EXPECT_TRUE(*_List.back() == 5156);
        }

        TEST(modifiers, pop_front) {
            safe_list<int> _List(_Sample_data::_Init_list());
            for (uint8_t _Count = 0; _Count < 5; ++_Count) {
                _List.pop_front();
            }

            GTEST_EXPECT_TRUE(_List.size() == 5);
            GTEST_EXPECT_TRUE(*_List.front() == 2551);
        }

        TEST(modifiers, resize_empty) {
            safe_list<int> _List;
            constexpr size_t _Expected_size         = 5;
            constexpr int _Expected[_Expected_size] = {551, 551, 551, 551, 551};
            GTEST_ASSERT_TRUE(_List.resize(5, 551));
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, resize_increase_capacity) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Expected_size         = 15;
            constexpr int _Expected[_Expected_size] = {
                251, 515, 25, 16232, 5156, 2551, 251, 5621, 6722, 915, 49, 49, 49, 49, 49};
            GTEST_ASSERT_TRUE(_List.resize(_Expected_size, 49));
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(modifiers, resize_decrease_capacity) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Expected_size         = 5;
            constexpr int _Expected[_Expected_size] = {251, 515, 25, 16232, 5156};
            GTEST_ASSERT_TRUE(_List.resize(_Expected_size));
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }
    } // namespace modifiers

    inline namespace operations {
        TEST(operations, remove_exact_value) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Expected_size         = _Sample_data::_Size - 4;
            constexpr int _Expected[_Expected_size] = {515, 25, 16232, 2551, 5621, 6722};
            _List.remove(251);
            _List.remove(5156);
            _List.remove(915);
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(operations, remove_predicted) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr size_t _Expected_size         = 7;
            constexpr int _Expected[_Expected_size] = {
                251, 515, 25, 2551, 251, 5621, 915
            };
            _List.remove_if(
                [](const int _Value) noexcept {
                    return _Value % 2 == 0;
                }
            );
            GTEST_EXPECT_TRUE(_List.size() == _Expected_size);
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }

        TEST(operations, reverse) {
            safe_list<int> _List(_Sample_data::_Init_list());
            constexpr int _Expected[_Sample_data::_Size] = {
                915, 6722, 5621, 251, 2551, 5156, 16232, 25, 515, 251
            };
            _List.reverse();
            GTEST_EXPECT_TRUE(_Compare_arrays(_List.begin(), _List.end(), _Expected));
        }
    } // namespace operations
} // namespace tests

int main() {
    ::testing::InitGoogleTest();
    return ::RUN_ALL_TESTS();
}