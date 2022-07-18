#include <stdio.h>
#include <sys/types.h>
int main(int argc, char ** argv) {
	printf("FD_SETSIZE: %d \n", FD_SETSIZE);
	return 0;
}