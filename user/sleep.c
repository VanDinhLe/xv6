#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	// check for sleep time
	if(argc < 2) {
		printf("please provide sleep time\n");
		exit(0);
	}
	int ticks = atoi(argv[1]);
	// put to sleep if no err
	printf("Nothing happens for a little while\n"); 
        if(pause(ticks) != 0){
                printf("Sleep error\n");
        }
        exit(0);
}

