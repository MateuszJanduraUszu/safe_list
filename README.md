safe_list
---

A C++17 non-throwing doubly-linked list.

Features
---

* Exception-safe memory allocation/deallocation
* No exceptions
* No third-party dependencies
* `std::list<T>` coherent (see **Important** for more details)

Important
---

Note that some methods differ from `std::list<T>`, for eg. `front()` and `back()`.
The standard version returns a reference to the first and last element what enforces
exceptions use. Since the `safe_list<T>` implementation does not use exceptions,
the `front()` and `back()` methods have been modified to return a pointers to the first
and last elements, respectvely. This is done to avoid enforcing exception usage in the client code.
Methods like `push_back()` and `push_front()` returns `bool` to let user know whether they succeeded or not.

Optional future features
---

* `assign_range()`
* `insert_range()`
* `append_range()`
* `prepend_range()`
* `merge()`
* `slice()`
* `unique()`
* `sort()`