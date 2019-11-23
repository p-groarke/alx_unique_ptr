#pragma once

#include <cstddef>
#include <iostream>
#include <memory>
#include <atomic>

namespace alx {
template <class _Tp, class _Deleter = std::default_delete<_Tp>>
class unique_ptr;

template <class _Tp>
class weak_ptr;

template <class _Tp>
class enable_weak_from_this;

// template <class _Tp>
class stealable_unique_ptr {
	//  stealable_unique_ptr() = default;
};

namespace detail {
template <class _Tp>
struct __identity {
	typedef _Tp type;
};

template <class _Tp, bool>
struct __dependent_type : public _Tp {};

template <class _Deleter>
struct __unique_ptr_deleter_sfinae {
	static_assert(
			!std::is_reference<_Deleter>::value, "incorrect specialization");
	typedef const _Deleter& __lval_ref_type;
	typedef _Deleter&& __good_rval_ref_type;
	typedef std::true_type __enable_rval_overload;
};

template <class _Deleter>
struct __unique_ptr_deleter_sfinae<_Deleter const&> {
	typedef const _Deleter& __lval_ref_type;
	typedef const _Deleter&& __bad_rval_ref_type;
	typedef std::false_type __enable_rval_overload;
};

template <class _Deleter>
struct __unique_ptr_deleter_sfinae<_Deleter&> {
	typedef _Deleter& __lval_ref_type;
	typedef _Deleter&& __bad_rval_ref_type;
	typedef std::false_type __enable_rval_overload;
};

struct stolen_data {
	static void* get() {
		static void* p = nullptr;
		return &p;
	}

	template <typename T>
	static T* get() {
		return reinterpret_cast<T*>(get());
	}
};

template <typename _Tp>
class unique_weak_data {
public:
	//  static_assert(std::atomic<std::size_t>::is_);

	using __self = unique_weak_data;
	using element_type = _Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;

	unique_weak_data()
			: _ptr(nullptr)
			, _weak_count(0) {
	}
	unique_weak_data(pointer p)
			: _ptr(p)
			, _weak_count(0) {
	}

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

	inline void set_stolen_pointer() {
		_ptr = stolen_data::get<element_type>();
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

	inline bool is_stolen() const {
		return reinterpret_cast<void*>(_ptr) == stolen_data::get();
	}

private:
	//	std::size_t _weak_count = 0;
	pointer _ptr = nullptr;
	std::atomic<std::size_t> _weak_count; // = 0;
};

// template<typename T>
// const unique_weak_data<T> unique_weak_data<T>::stolen_data;

template <typename _Tp, typename _Deleter, bool _IsDeleterEmpty = false>
class pointer_deleter_pair_base {
public:
	using __self = pointer_deleter_pair_base;
	using element_type = _Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;
	using deleter_type = _Deleter;
	using deleter_reference = deleter_type&;
	using deleter_const_reference = const deleter_type&;

	inline pointer_deleter_pair_base()
			: _ptr(pointer())
			, _deleter(deleter_type()) {
	}

	inline pointer_deleter_pair_base(pointer p)
			: _ptr(p)
			, _deleter(deleter_type()) {
	}

	template <typename U>
	inline pointer_deleter_pair_base(pointer p, U&& d)
			: _ptr(p)
			, _deleter(std::forward<U>(d)) {
	}

	//  inline pointer_deleter_pair_base(pointer p, const deleter_type& d)
	//  : _ptr(p)
	//  , _deleter(d) {
	//  }
	//
	//  inline pointer_deleter_pair_base(pointer p, deleter_type&& d)
	//  : _ptr(p)
	//  , _deleter(std::move(d)) {
	//  }

	inline pointer_deleter_pair_base(const pointer_deleter_pair_base& pdb)
			: _ptr(pdb._ptr)
			, _deleter(pdb._deleter) {
	}

	inline pointer_deleter_pair_base(pointer_deleter_pair_base&& pdb)
			: _ptr(pdb._ptr)
			, _deleter(std::move(pdb._deleter)) {
		pdb._ptr = nullptr;
	}

	inline pointer_deleter_pair_base& operator=(
			const pointer_deleter_pair_base& pdb) {
		_ptr = pdb._ptr;
		_deleter = pdb._deleter;
		return *this;
	}

