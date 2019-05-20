#include <iostream>
#include "../testdll/dllclass.h"

int main() {
	test_class a;
	a.dosomething();
	std::cout << a.get_state();
    std::cout << "Hello World!\n"; 
}
