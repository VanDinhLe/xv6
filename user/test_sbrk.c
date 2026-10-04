#include "kernel/types.h"
#include "user/user.h"

#define PGSIZE 4096

int main(int argc, char * argv[])
{
	// get number of pages from command line
	int n = atoi(argv[1]);
	printf("user request %d pages\n", n);
	
	// allocate more pages 
	char *p = sys_sbrk(PGSIZE * n, 0);
	// write to each page
	for(int i = 0; i < n ; i++)
	{
	  p[i * PGSIZE] = 'a';
	}
	// print out allocator information
	printf("page fault %d\n", pagefault());
	printf("page allocated %d\n", pagecount());
	exit(0);
	
}

