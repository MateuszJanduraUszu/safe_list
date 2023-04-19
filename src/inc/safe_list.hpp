// safe_list.hpp

// Copyright (c) Mateusz Jandura. All rights reserved.
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef _SAFE_LIST_HPP_
#define _SAFE_LIST_HPP_
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

namespace mjx {
    template <class _Ty, class... _Types>
    [[nodiscard]] constexpr _Ty* _Construct_object(const _Types&... _Args) noexcept {
        void* const _Raw = ::operator new(sizeof(_Ty), ::std::nothrow);
        return _Raw ? ::new (_Raw) _Ty(_Args...) : nullptr;
    }

    template <class _Ty, class... _Types>
    [[nodiscard]] constexpr _Ty* _Construct_object(_Types&&... _Args) noexcept {
        void* const _Raw = ::operator new(sizeof(_Ty), ::std::nothrow);
        return _Raw ? ::new (_Raw) _Ty(::std::forward<_Types>(_Args)...) : nullptr;
    }

    template <class _Ty>
    constexpr void _Destroy_object(_Ty* const _Ptr) noexcept {
        if (_Ptr) {
            _Ptr->~_Ty();
            ::operator delete(_Ptr, ::std::nothrow);
        }
    }

    template <class _Ty>
    struct _Safe_list_traits { // traits for safe_list<T>
        static constexpr bool _Is_nothrow_default_constructible // checks default construction safety
            = ::std::is_nothrow_default_constructible_v<_Ty>;

        static constexpr bool _Is_nothrow_copy_constructible // checks copy construction safety
            = ::std::is_nothrow_copy_constructible_v<_Ty>;

        static constexpr bool _Is_nothrow_move_constructible // checks move construction safety
            = ::std::is_nothrow_move_constructible_v<_Ty>;
    
        static constexpr bool _Is_nothrow_swappable // checks swap safety
            = ::std::is_nothrow_swappable_v<_Ty>;

        template <class... _Types>
        static constexpr bool _Is_nothrow_constructible // checks variadic-args construciton safety
            = ::std::is_nothrow_constructible_v<_Ty, _Types...>;

        template <class... _Types>
        static constexpr bool _Is_constructible // checks whether _Ty can be constructed from _Types...
            = ::std::is_constructible_v<_Ty, _Types...>;
    };

    template <class _Ty, class _Traits>
    class _Safe_list_node {
    public:
        _Safe_list_node* _Next; // pointer to the next node
        _Safe_list_node* _Prev; // pointer to the previous node
        _Ty _Value; // the stored value

        static_assert(_Traits::template _Is_constructible<> // default constructible
            && _Traits::template _Is_constructible<const _Ty&> // copy/move constructible
            && _Traits::template _Is_constructible<_Ty&&>, "T must be constructible.");

        _Safe_list_node() noexcept(_Traits::_Is_nothrow_default_constructible)
            : _Next(nullptr), _Prev(nullptr), _Value() {}

        ~_Safe_list_node() noexcept {}

        explicit _Safe_list_node(const _Ty& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible)
            : _Next(nullptr), _Prev(nullptr), _Value(_Value) {}

        explicit _Safe_list_node(_Ty&& _Value) noexcept(_Traits::_Is_nothrow_move_constructible)
            : _Next(nullptr), _Prev(nullptr), _Value(::std::move(_Value)) {}
    };

    template <class _Ty, class _Traits, class _Size_type>
    class _Safe_list_storage {
    public:
        using _Value_type = _Ty;
        using _Node_type  = _Safe_list_node<_Ty, _Traits>;

        _Node_type* _Head; // pointer to the first node
        _Node_type* _Tail; // pointer to the last node
        _Size_type _Size; // number of nodes

        _Safe_list_storage() noexcept : _Head(nullptr), _Tail(nullptr), _Size(0) {}

        void _Swap(_Safe_list_storage& _Other) noexcept {
            ::std::swap(_Head, _Other._Head);
            ::std::swap(_Tail, _Other._Tail);
            ::std::swap(_Size, _Other._Size);
        }
    };

    template <class _List, class _Traits>
    class _Safe_list_iterator_base { // base class for all list iterators
    public:
        using value_type        = typename _List::value_type;
        using difference_type   = typename _List::difference_type;
        using iterator_category = ::std::bidirectional_iterator_tag;

