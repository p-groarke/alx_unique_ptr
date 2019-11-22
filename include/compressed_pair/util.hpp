#pragma once

#include <type_traits>

namespace alx {
template <typename T, bool small_>
struct ct_imp2 {
	typedef const T& param_type;
};

template <typename T>
struct ct_imp2<T, true> {
	typedef const T param_type;
};

template <typename T, bool isp, bool b1>
struct ct_imp {
	typedef const T& param_type;
};

template <typename T, bool isp>
struct ct_imp<T, isp, true> {
	typedef typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type
			param_type;
};

template <typename T, bool b1>
struct ct_imp<T, true, b1> {
	typedef T const param_type;
};

template <typename T>
struct call_traits {
public:
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef typename ct_imp<T, std::is_pointer<T>::value,
			std::is_arithmetic<T>::value>::param_type param_type;
};

template <typename T>
struct call_traits<T&> {
	typedef T& value_type;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T& param_type;
};

template <typename T, std::size_t N>
struct call_traits<T[N]> {
private:
	typedef T array_type[N];

public:
	typedef const T* value_type;
	typedef array_type& reference;
	typedef const array_type& const_reference;
	typedef const T* const param_type;
};

template <typename T, std::size_t N>
struct call_traits<const T[N]> {
private:
	typedef const T array_type[N];

public:
	typedef const T* value_type;
	typedef array_type& reference;
	typedef const array_type& const_reference;
	typedef const T* const param_type;
};

} // namespace alx
