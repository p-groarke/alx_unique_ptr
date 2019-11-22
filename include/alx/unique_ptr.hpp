#pragma once

#include "compressed_pair.hpp"
#include <cstddef>
#include <memory>

namespace alx {
template <class T, class Deleter = std::default_delete<T>>
class unique_ptr;

template <class T>
class weak_ptr;

template <class T>
class enable_weak_from_this;

namespace detail {
template <typename __Tp>
class unique_weak_data {
public:
	using __self = unique_weak_data;
	using element_type = __Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;

	inline void incr() {
		_weak_count++;
	}

	inline void decr() {
		if (_weak_count) {
			_weak_count--;
		}
	}

	inline std::size_t count() const {
		return _weak_count;
	}

	inline void set_pointer(pointer p) {
		_ptr = p;
	}

	inline pointer get() {
		return _ptr;
	}

	inline const_pointer get() const {
		return _ptr;
	}

	inline bool is_valid() const {
		return _ptr != nullptr;
	}

private:
	std::size_t _weak_count = 0;
	pointer _ptr = nullptr;
};
} // namespace detail.

// template<typename T>
// class unique_ptr {
// public:
//  using pointer = T*;
//  using const_pointer = const T*;
//
//  unique_ptr(pointer p = nullptr) noexcept;
//  unique_ptr(unique_ptr&& u) noexcept;
//  ~unique_ptr() noexcept;
//
//  unique_ptr& operator=(std::nullptr_t) noexcept;
//* unique_ptr& operator=(unique_ptr&& u) noexcept;

//  void reset(pointer p = nullptr) noexcept;
//  void swap(unique_ptr& other) noexcept;
//  bool is_valid() const noexcept;
//  pointer get() noexcept;
//  const_pointer get() const noexcept;
//  pointer operator->() noexcept;
//  const_pointer operator->() const noexcept;
//
//* pointer release() noexcept;
//* explicit operator bool() const noexcept;

//  typename std::add_lvalue_reference<T>::type operator*() const;
//  pointer operator->() const noexcept;
//};
// std::unique_ptr<T>

template <class __Tp, class __Dp>
class unique_ptr {
public:
	using __self = unique_ptr;
	using element_type = __Tp;
	using deleter_type = __Dp;
	using pointer = element_type*;
	using const_pointer = const element_type*;

private:
	using weak_data_type = detail::unique_weak_data<element_type>;
	using weak_from_this_type = enable_weak_from_this<element_type>;

	static constexpr bool has_weak_from_this
			= std::is_base_of<alx::enable_weak_from_this<element_type>,
					element_type>::value;

	using __enabled_if_deleter_is_default_constructible =
			typename std::enable_if<
					std::is_default_constructible<deleter_type>::value
							&& !std::is_pointer<deleter_type>::value,
					int>::type;

	template <bool __HasWeakFromThis = true>
	inline void weak_from_this_assign();

	template <>
	inline void weak_from_this_assign<false>() {
	}

public:
	template <bool __Dummy = true,
			__enabled_if_deleter_is_default_constructible = 0>
	inline constexpr unique_ptr() noexcept
			: _ptr(nullptr) {
	}

	template <bool __Dummy = true,
			__enabled_if_deleter_is_default_constructible = 0>
	inline unique_ptr(pointer p) noexcept
			: _ptr(p) {

		weak_from_this_assign<has_weak_from_this>();
	}

	unique_ptr(unique_ptr&& u) noexcept
			: _ptr(u._ptr)
			, _weak_data(u._weak_data) {

		if (_weak_data) {
			_weak_data->set_pointer(_ptr.first());
		}

		u._ptr = alx::compressed_pair<pointer, deleter_type>(nullptr);
		u._weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		u.weak_from_this_assign<has_weak_from_this>();
	}

	unique_ptr(const unique_ptr&) = delete;

	inline ~unique_ptr() noexcept {
		reset(nullptr);
	}

	inline void reset(pointer p = nullptr) noexcept {
		if (_ptr.first()) {
			delete _ptr.first();
		}

		if (_weak_data) {
			_weak_data->set_pointer(nullptr);

			if (_weak_data->count() == 0) {
				delete _weak_data;
			}
		}

		_ptr = alx::compressed_pair<pointer, deleter_type>(p);
		_weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
	}

	unique_ptr& operator=(std::nullptr_t) noexcept {
		reset(nullptr);
		return *this;
	}

	unique_ptr& operator=(unique_ptr&& u) noexcept {
		reset(nullptr);

		_ptr = u._ptr;
		_weak_data = u._weak_data;

		if (_weak_data) {
			_weak_data->set_pointer(_ptr.first());
		}

		u._ptr = alx::compressed_pair<pointer, deleter_type>(nullptr);
		u._weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		u.weak_from_this_assign<has_weak_from_this>();
		return *this;
	}

	void swap(unique_ptr& other) noexcept {
		alx::compressed_pair<pointer, deleter_type> p = other._ptr;
		weak_data_type* weak_data = other._weak_data;

		other._ptr = _ptr;
		other._weak_data = _weak_data;
		if (other._weak_data) {
			other._weak_data->set_pointer(other._ptr.first());
		}

		_ptr = p;
		_weak_data = weak_data;
		if (_weak_data) {
			_weak_data->set_pointer(_ptr.first());
		}

		other.weak_from_this_assign<has_weak_from_this>();
		weak_from_this_assign<has_weak_from_this>();
	}

	inline bool is_valid() const noexcept {
		return _ptr.first() != nullptr;
	}

	inline pointer get() const noexcept {
		return _ptr.first();
	}

