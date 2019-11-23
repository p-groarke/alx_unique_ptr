#include "test_util/privablic.h"
#include <alx_unique_ptr/unique_ptr.hpp>
#include <gtest/gtest.h>
#include <iostream>

size_t allocations = 0;
size_t deallocations = 0;
void* operator new(std::size_t count) {
	++allocations;
	return malloc(count);
}

void operator delete(void* p) noexcept {
	--deallocations;
	free(p);
}


struct A {
	int a, b, c;
};

struct B : public alx::stealable_unique_ptr {
	B() = default;

	B(int aa, int bb, int cc)
			: a(aa)
			, b(bb)
			, c(cc) {
	}
	~B() {
		std::cout << "DELETE B" << std::endl;
	}
	int a, b, c;
};


struct C : public alx::enable_weak_from_this<C> {
	C() = default;

	C(int aa, int bb, int cc)
			: a(aa)
			, b(bb)
			, c(cc) {
	}
	~C() {
		std::cout << "DELETE C" << std::endl;
	}
	int a, b, c;
};
template <typename T>
struct UniquePtrGetWeakData {
	typedef alx::detail::unique_weak_data<T>* (alx::unique_ptr<T>::*type)();
};

template <typename T>
struct WeakGetWeakData {
	typedef alx::detail::unique_weak_data<T>*(alx::weak_ptr<T>::*type);
};

template struct private_method<UniquePtrGetWeakData<A>,
		&alx::unique_ptr<A>::get_weak_data>;
template struct private_member<WeakGetWeakData<A>,
		&alx::weak_ptr<A>::_weak_data>;

template <typename T>
inline alx::detail::unique_weak_data<T>* get_weak_data(alx::unique_ptr<T>& u) {
	return (&u->*func<UniquePtrGetWeakData<T>>::ptr)();
}

template <typename T>
inline alx::detail::unique_weak_data<T>* get_weak_data(alx::weak_ptr<T>& u) {
	return u.*member<WeakGetWeakData<T>>::value;
}

template <typename T>
struct CustomDeleter {
	CustomDeleter(int k)
			: _k(k) {
	}

	CustomDeleter(const CustomDeleter&) = delete;
	//  CustomDeleter(CustomDeleter&&) = default;
	CustomDeleter(CustomDeleter&& cd)
			: _k(cd._k) {
	}

	CustomDeleter& operator=(CustomDeleter&& cd) {
		_k = cd._k;
		return *this;
	}
	inline void operator()(T* ptr) {
		std::cout << "delete custom deleter pointer " << _k << std::endl;
		delete ptr;
	}

	int _k;
};

namespace {
TEST(alx_unique_ptr, move_constructor) {
	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	alx::unique_ptr<A> p1(std::move(p0));
	EXPECT_FALSE(p0.is_valid());
	EXPECT_TRUE(p1.is_valid());
	EXPECT_TRUE(p1->a == 8 && p1->b == 2 && p1->c == 90);
}

TEST(alx_unique_ptr, move_equal) {
	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };
	alx::unique_ptr<A> p1;

	EXPECT_TRUE(p0.is_valid());
	EXPECT_FALSE(p1.is_valid());

	p1 = std::move(p0);

	EXPECT_FALSE(p0.is_valid());
	EXPECT_TRUE(p1.is_valid());
	EXPECT_TRUE(p1->a == 8 && p1->b == 2 && p1->c == 90);
}


TEST(alx_unique_ptr, weak_unique_alive) {
	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };
	alx::weak_ptr<A> w0(p0);

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	EXPECT_TRUE(w0.is_valid());
	EXPECT_TRUE(w0->a == 8 && w0->b == 2 && w0->c == 90);

	w0.reset();

	EXPECT_FALSE(w0.is_valid());
	EXPECT_FALSE(get_weak_data(p0) == nullptr);

	auto* weak_data = get_weak_data(p0);
	EXPECT_EQ(weak_data->count(), 0);
	EXPECT_TRUE(weak_data->get() == p0.get());
}