    protected:
        using _Node_t = _Safe_list_node<value_type, _Traits>;

    public:
        _Safe_list_iterator_base() noexcept : _Node(nullptr) {}

        explicit _Safe_list_iterator_base(_Node_t* const _Node) noexcept : _Node(_Node) {}

        ~_Safe_list_iterator_base() noexcept {}

        explicit operator bool() const noexcept {
            return _Node != nullptr;
        }

        bool valid() const noexcept {
            return _Node != nullptr;
        }

        bool operator==(const _Safe_list_iterator_base& _Other) const noexcept {
            return _Node == _Other._Node;
        }

        bool operator!=(const _Safe_list_iterator_base& _Other) const noexcept {
            return _Node != _Other._Node;
        }

        _Node_t* _Get_node() noexcept {
            return _Node;
        }

        const _Node_t* _Get_node() const noexcept {
            return _Node;
        }

    protected:
        _Node_t* _Node;
    };

    template <class _List, class _Traits>
    class _Safe_list_iterator
        : public _Safe_list_iterator_base<_List, _Traits> { // mutable list iterator
    private:
        using _Mybase = _Safe_list_iterator_base<_List, _Traits>;
        using _Node_t = typename _Mybase::_Node_t;

    public:
        using value_type        = typename _Mybase::value_type;
        using difference_type   = typename _Mybase::difference_type;
        using pointer           = typename _List::pointer;
        using reference         = typename _List::reference;
        using iterator_category = typename _Mybase::iterator_category;

        _Safe_list_iterator() noexcept : _Mybase() {}

        explicit _Safe_list_iterator(_Node_t* const _Node) noexcept : _Mybase(_Node) {}

        ~_Safe_list_iterator() noexcept {}
    
        reference operator*() const noexcept {
            return this->_Node->_Value;
        }

        pointer operator->() const noexcept {
            return ::std::addressof(this->_Node->_Value);
        }

        _Safe_list_iterator& operator++() noexcept {
            this->_Node = this->_Node->_Next;
            return *this;
        }

        _Safe_list_iterator& operator++(int) noexcept {
            _Safe_list_iterator _Temp = *this;
            this->_Node               = this->_Node->_Next;
            return _Temp;
        }

        _Safe_list_iterator& operator--() noexcept {
            this->_Node = this->_Node->_Prev;
            return *this;
        }

        _Safe_list_iterator& operator--(int) noexcept {
            _Safe_list_iterator _Temp = *this;
            this->_Node               = this->_Node->_Prev;
            return _Temp;
        }
    };

    template <class _List, class _Traits>
    class _Safe_list_const_iterator
        : public _Safe_list_iterator_base<_List, _Traits> { // constant list iterator
    private:
        using _Mybase = _Safe_list_iterator_base<_List, _Traits>;
        using _Node_t = typename _Mybase::_Node_t;

    public:
        using value_type        = typename _Mybase::value_type;
        using difference_type   = typename _Mybase::difference_type;
        using pointer           = typename _List::const_pointer;
        using reference         = typename _List::const_reference;
        using iterator_category = typename _Mybase::iterator_category;

        _Safe_list_const_iterator() noexcept : _Mybase() {}

        explicit _Safe_list_const_iterator(_Node_t* const _Node) noexcept : _Mybase(_Node) {}

        ~_Safe_list_const_iterator() noexcept {}

        reference operator*() const noexcept {
            return this->_Node->_Value;
        }

        pointer operator->() const noexcept {
            return ::std::addressof(this->_Node->_Value);
        }

        _Safe_list_const_iterator& operator++() noexcept {
            this->_Node = this->_Node->_Next;
            return *this;
        }

        _Safe_list_const_iterator& operator++(int) noexcept {
            _Safe_list_const_iterator _Temp = *this;
            this->_Node                     = this->_Node->_Next;
            return _Temp;
        }

        _Safe_list_const_iterator& operator--() noexcept {
            this->_Node = this->_Node->_Prev;
            return *this;
        }

        _Safe_list_const_iterator& operator--(int) noexcept {
            _Safe_list_const_iterator _Temp = *this;
            this->_Node                     = this->_Node->_Prev;
            return _Temp;
        }
    };