	inline pointer_deleter_pair_base& operator=(
			pointer_deleter_pair_base&& pdb) {
		_ptr = pdb._ptr;
		_deleter = std::move(pdb._deleter);
		pdb._ptr = nullptr;
		return *this;
	}

	inline void set_pointer(pointer p) {
		_ptr = p;
	}

	inline pointer get_pointer() const {
		return _ptr;
	}

	inline void set_deleter(deleter_const_reference d) {
		_deleter = d;
	}

	inline deleter_reference get_deleter() {
		return _deleter;
	}

	inline deleter_const_reference get_deleter() const {
		return _deleter;
	}

	inline void delete_pointer() {
		_deleter(_ptr);
		_ptr = nullptr;
	}

private:
	pointer _ptr;
	deleter_type _deleter;
};

template <typename _Tp, typename _Deleter>
class pointer_deleter_pair_base<_Tp, _Deleter, true> {
public:
	using __self = pointer_deleter_pair_base;
	using element_type = _Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;
	using pointer_reference = pointer&;
	using pointer_const_reference = const pointer&;
	using deleter_type = _Deleter;
	using deleter_reference = deleter_type&;
	using deleter_const_reference = const deleter_type&;

	inline pointer_deleter_pair_base()
			: _ptr(pointer()) {
	}

	inline pointer_deleter_pair_base(pointer p)
			: _ptr(p) {
	}

	template <typename U>
	inline pointer_deleter_pair_base(pointer p, U&&)
			: _ptr(p) {
	}

	inline pointer_deleter_pair_base(pointer p, deleter_type&&)
			: _ptr(p) {
	}

	inline pointer_deleter_pair_base(const pointer_deleter_pair_base& pdb)
			: _ptr(pdb._ptr) {
	}

	inline pointer_deleter_pair_base(pointer_deleter_pair_base&& pdb)
			: _ptr(pdb._ptr) {
		pdb._ptr = nullptr;
	}

	inline pointer_deleter_pair_base& operator=(
			const pointer_deleter_pair_base& pdb) {
		_ptr = pdb._ptr;
		return *this;
	}

	inline pointer_deleter_pair_base& operator=(
			pointer_deleter_pair_base&& pdb) {
		_ptr = pdb._ptr;
		pdb._ptr = nullptr;
		return *this;
	}

	inline void set_pointer(pointer p) {
		_ptr = p;
	}

	inline pointer get_pointer() const {
		return _ptr;
	}

	inline void set_deleter(deleter_const_reference) {
	}

	inline deleter_reference get_deleter() {
		return *reinterpret_cast<deleter_type*>(this);
	}

	inline deleter_const_reference get_deleter() const {
		return *reinterpret_cast<const deleter_type*>(this);
	}

