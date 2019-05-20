#include "dllclass.h"

void test_class::dosomething() {
	++count;
}

size_t test_class::get_state() {
	return count;
}
