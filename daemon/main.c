#include <unistd.h>

#define AUSEC_AUTH __
#define AUSEC_WAIT __
#define AUSEC_ANSWER __

int main(int argc, char* argv[])
{
	__syscall3(numero, ausec_auth);

	return 0;
}