    template <class _List, class _Traits>
    class _Safe_list_reverse_iterator
        : public _Safe_list_iterator_base<_List, _Traits> { // mutable list reverse iterator
    private:
        using _Mybase = _Safe_list_iterator_base<_List, _Traits>;
        using _Node_t = typename _Mybase::_Node_t;

    public:
        using value_type        = typename _Mybase::value_type;
        using difference_type   = typename _Mybase::difference_type;
        using pointer           = typename _List::pointer;
        using reference         = typename _List::reference;
        using iterator_category = typename _Mybase::iterator_category;

        _Safe_list_reverse_iterator() noexcept : _Mybase() {}

        explicit _Safe_list_reverse_iterator(_Node_t* const _Node) noexcept : _Mybase(_Node) {}

        ~_Safe_list_reverse_iterator() noexcept {}

        reference operator*() const noexcept {
            return this->_Node->_Value;
        }

        pointer operator->() const noexcept {
            return ::std::addressof(this->_Node->_Value);
        }

        _Safe_list_reverse_iterator& operator++() noexcept {
            this->_Node = this->_Node->_Prev;
            return *this;
        }

        _Safe_list_reverse_iterator& operator++(int) noexcept {
            _Safe_list_reverse_iterator _Temp = *this;
            this->_Node                       = this->_Node->_Prev;
            return _Temp;
        }

        _Safe_list_reverse_iterator& operator--() noexcept {
            this->_Node = this->_Node->_Next;
            return *this;
        }

        _Safe_list_reverse_iterator& operator--(int) noexcept {
            _Safe_list_reverse_iterator _Temp = *this;
            this->_Node                       = this->_Node->_Next;
            return _Temp;
        }
    };

    template <class _List, class _Traits>
    class _Safe_list_const_reverse_iterator
        : public _Safe_list_iterator_base<_List, _Traits> { // constant list reverse iterator
    private:
        using _Mybase = _Safe_list_iterator_base<_List, _Traits>;
        using _Node_t = typename _Mybase::_Node_t;

    public:
        using value_type        = typename _Mybase::value_type;
        using difference_type   = typename _Mybase::difference_type;
        using pointer           = typename _List::const_pointer;
        using reference         = typename _List::const_reference;
        using iterator_category = typename _Mybase::iterator_category;

        _Safe_list_const_reverse_iterator() noexcept : _Mybase() {}

        explicit _Safe_list_const_reverse_iterator(_Node_t* const _Node) noexcept : _Mybase(_Node) {}

        ~_Safe_list_const_reverse_iterator() noexcept {}

        reference operator*() const noexcept {
            return this->_Node->_Value;
        }

        pointer operator->() const noexcept {
            return ::std::addressof(this->_Node->_Value);
        }

        _Safe_list_const_reverse_iterator& operator++() noexcept {
            this->_Node = this->_Node->_Prev;
            return *this;
        }

        _Safe_list_const_reverse_iterator& operator++(int) noexcept {
            _Safe_list_const_reverse_iterator _Temp = *this;
            this->_Node                             = this->_Node->_Prev;
            return _Temp;
        }

        _Safe_list_const_reverse_iterator& operator--() noexcept {
            this->_Node = this->_Node->_Next;
            return *this;
        }

        _Safe_list_const_reverse_iterator& operator--(int) noexcept {
            _Safe_list_const_reverse_iterator _Temp = *this;
            this->_Node                             = this->_Node->_Next;
            return _Temp;
        }
    };

    template <class _Ty>
    class safe_list { // exception-safe doubly-linked list
    private:
        using _Traits = _Safe_list_traits<_Ty>;
        using _Node_t = _Safe_list_node<_Ty, _Traits>;
        using _Self_t = safe_list<_Ty>;
        
    public:
        using value_type      = _Ty;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;
        using pointer         = _Ty*;
        using const_pointer   = const _Ty*;
        using reference       = _Ty&;
        using const_reference = const _Ty&;
    
        using iterator               = _Safe_list_iterator<_Self_t, _Traits>;
        using const_iterator         = _Safe_list_const_iterator<_Self_t, _Traits>;
        using reverse_iterator       = _Safe_list_reverse_iterator<_Self_t, _Traits>;
        using const_reverse_iterator = _Safe_list_const_reverse_iterator<_Self_t, _Traits>;

        safe_list() noexcept : _Mystorage() {}

