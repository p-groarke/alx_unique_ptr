#include <alx_unique_ptr/alx_unique_ptr.hpp>
#include <gtest/gtest.h>

size_t allocations = 0;
size_t deallocations = 0;
void* operator new(std::size_t count) {
	++allocations;
	return malloc(count);
}

void operator delete(void* p) {
	--deallocations;
	free(p);
}


namespace {
TEST(alx_unique_ptr, basics) {
	EXPECT_TRUE(true);
}
} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
