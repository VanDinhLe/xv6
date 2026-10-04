#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
	// hello from user prog
	printf("Hello Xv6!\n");
	// hello from kernel
	if(hello() != 0){
		printf("Hello failed\n");
	}
	exit(0);
}
