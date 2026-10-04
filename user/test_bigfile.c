#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h" // For BSIZE (1024)

#define NBLOCKS 16523
//#define NBLOCKS 65803
//#define NBLOCKS 65804
//#define NBLOCKS 131338
//#define NBLOCKS 131339
#define BLOCK_SIZE 1024 // Assuming standard xv6 BSIZE

void
main(void)
{
  int fd;
  int i;
  char buf[BLOCK_SIZE];
  int write_count = 0;
  int total_bytes = 0;
  
  printf("bigwrite: Attempting to write %d blocks (%.2f MB)...\n", 
         NBLOCKS, (float)(NBLOCKS * BLOCK_SIZE) / (1024 * 1024));

  // Initialize buffer with some data (e.g., 'X')
  for (i = 0; i < BLOCK_SIZE; i++) {
    buf[i] = 'X';
  }

  // 1. Open or Create the file
  fd = open("bigfile", O_CREATE | O_WRONLY);
  if (fd < 0) {
    printf("bigwrite: ERROR: cannot open bigfile\n");
    exit(1);
  }

  // 2. Loop and write blocks
  for (i = 0; i < NBLOCKS; i++) {
    int ret = write(fd, buf, BLOCK_SIZE);
    
    if (ret == BLOCK_SIZE) {
      write_count++;
      total_bytes += ret;
    } else if (ret >= 0) {
      // Partial write: should not happen for full block size, but handle it.
      printf("bigwrite: WARNING: Partial write on block %d: wrote %d bytes.\n", i, ret);
      break; 
    } else {
      // CRITICAL FAILURE: write() returned -1
      printf("bigwrite: FAILURE on block %d (after %d successful blocks).\n", i, write_count);
      printf("bigwrite: System returned error code %d (likely out of disk blocks).\n", ret);
      break;
    }

    // Optional: Print status at the limit
    if (write_count == 268) {
      printf("bigwrite: PASSED MAX POINTERS (Block 268). File Size: %d bytes.\n", total_bytes);
    }
  }

  close(fd);

  printf("bigwrite: Finished. Successfully wrote %d blocks. Total bytes: %d.\n", write_count, total_bytes);
  
  if (write_count < NBLOCKS) {
    printf("bigwrite: RESULT: WRITE FAILED at the file size limit.\n");
  }
  
  exit(0);
}