TEST(alx_unique_ptr, case_weak_unique_dead) {
	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };
	alx::weak_ptr<A> w0(p0);

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	EXPECT_TRUE(w0.is_valid());
	EXPECT_TRUE(w0->a == 8 && w0->b == 2 && w0->c == 90);

	p0.reset();

	EXPECT_FALSE(w0.is_valid());
	EXPECT_EQ(get_weak_data(p0), nullptr);

	auto* weak_data = get_weak_data(w0);
	EXPECT_EQ(weak_data->count(), 1);
	EXPECT_FALSE(weak_data->is_valid());
	EXPECT_EQ(weak_data->get(), nullptr);

	{
		alx::weak_ptr<A> w1(w0);
		EXPECT_EQ(weak_data->count(), 2);
		EXPECT_FALSE(weak_data->is_valid());
		EXPECT_EQ(weak_data->get(), nullptr);
	}

	EXPECT_EQ(weak_data->count(), 1);
}


TEST(alx_unique_ptr, custom_deleter) {
	//  alx::unique_ptr<A, CustomDeleter<A>> p1(new A{ 8, 2, 90 });
	//  CustomDeleter<A> cd(32);
	alx::unique_ptr<A, CustomDeleter<A>> p0(
			new A{ 8, 2, 90 }, CustomDeleter<A>(32), alx::alloc_weak_tag{});
	alx::unique_ptr<A, CustomDeleter<A>> p1(std::move(p0));


	alx::weak_ptr<A> w0(p1);
	//    alx::unique_ptr<A, CustomDeleter<A>> p2(std::move(p0));
	//    p2.reset();
	EXPECT_TRUE(w0.is_valid());
	p1 = alx::unique_ptr<A, CustomDeleter<A>>(
			new A{ 8, 2, 90 }, CustomDeleter<A>(332));

	EXPECT_FALSE(w0.is_valid());
}

TEST(alx_unique_ptr, reset) {
	alx::unique_ptr<A> p0(new A{ 8, 2, 90 });
	alx::weak_ptr<A> w0(p0);
	EXPECT_TRUE(w0.is_valid());
	p0.reset(new A{ 1, 2, 3 }, alx::keep_weak_tag{});
	EXPECT_TRUE(w0.is_valid());

	EXPECT_TRUE(w0->a == 1 && w0->b == 2 && w0->c == 3);

	alx::unique_ptr<A> p1(new A{ 8, 2, 90 });
	alx::weak_ptr<A> w1(p1);
	EXPECT_TRUE(w1.is_valid());
	p1.reset(new A{ 1, 2, 3 });
	EXPECT_FALSE(w1.is_valid());
}

TEST(alx_unique_ptr, steal) {
	alx::weak_ptr<B> w0;
	alx::unique_ptr<B> p0(new B{ 1, 2, 3 });
	w0 = p0;
	alx::unique_ptr<B> p1(w0.steal());

	EXPECT_FALSE(w0.is_valid());
	EXPECT_FALSE(p0.is_valid());
	EXPECT_TRUE(p0.is_stolen());
	EXPECT_TRUE(p1.is_valid());

	EXPECT_TRUE(p1->a == 1 && p1->b == 2 && p1->c == 3);

	alx::weak_ptr<B> w1(p1);
	EXPECT_TRUE(w1.is_valid());
	EXPECT_TRUE(w1->a == 1 && w1->b == 2 && w1->c == 3);
}

TEST(alx_unique_ptr, invalid_weak) {

	alx::unique_ptr<B> p0(new B{ 1, 2, 3 });
	alx::weak_ptr<B> w0(p0);

	EXPECT_TRUE(w0.is_valid());
	p0.invalidate_weak();
	EXPECT_FALSE(w0.is_valid());
}

TEST(alx_unique_ptr, weak_from_this) {

	alx::unique_ptr<C> p0(new C{ 1, 2, 3 });

	alx::weak_ptr<C> w0 = p0.get()->weak_from_this();
	EXPECT_TRUE(w0.is_valid());
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