        safe_list(const safe_list& _Other) noexcept(_Traits::_Is_nothrow_copy_constructible) : _Mystorage() {
            _Copy_list(_Other);
        }

        safe_list(safe_list&& _Other) noexcept : _Mystorage() {
            _Mystorage._Swap(_Other._Mystorage); // swap storages
        }

        explicit safe_list(
            const size_type _Count) noexcept(_Traits::_Is_nothrow_move_constructible) : _Mystorage() {
            (void) resize(_Count);
        }

        explicit safe_list(const size_type _Count,
            const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) : _Mystorage() {
            (void) resize(_Count, _Value);
        }

        safe_list(::std::initializer_list<value_type> _Init_list) noexcept(
            _Traits::_Is_nothrow_copy_constructible) : _Mystorage() {
            (void) assign(_Init_list);
        }

        ~safe_list() noexcept {
            clear();
        }
        
        safe_list& operator=(const safe_list& _Other) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            if (this != ::std::addressof(_Other)) {
                _Copy_list(_Other);
            }

            return *this;
        }

        safe_list& operator=(safe_list&& _Other) noexcept {
            if (this != ::std::addressof(_Other)) {
                _Mystorage._Swap(_Other._Mystorage);
            }

            return *this;
        }

        safe_list& operator=(
            ::std::initializer_list<value_type> _Init_list) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            (void) assign(_Init_list);
            return *this;
        }

        bool empty() const noexcept {
            return _Mystorage._Size == 0;
        }

        size_type size() const noexcept {
            return _Mystorage._Size;
        }

        size_type max_size() const noexcept {
            return static_cast<size_type>(-1) / sizeof(_Ty);
        }

        iterator begin() noexcept {
            return iterator{_Mystorage._Head};
        }

        const_iterator begin() const noexcept {
            return const_iterator{_Mystorage._Head};
        }

        const_iterator cbegin() const noexcept {
            return const_iterator{_Mystorage._Head};
        }

        reverse_iterator rbegin() noexcept {
            return reverse_iterator{_Mystorage._Tail};
        }

        const_reverse_iterator rbegin() const noexcept {
            return const_reverse_iterator{_Mystorage._Tail};
        }

        const_reverse_iterator crbegin() const noexcept {
            return const_reverse_iterator{_Mystorage._Tail};
        }

        iterator end() noexcept {
            return iterator{nullptr}; // past-the-last element (null-pointer)
        }

        const_iterator end() const noexcept {
            return const_iterator{nullptr}; // past-the-last element (null-pointer)
        }

        const_iterator cend() const noexcept {
            return const_iterator{nullptr}; // past-the-last element (null-pointer)
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator{nullptr};
        }

        const_reverse_iterator rend() const noexcept {
            return const_reverse_iterator{nullptr};
        }

        const_reverse_iterator crend() const noexcept {
            return const_reverse_iterator{nullptr};
        }

        // Note: Since the container is exception-safe, front() and back() cannot throw an exception
        //       if the list is empty. Instead, we use pointers to access the first and the last node.
        //       This approach is not compatible with std::list, but it works perfectly in this case.

        pointer front() noexcept {
            return !empty() ? ::std::addressof(_Mystorage._Head->_Value) : nullptr;
        }

        const_pointer front() const noexcept {
            return !empty() ? ::std::addressof(_Mystorage._Head->_Value) : nullptr;
        }

        pointer back() noexcept {
            return !empty() ? ::std::addressof(_Mystorage._Tail->_Value) : nullptr;
        }

        const_pointer back() const noexcept {
            return !empty() ? ::std::addressof(_Mystorage._Tail->_Value) : nullptr;
        }

        [[nodiscard]] bool assign(
            size_type _Count, const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            clear(); // must be empty
            return resize(_Count, _Value);
        }

        template <class _InIt>
        [[nodiscard]] bool assign(_InIt _First, _InIt _Last) noexcept(
            _Traits::template _Is_nothrow_constructible<decltype(*::std::declval<_InIt>())>) {
            static_assert(_Traits::template _Is_constructible<decltype(*::std::declval<_InIt>())>,
                "T must be constructible from InIt's value type.");
            clear(); // must be empty
            for (; _First != _Last; ++_First) {
                if (!push_back(static_cast<_Ty>(*_First))) { // failed to create a new node
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]] bool assign(
            ::std::initializer_list<value_type> _Init_list) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            return assign(_Init_list.begin(), _Init_list.end());
        }

        void clear() noexcept {
            if (_Mystorage._Size > 0) { // non-empty list, erase elements
                _Node_t* _Next;
                for (_Node_t* _Node = _Mystorage._Head; _Node != nullptr; _Node = _Next) {
                    _Next = _Node->_Next;
                    _Destroy_object(_Node);
                }

                _Mystorage._Head = nullptr;
                _Mystorage._Tail = nullptr;
                _Mystorage._Size = 0;
            }
        }

        iterator insert(
            const_iterator _Where, const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return iterator{};
            }

            if (empty()) { // _Where is unused since the list is empty
                return push_back(_Value) ? begin() : iterator{};
            }

            _Node_t* const _New_node = _Make_node(_Value);
            if (!_New_node) { // allocation failed
                return iterator{};
            }

            if (_Where == cbegin()) { // insert before the first node
                _Mystorage._Head->_Prev        = _New_node;
                _Mystorage._Head->_Prev->_Next = _Mystorage._Head;
                _Mystorage._Head               = _Mystorage._Head->_Prev;
            } else if (_Where == cend()) { // insert before the last node
                _New_node->_Prev        = _Mystorage._Tail;
                _Mystorage._Tail->_Next = _New_node;
                _Mystorage._Tail        = _Mystorage._Tail->_Next;
            } else { // insert before the inner node
                _Node_t* const _Node     = _Where._Get_node();
                _Node_t* const _Old_prev = _Node->_Prev;
                _Node->_Prev             = _New_node;
                _New_node->_Prev         = _Old_prev;
                _New_node->_Next         = _Node;
                _Old_prev->_Next         = _New_node;
            }

            ++_Mystorage._Size;
            return iterator{_New_node};
        }

        iterator insert(const_iterator _Where, value_type&& _Value) noexcept(_Traits::_Is_nothrow_move_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return iterator{};
            }

            if (empty()) { // _Where is unused since the list is empty
                return push_back(::std::move(_Value)) ? begin() : iterator{};
            }

            _Node_t* const _New_node = _Make_node(::std::move(_Value));
            if (!_New_node) { // allocation failed
                return iterator{};
            }

            if (_Where == cbegin()) { // insert before the first node
                _Mystorage._Head->_Prev        = _New_node;
                _Mystorage._Head->_Prev->_Next = _Mystorage._Head;
                _Mystorage._Head               = _Mystorage._Head->_Prev;
            } else if (_Where == cend()) { // insert before the last node
                _New_node->_Prev        = _Mystorage._Tail;
                _Mystorage._Tail->_Next = _New_node;
                _Mystorage._Tail        = _Mystorage._Tail->_Next;
            } else { // insert before the inner node
                _Node_t* const _Node     = _Where._Get_node();
                _Node_t* const _Old_prev = _Node->_Prev;
                _Node->_Prev             = _New_node;
                _New_node->_Prev         = _Old_prev;
                _New_node->_Next         = _Node;
                _Old_prev->_Next         = _New_node;
            }

            ++_Mystorage._Size;
            return iterator{_New_node};
        }

        iterator insert(const_iterator _Where,
            size_type _Count, const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            iterator _Iter;
            while (_Count-- > 0) {
                _Iter  = insert(_Where, _Value);
                _Where = const_iterator{_Iter._Get_node()}; // update _Where's previous node
            }

            return _Iter;
        }

        template <class _InIt>
        iterator insert(const_iterator _Where, _InIt _First, _InIt _Last) noexcept(
            _Traits::template _Is_nothrow_constructible<decltype(*::std::declval<_InIt>())>) {
            static_assert(_Traits::template _Is_constructible<decltype(*::std::declval<_InIt>())>,
                "T must be constructible from InIt's value type.");
            iterator _Iter;
            for (; _First != _Last; ++_First) {
                _Iter  = insert(_Where, static_cast<_Ty>(*_First));
                _Where = const_iterator{_Iter._Get_node()->_Next};
            }

            return _Iter;
        }

        iterator insert(const_iterator _Where,
            ::std::initializer_list<value_type> _Init_list) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            return insert(_Where, _Init_list.begin(), _Init_list.end());
        }

        template <class... _Types>
        iterator emplace(const_iterator _Where,
            _Types&&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<_Types&&...>) {
            return insert(_Where, _Ty{::std::forward<_Types>(_Args)...});
        }

        iterator erase(const_iterator _Where) noexcept {
            if (_Mystorage._Size == 0) { // empty list, nothing to do
                return iterator{};
            }

            if (_Where == cbegin()) { // erase the first node
                _Node_t* const _Head = _Mystorage._Head;
                if (_Head->_Next) {
                    _Head->_Next->_Prev = nullptr;
                    _Mystorage._Head    = _Head->_Next;
                }

                _Destroy_object(_Head);
                return --_Mystorage._Size > 0 ? iterator{_Mystorage._Head} : iterator{};
            } else if (_Where == cend()) { // erase the last node
                _Node_t* const _Tail = _Mystorage._Tail;
                if (_Tail->_Prev) {
                    _Tail->_Prev->_Next = nullptr;
                    _Mystorage._Tail    = _Tail->_Prev;
                }

                _Destroy_object(_Tail);
                return --_Mystorage._Size > 0 ? iterator{_Mystorage._Tail} : iterator{};
            } else { // erase the inner node
                _Node_t* const _Node = _Where._Get_node();
                _Node->_Prev->_Next  = _Node->_Next;
                _Node->_Next->_Prev  = _Node->_Prev;
                const iterator _Iter(_Node->_Next); // next node replaces _Where's node
                _Destroy_object(_Node);
                --_Mystorage._Size;
                return _Iter;
            }
        }

        iterator erase(const_iterator _First, const_iterator _Last) noexcept {
            if (_Mystorage._Size == 0) { // empty list, nothing to do
                return iterator{};
            }

            iterator _Iter;
            while (_First != _Last) {
                _Iter  = erase(_First);
                _First = const_iterator{_Iter._Get_node()}; // update _First's node
            }

            return _Iter;
        }

        [[nodiscard]] bool push_back(
            const value_type& _Value) noexcept(_Traits::template _Is_nothrow_copy_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return false;
            }

            _Node_t* const _New_node = _Make_node(_Value);
            if (!_New_node) { // allocation failed
                return false;
            }

            if (_Mystorage._Size == 0) { // allocate the first node
                _Mystorage._Head = _New_node;
                _Mystorage._Tail = _Mystorage._Head;
            } else { // allocate the next node
                _Mystorage._Tail->_Next        = _New_node;
                _Mystorage._Tail->_Next->_Prev = _Mystorage._Tail;
                _Mystorage._Tail               = _Mystorage._Tail->_Next;
            }

            ++_Mystorage._Size;
            return true;
        }

        [[nodiscard]] bool push_back(value_type&& _Value) noexcept(_Traits::_Is_nothrow_move_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return false;
            }

            _Node_t* const _New_node = _Make_node(::std::move(_Value));
            if (!_New_node) { // allocation failed
                return false;
            }

            if (_Mystorage._Size == 0) { // allocate the first node
                _Mystorage._Head = _New_node;
                _Mystorage._Tail = _Mystorage._Head;
            } else { // allocate the next node
                _Mystorage._Tail->_Next        = _New_node;
                _Mystorage._Tail->_Next->_Prev = _Mystorage._Tail;
                _Mystorage._Tail               = _Mystorage._Tail->_Next;
            }

            ++_Mystorage._Size;
            return true;
        }

        template <class... _Types>
        [[nodiscard]] bool emplace_back(
            const _Types&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<const _Types&...>) {
            return insert(cend(), _Ty{_Args...}).valid();
        }

        template <class... _Types>
        [[nodiscard]] bool emplace_back(
            _Types&&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<_Types&&...>) {
            return insert(cend(), _Ty{::std::forward<_Types>(_Args)...}).valid();
        }

        [[nodiscard]] bool push_front(const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return false;
            }

            _Node_t* const _New_node = _Make_node(_Value);
            if (!_New_node) { // allocation failed
                return false;
            }

            if (_Mystorage._Size == 0) { // allocate the first node
                _Mystorage._Head = _New_node;
                _Mystorage._Tail = _Mystorage._Head;
            } else { // allocate the next node
                _Mystorage._Head->_Prev        = _New_node;
                _Mystorage._Head->_Prev->_Next = _Mystorage._Head;
                _Mystorage._Head               = _Mystorage._Head->_Prev;
            }

            ++_Mystorage._Size;
            return true;
        }

        [[nodiscard]] bool push_front(value_type&& _Value) noexcept(_Traits::_Is_nothrow_move_constructible) {
            if (_Mystorage._Size == max_size()) { // not enough space for another element
                return false;
            }

            _Node_t* const _New_node = _Make_node(::std::move(_Value));
            if (!_New_node) { // allocation failed
                return false;
            }

            if (_Mystorage._Size == 0) { // allocate the first node
                _Mystorage._Head = _New_node;
                _Mystorage._Tail = _Mystorage._Head;
            } else { // allocate the next node
                _Mystorage._Head->_Prev        = _New_node;
                _Mystorage._Head->_Prev->_Next = _Mystorage._Head;
                _Mystorage._Head               = _Mystorage._Head->_Prev;
            }

            ++_Mystorage._Size;
            return true;
        }

        template <class... _Types>
        [[nodiscard]] bool emplace_front(
            const _Types&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<const _Types&...>) {
            return insert(cbegin(), _Ty{_Args...}).valid();
        }

        template <class... _Types>
        [[nodiscard]] bool emplace_front(
            _Types&&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<_Types&&...>) {
            return insert(cbegin(), _Ty{::std::forward<_Types>(_Args)...}).valid();
        }

        void pop_back() noexcept {
            switch (_Mystorage._Size) {
            case 0: // empty list, nothing to do
                break;
            case 1: // erase the only node (destroys the whole list)
                _Destroy_object(_Mystorage._Head);
                _Mystorage._Head = nullptr;
                _Mystorage._Tail = nullptr;
                _Mystorage._Size = 0;
                break;
            default: // erase an element
                _Node_t* const _Tail = _Mystorage._Tail;
                _Tail->_Prev->_Next  = nullptr;
                _Mystorage._Tail     = _Tail->_Prev;
                --_Mystorage._Size;
                _Destroy_object(_Tail);
                break;
            }
        }

        void pop_front() noexcept {
            switch (_Mystorage._Size) {
            case 0: // empty list, nothing to do
                break;
            case 1: // erase the only node (destroys the whole list)
                _Destroy_object(_Mystorage._Head);
                _Mystorage._Head = nullptr;
                _Mystorage._Tail = nullptr;
                _Mystorage._Size = 0;
                break;
            default: // erase an element
                _Node_t* const _Head = _Mystorage._Head;
                _Head->_Next->_Prev  = nullptr;
                _Mystorage._Head     = _Head->_Next;
                --_Mystorage._Size;
                _Destroy_object(_Head);
                break;
            }
        }

        [[nodiscard]] bool resize(const size_type _New_size) noexcept(_Traits::_Is_nothrow_move_constructible) {
            if (_Mystorage._Size < _New_size) { // create new nodes
                while (_Mystorage._Size != _New_size) {
                    if (!push_back(_Ty{})) { // failed to create a new node
                        return false;
                    }
                }
            } else { // delete existing nodes
                while (_Mystorage._Size != _New_size) {
                    pop_back();
                }
            }

            return true;
        }

        [[nodiscard]] bool resize(const size_type _New_size,
            const value_type& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            if (_Mystorage._Size < _New_size) { // create new nodes
                while (_Mystorage._Size != _New_size) {
                    if (!push_back(_Value)) { // failed to create a new node
                        return false;
                    }
                }
            } else { // delete existing nodes
                while (_Mystorage._Size != _New_size) {
                    pop_back();
                }
            }

            return true;
        }

        void swap(safe_list& _Other) noexcept {
            _Mystorage._Swap(_Other._Mystorage);
        }

        template <class _Pr>
        size_type remove_if(_Pr _Pred) noexcept(::std::is_nothrow_invocable_v<_Pr, const _Ty&>) {
            if (_Mystorage._Size == 0) { // must not be empty
                return 0;
            }
            
            size_type _Count = 0;
            _Node_t* _Next;
            for (_Node_t* _Node = _Mystorage._Head; _Node != nullptr; _Node = _Next) {
                _Next = _Node->_Next;
                if (_Pred(_Node->_Value)) { // element found, erase it
                    _Delete_node(_Node);
                    ++_Count;
                }
            }

            return _Count;
        }

        size_type remove(const value_type& _Value) noexcept {
            return remove_if(
                [_Value](const value_type& _Node_value) noexcept {
                    return _Node_value == _Value;
                }
            );
        }

        void reverse() noexcept(_Traits::_Is_nothrow_swappable) {
            if (_Mystorage._Size > 1) { // must contains at least 2 nodes
                _Node_t* _Head = _Mystorage._Head;
                _Node_t* _Tail = _Mystorage._Tail;
                while (_Head != _Tail) {
                    ::std::swap(_Head->_Value, _Tail->_Value);
                    _Head = _Head->_Next;
                    _Tail = _Tail->_Prev;
                    if (_Head->_Next == _Tail) { // reverse two middle nodes
                        ::std::swap(_Head->_Value, _Tail->_Value);
                        break;
                    }
                }
            }
        }

    private:
        template <class... _Types>
        static _Node_t* _Make_node(
            const _Types&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<const _Types&...>) {
            static_assert(_Traits::template
                _Is_constructible<const _Types&...>, "T must be constructible from Types...");
            return _Construct_object<_Node_t>(_Args...);
        }

        template <class... _Types>
        static _Node_t* _Make_node(
            _Types&&... _Args) noexcept(_Traits::template _Is_nothrow_constructible<_Types&&...>) {
            static_assert(_Traits::template
                _Is_constructible<_Types&&...>, "T must be constructible from Types...");
            return _Construct_object<_Node_t>(::std::forward<_Types>(_Args)...);
        }

        void _Delete_node(_Node_t* const _Node) noexcept {
            if (_Node == _Mystorage._Head) { // delete the first node
                if (_Node->_Next) {
                    _Node->_Next->_Prev = nullptr;
                    _Mystorage._Head    = _Node->_Next;
                }
            } else if (_Node == _Mystorage._Tail) { // delete the last node
                _Node->_Prev->_Next = nullptr;
                _Mystorage._Tail    = _Node->_Prev;
            } else { // delete the inner node
                _Node->_Prev->_Next = _Node->_Next;
                _Node->_Next->_Prev = _Node->_Prev;
            }

            _Destroy_object(_Node);
            --_Mystorage._Size;
        }

        [[nodiscard]] bool _Append_node(const _Ty& _Value) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            // Note: This function is near similar to push_back(), but it does not check
            //       if we can fit another node, unlike push_back() does. This function is
            //       only used in _Copy_list(), where we already know the maximum size of
            //       the copied list, so we can skip the size check performed by push_back().
            //       By doing so, we avoid unnecessary size comparisons and improve performance.
            _Node_t* const _New_node = _Make_node(_Value);
            if (!_New_node) { // allocation failed
                return false;
            }

            if (_Mystorage._Size == 0) { // allocate the first node
                _Mystorage._Head = _New_node;
                _Mystorage._Tail = _Mystorage._Head;
            } else { // allocate the next node
                _Mystorage._Tail->_Next        = _New_node;
                _Mystorage._Tail->_Next->_Prev = _Mystorage._Tail;
                _Mystorage._Tail               = _Mystorage._Tail->_Next;
            }

            ++_Mystorage._Size;
            return true;
        }

        void _Copy_list(const safe_list& _Other) noexcept(_Traits::_Is_nothrow_copy_constructible) {
            for (const value_type& _Value : _Other) {
                if (!_Append_node(_Value)) { // failed to create a new node
                    return;
                }
            }
        }

        _Safe_list_storage<_Ty, _Traits, size_type> _Mystorage;
    };

    template <class _Ty>
    void swap(safe_list<_Ty>& _Left, safe_list<_Ty>& _Right) noexcept {
        _Left.swap(_Right);
    }

    template <class _Ty>
    typename safe_list<_Ty>::size_type erase(safe_list<_Ty>& _List, const _Ty& _Value) noexcept {
        return _List.remove(_Value);
    }

    template <class _Ty, class _Pr>
    typename safe_list<_Ty>::size_type erase(
        safe_list<_Ty>& _List, _Pr _Pred) noexcept(::std::is_nothrow_invocable_v<_Pr, const _Ty&>) {
        return _List.remove_if(_Pred);
    }
} // namespace mjx

#endif // _SAFE_LIST_HPP_