	inline pointer operator->() const noexcept {
		return get();
	}

	inline typename std::add_lvalue_reference<element_type>::type
	operator*() const {
		return *(_ptr.first());
	}

private:
	friend weak_ptr<element_type>;
	friend enable_weak_from_this<element_type>;

	alx::compressed_pair<pointer, deleter_type> _ptr;
	mutable weak_data_type* _weak_data = nullptr; // Mutable ???

	inline weak_data_type* get_weak_data() {
		return _weak_data;
	}

	inline bool has_weak_data() const {
		return _weak_data != nullptr;
	}

public:
	inline bool test__has_weak_data() const {
		return has_weak_data();
	}

	inline weak_data_type* test__get_weak_data() {
		return get_weak_data();
	}
};

// template<typename T>
// class weak_ptr {
// public:
//  using pointer = T*;
//  using const_pointer = const T*;
//  weak_ptr();
//  weak_ptr(const weak_ptr& wp) noexcept;
//  weak_ptr(weak_ptr&& wp) noexcept;
//  weak_ptr(unique_ptr<T>& u) noexcept;
//
//  ~weak_ptr() noexcept;
//
//  weak_ptr& operator=(unique_ptr<T>& u) noexcept;
//  weak_ptr& operator=(const weak_ptr& wp) noexcept;
//
//  void reset() noexcept;
//  bool is_valid() const noexcept;
//  pointer operator->() noexcept;
//  const_pointer operator->() const noexcept;
//};

template <typename __Tp>
class weak_ptr {
public:
	using __self = weak_ptr;
	using element_type = __Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;

private:
	using weak_data_type = detail::unique_weak_data<element_type>;

public:
	weak_ptr() {
	}

	weak_ptr(const weak_ptr& wp) noexcept
			: _weak_data(wp._weak_data) {
		if (_weak_data) {
			_weak_data->incr();
		}
	}

	weak_ptr(weak_ptr&& wp) noexcept
			: _weak_data(wp._weak_data) {
		wp._weak_data = nullptr;
	}

	weak_ptr(unique_ptr<element_type>& u) noexcept {
		if (!u.is_valid()) {
			return;
		}

		// Copy weak data if it exist.
		if (u.has_weak_data()) {
			_weak_data = u.get_weak_data();
			_weak_data->incr();
			return;
		}

		// Create weak data and set it inside the unique_ptr.
		weak_data_type* weak_data = new weak_data_type;
		weak_data->incr();
		weak_data->set_pointer(u.get());
		u._weak_data = weak_data;
		_weak_data = weak_data;
	}

	~weak_ptr() noexcept {
		if (_weak_data == nullptr) {
			return;
		}

		_weak_data->decr();

		// If instance still exist then it will take care of deleting it.
		if (_weak_data->is_valid()) {
			return;
		}

		if (_weak_data->count() == 0) {
			delete _weak_data;
			_weak_data = nullptr;
		}
	}

	inline void reset() noexcept {
		if (!_weak_data) {
			return;
		}

		_weak_data->decr();

		if (!_weak_data->is_valid() && _weak_data->count() == 0) {
			delete _weak_data;
		}

		_weak_data = nullptr;
	}

	inline weak_ptr& operator=(unique_ptr<element_type>& u) noexcept {
		// Release current instance.
		reset();

		if (!u.is_valid()) {
			return *this;
		}

		// Copy weak data if it exist.
		if (u.has_weak_data()) {
			_weak_data = u.get_weak_data();
			_weak_data->incr();
			return *this;
		}

		// Create weak data and set it inside the wunique_ptr.
		weak_data_type* weak_data = new weak_data_type;
		weak_data->incr();
		weak_data->set_pointer(u.get());
		u._weak_data = weak_data;
		_weak_data = weak_data;

		return *this;
	}

	inline weak_ptr& operator=(const weak_ptr& wp) noexcept {
		// Release current instance.
		reset();

		_weak_data = wp._weak_data;
		if (_weak_data) {
			_weak_data->incr();
		}
		return *this;
	}

	inline bool is_valid() const noexcept {
		if (!_weak_data) {
			return false;
		}

		return _weak_data->is_valid();
	}

	inline pointer operator->() noexcept {
		return _weak_data->get();
	}

	inline const_pointer operator->() const noexcept {
		return _weak_data->get();
	}

private:
	weak_data_type* _weak_data = nullptr;

public:
	inline bool test__has_weak_data() const {
		return _weak_data != nullptr;
	}

	inline weak_data_type* test__get_weak_data() {
		return _weak_data;
	}
};

template <class _Tp>
class enable_weak_from_this {
public:
	using __self = enable_weak_from_this;
	using element_type = _Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;

protected:
	inline enable_weak_from_this() noexcept {
	}
	inline enable_weak_from_this(enable_weak_from_this const&) noexcept {
	}
	inline enable_weak_from_this& operator=(
			enable_weak_from_this const&) noexcept {
		return *this;
	}
	inline ~enable_weak_from_this() {
	}

public:
	inline weak_ptr<element_type> weak_from_this() noexcept {
		return _weak_this;
	}
	//    inline weak_ptr<const _Tp> weak_from_this() const noexcept { return
	//    _weak_this; }

	template <class T, class Deleter>
	friend class unique_ptr;

private:
	mutable weak_ptr<element_type> _weak_this;
};

template <typename _Tp, typename _Deleter>
template <bool __HasWeakFromThis> // Always true.
inline void unique_ptr<_Tp, _Deleter>::weak_from_this_assign() {
	if (!is_valid()) {
		return;
	}

	get()->_weak_this = *this;
}
} // namespace alx.
