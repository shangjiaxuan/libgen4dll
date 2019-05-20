#pragma once

#ifdef _WINDLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

class EXPORT test_class {
public:
	void dosomething();
	size_t get_state();
private:
	size_t count = 0;
};



