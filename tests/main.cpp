#include <alx_unique_ptr/alx_unique_ptr.hpp>
#include <gtest/gtest.h>

namespace {
TEST(alx_unique_ptr, basics) {
	EXPECT_TRUE(true);
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
