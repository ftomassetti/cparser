#include <assert.h>

static int switch1(int k) {
	switch(k) {
	case 42:
		return 5;
	case 13:
		return 7;
	}
	return 3;
}

int main(void)
{
	assert(switch1(42) == 5);
	assert(switch1(13) == 7);
	assert(switch1(700) == 3);
	assert(switch1(-32000) == 3);
	return 0;
}