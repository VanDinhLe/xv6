#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// 1st arg is file name
// 2nd is lseek offset
int main(int argc, char * argv[])
{
  if(argc < 2) {
    printf("Missing arguments\n");
  } 
  int fd = open(argv[1] ,O_RDWR);
  int off = 0;
  char test[] = "***NEW DATA***\n";
  // move offset cursor
  if((off = lseek(fd, atoi(argv[2]))) == -1){
    printf("couldnt set offset lseek err\n");
  }
  // write new data
  if(write(fd, test, sizeof(test)) != sizeof(test)) {
    printf("fail to write new data\n");
  }
  // reset offset cursor for printing
  int n;
  char buf[512];
  off = -1 * (off + sizeof(test));
  lseek(fd, off);

  // print file for comparing
  while((n = read(fd, buf, sizeof(buf))) > 0) {
    printf("buffer:\n");
    if (write(1, buf, n) != n) {
      printf( "Write error\n");
      exit(1);
    }

    printf("\nRawchar:\n");
    for(int i = 0; i < n; i++) {
      printf("0x%x-%c ", buf[i], buf[i]);   // print each byte in hex
    }
    printf("\n");
  }
  exit(0);
}
