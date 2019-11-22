#include <alx/unique_ptr.hpp>
#include <gtest/gtest.h>

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

namespace {
TEST(alx_unique_ptr, move_constructor) {
	struct A {
		int a, b, c;
	};

	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	alx::unique_ptr<A> p1(std::move(p0));
	EXPECT_FALSE(p0.is_valid());
	EXPECT_TRUE(p1.is_valid());
	EXPECT_TRUE(p1->a == 8 && p1->b == 2 && p1->c == 90);
}

TEST(alx_unique_ptr, move_equal) {
	struct A {
		int a, b, c;
	};

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
	struct A {
		int a, b, c;
	};

	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };
	alx::weak_ptr<A> w0(p0);

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	EXPECT_TRUE(w0.is_valid());
	EXPECT_TRUE(w0->a == 8 && w0->b == 2 && w0->c == 90);

	w0.reset();

	EXPECT_FALSE(w0.is_valid());
	EXPECT_TRUE(p0.test__has_weak_data());

	auto* weak_data = p0.test__get_weak_data();
	EXPECT_EQ(weak_data->count(), 0);
	EXPECT_TRUE(weak_data->get() == p0.get());
}

TEST(alx_unique_ptr, case_weak_unique_dead) {
	struct A {
		int a, b, c;
	};

	alx::unique_ptr<A> p0 = new A{ 8, 2, 90 };
	alx::weak_ptr<A> w0(p0);

	EXPECT_TRUE(p0.is_valid());
	EXPECT_TRUE(p0->a == 8 && p0->b == 2 && p0->c == 90);

	EXPECT_TRUE(w0.is_valid());
	EXPECT_TRUE(w0->a == 8 && w0->b == 2 && w0->c == 90);

	p0.reset();

	EXPECT_FALSE(w0.is_valid());
	EXPECT_FALSE(p0.test__has_weak_data());

	auto* weak_data = w0.test__get_weak_data();
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
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