	inline void delete_pointer() {
		deleter_type deleter;
		deleter(_ptr);
		_ptr = nullptr;
	}

private:
	pointer _ptr;
};

template <typename _Tp, typename _Deleter>
class pointer_and_deleter : public pointer_deleter_pair_base<_Tp, _Deleter,
									std::is_empty<_Deleter>::value> {
public:
	using __self = pointer_and_deleter;
	using element_type = _Tp;
	using pointer = element_type*;
	using const_pointer = const element_type*;
	using deleter_type = _Deleter;
	using base = pointer_deleter_pair_base<_Tp, _Deleter,
			std::is_empty<_Deleter>::value>;

	pointer_and_deleter() {
	}
	pointer_and_deleter(pointer p)
			: base(p) {
	}

	template <typename U>
	pointer_and_deleter(pointer p, U&& d)
			: base(p, std::forward<U>(d)) {
	}

	pointer_and_deleter(const pointer_and_deleter&) = default;
	pointer_and_deleter(pointer_and_deleter&&) = default;
	pointer_and_deleter& operator=(const pointer_and_deleter&) = default;
	pointer_and_deleter& operator=(pointer_and_deleter&&) = default;

	using base::set_pointer;
	using base::get_pointer;
	using base::set_deleter;
	using base::get_deleter;
	using base::delete_pointer;
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
//  unique_ptr& operator=(unique_ptr&& u) noexcept;

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

struct alloc_weak_tag {};
struct keep_weak_tag {};
struct keep_or_alloc_weak_tag {};

template <class _Tp, class _Deleter>
class unique_ptr {
public:
	using __self = unique_ptr;
	using element_type = _Tp;
	using deleter_type = _Deleter;
	using pointer = element_type*;
	using const_pointer = const element_type*;

private:
	using weak_data_type = detail::unique_weak_data<element_type>;
	using weak_from_this_type = enable_weak_from_this<element_type>;
	using pointer_deleter_type
			= detail::pointer_and_deleter<element_type, deleter_type>;

	static constexpr bool has_weak_from_this
			= std::is_base_of<alx::enable_weak_from_this<element_type>,
					element_type>::value;

	static constexpr bool is_stealable
			= std::is_base_of<alx::stealable_unique_ptr, element_type>::value;

	//  template <bool _Dummy, class _D = typename detail::__dependent_type<
	//                             detail::__identity<element_type>,
	//                             _Dummy>::type>
	//  using _EnableIfStealable =
	//      typename
	//      std::enable_if<std::is_base_of<alx::unstealable_unique_ptr<_D>,
	//      _D>::value>::type;


	template <class _ArgType>
	using _EnableIfDeleterConstructible = typename std::enable_if<
			std::is_constructible<deleter_type, _ArgType>::value>::type;

	template <bool _Dummy,
			class _D = typename detail::__dependent_type<
					detail::__identity<deleter_type>, _Dummy>::type>
	using _EnableIfDeleterDefaultConstructible =
			typename std::enable_if<std::is_default_constructible<_D>::value
					&& !std::is_pointer<_D>::value>::type;

	template <class _UPtr, class _Up>
	using _EnableIfMoveConvertible = typename std::enable_if<
			std::is_convertible<typename _UPtr::pointer, pointer>::value
			&& !std::is_array<_Up>::value>::type;

	template <class _UDel>
	using _EnableIfDeleterConvertible = typename std::enable_if<
			(std::is_reference<deleter_type>::value
					&& std::is_same<deleter_type, _UDel>::value)
			|| (!std::is_reference<deleter_type>::value
					   && std::is_convertible<_UDel, deleter_type>::value)>::
			type;

	typedef detail::__unique_ptr_deleter_sfinae<deleter_type> _DeleterSFINAE;

	template <bool _Dummy>
	using deleter_left_value_ref =
			typename detail::__dependent_type<_DeleterSFINAE,
					_Dummy>::__lval_ref_type;

	template <bool _Dummy>
	using deleter_right_value_ref =
			typename detail::__dependent_type<_DeleterSFINAE,
					_Dummy>::__good_rval_ref_type;

	template <bool _Dummy>
	using deleter_bad_right_value_ref =
			typename detail::__dependent_type<_DeleterSFINAE,
					_Dummy>::__bad_rval_ref_type;

	template <bool _Dummy>
	using _EnableIfDeleterLeftValueRefConstructible
			= _EnableIfDeleterConstructible<deleter_left_value_ref<_Dummy>>;

	template <bool _Dummy>
	using _EnableIfDeleterRightValueRefConstructible
			= _EnableIfDeleterConstructible<deleter_right_value_ref<_Dummy>>;

	template <bool __HasWeakFromThis = true>
	inline void weak_from_this_assign();

	template <>
	inline void weak_from_this_assign<false>() {
	}

	template <bool __HasWeakFromThis = true>
	inline void weak_from_this_assign(alloc_weak_tag);

	template <>
	inline void weak_from_this_assign<false>(alloc_weak_tag) {
		if (is_valid()) {
			_weak_data = new weak_data_type(get());
		}
	}

public:
	//	template <bool __Dummy = true,
	//			__enabled_if_deleter_is_default_constructible = 0>
	template <bool _Dummy = true,
			class = _EnableIfDeleterDefaultConstructible<_Dummy>>
	inline constexpr unique_ptr() noexcept
			: _pointer_and_deleter(nullptr) {
	}

	template <bool _Dummy = true,
			class = _EnableIfDeleterDefaultConstructible<_Dummy>>
	inline unique_ptr(pointer p) noexcept
			: _pointer_and_deleter(p) {

		weak_from_this_assign<has_weak_from_this>();
	}

	// template<typename U>
	// template <typename U, bool _Dummy = true,
	// class = _EnableIfDeleterConstructible<_LValRefType<_Dummy>>>
	// inline unique_ptr(pointer p, U&& d) noexcept
	//     : _pointer_and_deleter(p, std::forward<U>(d)) {
	//
	//   weak_from_this_assign<has_weak_from_this>();
	// }

	template <bool _Dummy = true,
			class = _EnableIfDeleterLeftValueRefConstructible<_Dummy>>
	inline unique_ptr(pointer p, deleter_left_value_ref<_Dummy> d) noexcept
			: _pointer_and_deleter(p, d) {
		weak_from_this_assign<has_weak_from_this>();
	}

	template <bool _Dummy = true,
			class = _EnableIfDeleterRightValueRefConstructible<_Dummy>>
	inline unique_ptr(pointer p, deleter_right_value_ref<_Dummy> d) noexcept
			: _pointer_and_deleter(p, std::move(d)) {
		static_assert(!std::is_reference<deleter_type>::value,
				"rvalue deleter bound to reference");

		weak_from_this_assign<has_weak_from_this>();
	}
	//  template <bool _Dummy = true,
	//            class =
	//            _EnableIfDeleterConstructible<deleter_right_value_ref<_Dummy>>>
	//  inline
	//  unique_ptr(pointer p, deleter_right_value_ref<_Dummy> d) noexcept
	//      : _pointer_and_deleter(p, std::move(d)) {
	//    static_assert(!std::is_reference<deleter_type>::value, "rvalue deleter
	//    bound to reference");
	//
	//    weak_from_this_assign<has_weak_from_this>();
	//  }

	template <bool _Dummy = true,
			class = _EnableIfDeleterConstructible<
					deleter_bad_right_value_ref<_Dummy>>>
	inline unique_ptr(pointer __p, deleter_bad_right_value_ref<_Dummy> __d)
			= delete;


	template <bool _Dummy = true,
			class = _EnableIfDeleterDefaultConstructible<_Dummy>>
	inline unique_ptr(pointer p, alloc_weak_tag awt) noexcept
			: _pointer_and_deleter(p) {

		weak_from_this_assign<has_weak_from_this>(awt);
	}

	template <bool _Dummy = true,
			class = _EnableIfDeleterLeftValueRefConstructible<_Dummy>>
	inline unique_ptr(pointer p, deleter_left_value_ref<_Dummy> d,
			alloc_weak_tag awt) noexcept
			: _pointer_and_deleter(p, d) {
		weak_from_this_assign<has_weak_from_this>(awt);
	}

	template <bool _Dummy = true,
			class = _EnableIfDeleterRightValueRefConstructible<_Dummy>>
	inline unique_ptr(pointer p, deleter_right_value_ref<_Dummy> d,
			alloc_weak_tag awt) noexcept
			: _pointer_and_deleter(p, std::move(d)) {
		static_assert(!std::is_reference<deleter_type>::value,
				"rvalue deleter bound to reference");

		weak_from_this_assign<has_weak_from_this>(awt);
	}

	template <bool _Dummy = true,
			class = _EnableIfDeleterConstructible<
					deleter_bad_right_value_ref<_Dummy>>>
	inline unique_ptr(pointer __p, deleter_bad_right_value_ref<_Dummy> __d,
			alloc_weak_tag awt)
			= delete;

	inline unique_ptr(unique_ptr&& u) noexcept
			: _pointer_and_deleter(std::move(u._pointer_and_deleter))
			, _weak_data(u._weak_data) {

		if (_weak_data) {
			_weak_data->set_pointer(_pointer_and_deleter.get_pointer());
		}

		u._weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		u.weak_from_this_assign<has_weak_from_this>();
	}

	template <class _Up, class _Ep,
			class = _EnableIfMoveConvertible<unique_ptr<_Up, _Ep>, _Up>,
			class = _EnableIfDeleterConvertible<_Ep>>
	inline unique_ptr(unique_ptr<_Up, _Ep>&& u) noexcept
			: _pointer_and_deleter(std::move(u._pointer_and_deleter))
			, _weak_data(u._weak_data) {

		if (_weak_data) {
			_weak_data->set_pointer(_pointer_and_deleter.get_pointer());
		}

		u._weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		u.template weak_from_this_assign<
				unique_ptr<_Up, _Ep>::has_weak_from_this>();
	}

	unique_ptr(const unique_ptr&) = delete;

	inline ~unique_ptr() noexcept {
		reset(nullptr);
	}

	inline bool is_stolen() const {
		return _weak_data && _weak_data->is_stolen();
	}

	inline void reset(pointer p = nullptr) noexcept {
		if (is_stolen()) {
			_pointer_and_deleter.set_pointer(nullptr);
			_weak_data->set_pointer(nullptr);

			if (_weak_data->count() == 0) {
				delete _weak_data;
			}

			_weak_data = nullptr;
			weak_from_this_assign<has_weak_from_this>();
			return;
		}


		if (_pointer_and_deleter.get_pointer()) {
			_pointer_and_deleter.delete_pointer();
		}

		if (_weak_data) {
			_weak_data->set_pointer(nullptr);

			if (_weak_data->count() == 0) {
				delete _weak_data;
			}
		}

		_pointer_and_deleter.set_pointer(p);
		_weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
	}

	inline void reset(pointer p, alloc_weak_tag awt) noexcept {
		if (_pointer_and_deleter.get_pointer()) {
			_pointer_and_deleter.delete_pointer();
		}

		if (_weak_data) {
			_weak_data->set_pointer(nullptr);

			if (_weak_data->count() == 0) {
				delete _weak_data;
			}
		}

		_pointer_and_deleter.set_pointer(p);
		_weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>(awt);
	}

	inline void reset(pointer p, keep_weak_tag) noexcept {
		if (_pointer_and_deleter.get_pointer()) {
			_pointer_and_deleter.delete_pointer();
		}

		_pointer_and_deleter.set_pointer(p);

		if (p == nullptr) {
			if (_weak_data) {
				_weak_data->set_pointer(nullptr);

				if (_weak_data->count() == 0) {
					delete _weak_data;
				}

				_weak_data = nullptr;
			}

			/// @todo Probably not necessary.
			weak_from_this_assign<has_weak_from_this>();
			return;
		}

		// Pointer is not nullptr.
		if (_weak_data) {
			_weak_data->set_pointer(p);
		}

		/// @todo Probably not necessary.
		weak_from_this_assign<has_weak_from_this>();
	}

	inline void reset(pointer p, keep_or_alloc_weak_tag) noexcept {
		if (_pointer_and_deleter.get_pointer()) {
			_pointer_and_deleter.delete_pointer();
		}

		_pointer_and_deleter.set_pointer(p);


		if (p == nullptr) {
			if (_weak_data) {
				_weak_data->set_pointer(nullptr);

				if (_weak_data->count() == 0) {
					delete _weak_data;
				}

				_weak_data = nullptr;
			}

			/// @todo Probably not necessary.
			weak_from_this_assign<has_weak_from_this>();
			return;
		}

		// Pointer is not nullptr.
		if (_weak_data) {
			_weak_data->set_pointer(p);
		}

		// Create weak data.
		weak_from_this_assign<has_weak_from_this>(alloc_weak_tag{});
	}

	inline unique_ptr& operator=(std::nullptr_t) noexcept {
		reset(nullptr);
		return *this;
	}

	inline unique_ptr& operator=(unique_ptr&& u) noexcept {
		reset(nullptr);

		_pointer_and_deleter = std::move(u._pointer_and_deleter);
		_weak_data = u._weak_data;

		if (_weak_data) {
			_weak_data->set_pointer(_pointer_and_deleter.get_pointer());
		}

		u._weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		u.weak_from_this_assign<has_weak_from_this>();
		return *this;
	}

	void swap(unique_ptr& other) noexcept {
		//		pointer_deleter_type p = other._pointer_and_deleter;
		weak_data_type* weak_data = other._weak_data;

		std::swap(other._pointer_and_deleter, _pointer_and_deleter);
		//		other._pointer_and_deleter = _pointer_and_deleter;
		other._weak_data = _weak_data;
		if (other._weak_data) {
			other._weak_data->set_pointer(
					other._pointer_and_deleter.get_pointer());
		}

		//		_pointer_and_deleter = p;
		_weak_data = weak_data;
		if (_weak_data) {
			_weak_data->set_pointer(_pointer_and_deleter.get_pointer());
		}

		other.weak_from_this_assign<has_weak_from_this>();
		weak_from_this_assign<has_weak_from_this>();
	}

	inline bool is_valid() const noexcept {
		return get() != nullptr && !is_stolen();
	}

	inline pointer get() const noexcept {
		return _pointer_and_deleter.get_pointer();
	}

	inline pointer release() noexcept {
		pointer p = get();

		if (_weak_data) {
			_weak_data->set_pointer(nullptr);

			if (_weak_data->count() == 0) {
				delete _weak_data;
			}
		}

		_pointer_and_deleter = pointer_deleter_type(nullptr, get_deleter());
		_weak_data = nullptr;

		weak_from_this_assign<has_weak_from_this>();
		return p;
	}

	inline pointer operator->() const noexcept {
		return get();
	}

	inline typename std::add_lvalue_reference<element_type>::type
	operator*() const {
		return *(get());
	}


	inline explicit operator bool() const noexcept {
		return is_valid();
	}

	inline deleter_type& get_deleter() noexcept {
		return _pointer_and_deleter.get_deleter();
	}

	inline const deleter_type& get_deleter() const noexcept {
		return _pointer_and_deleter.get_deleter();
	}

	inline void invalidate_weak() {
		if (has_weak_data() && _weak_data->count()) {
			_weak_data->set_pointer(nullptr);
			_weak_data = nullptr;
		}
	}

private:
	friend weak_ptr<element_type>;
	friend enable_weak_from_this<element_type>;

	pointer_deleter_type _pointer_and_deleter;
	mutable weak_data_type* _weak_data = nullptr; // Mutable ???

	inline weak_data_type* get_weak_data() {
		return _weak_data;
	}

	inline bool has_weak_data() const {
		return _weak_data != nullptr;
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
	//  static constexpr bool is_stealable =
	//  std::is_base_of<alx::stealable_unique_ptr, element_type>::value;

	template <bool _Dummy,
			class _D = typename detail::__dependent_type<
					detail::__identity<element_type>, _Dummy>::type>
	using _EnableIfStealable = typename std::enable_if<
			std::is_base_of<alx::stealable_unique_ptr, _D>::value>::type;

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

	template <typename U>
	weak_ptr(const unique_ptr<element_type, U>& u) noexcept {
		if (!u.is_valid()) {
			return;
		}

		// Copy weak data if it exist.
		if (u.has_weak_data()) {
			//			_weak_data = u.get_weak_data();
			_weak_data = u._weak_data;
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

	template <typename U>
	inline weak_ptr& operator=(const unique_ptr<element_type, U>& u) noexcept {
		// Release current instance.
		reset();

		if (!u.is_valid()) {
			return *this;
		}

		// Copy weak data if it exist.
		if (u.has_weak_data()) {
			//			_weak_data = u.get_weak_data();
			_weak_data = u._weak_data;
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

		return _weak_data->is_valid() && !_weak_data->is_stolen();
	}

	inline pointer operator->() noexcept {
		return _weak_data->get();
	}

	inline const_pointer operator->() const noexcept {
		return _weak_data->get();
	}

	inline pointer get() noexcept {
		return _weak_data->get();
	}

	inline const_pointer get() const noexcept {
		return _weak_data->get();
	}

	template <bool _Dummy = true, class = _EnableIfStealable<_Dummy>>
	inline unique_ptr<element_type> steal() {
		if (!is_valid() || !_weak_data) {
			return nullptr;
		}

		pointer p = get();
		_weak_data->set_stolen_pointer();
		return unique_ptr<element_type>(p);
	}

private:
	weak_data_type* _weak_data = nullptr;
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

template <typename _Tp, typename _Deleter>
template <bool __HasWeakFromThis> // Always true.
inline void unique_ptr<_Tp, _Deleter>::weak_from_this_assign(alloc_weak_tag) {
	if (!is_valid()) {
		return;
	}

	get()->_weak_this = *this;
}

} // namespace alx.
