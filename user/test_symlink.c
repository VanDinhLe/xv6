#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(void)
{
  int fd;
  char buf[128];
  int n;

  printf("=== Symlink test program ===\n");

  // 1. Create a file f1
  fd = open("f1", O_CREATE | O_RDWR);
  if(fd < 0){
    printf("create f1 failed\n");
    exit(1);
  }
  write(fd, "hello-from-f1", 13);
  close(fd);
  printf("created file f1\n");

  // 2. Create a symlink link_to_f1 -> f1
  if(symlink("f1", "link_to_f1") < 0){
    printf("symlink creation failed\n");
    exit(1);
  }
  printf("created symlink link_to_f1 -> f1\n");

  // 3. Open symlink normally: should follow and open f1
  fd = open("link_to_f1", O_RDONLY);
  if(fd < 0){
    printf("open(link_to_f1) failed\n");
    exit(1);
  }
  n = read(fd, buf, sizeof(buf)-1);
  if(n < 0){ printf("read failed\n"); close(fd); exit(1); }
  buf[n] = '\0';
  printf("read via link (followed): %s\n", buf);
  close(fd);

  // 4. Open the symlink with O_NOFOLLOW -> should open symlink itself
  fd = open("link_to_f1", O_RDONLY | O_NOFOLLOW);
  if(fd < 0){
    printf("open(link_to_f1, O_NOFOLLOW) failed\n");
    exit(1);
  }
  n = read(fd, buf, sizeof(buf)-1);
  if(n < 0){ printf("read symlink fd failed\n"); close(fd); exit(1); }
  buf[n] = '\0';
  printf("symlink contents (target path): %s\n", buf);
  close(fd);

  // 5. Try opening the symlink for writing with O_NOFOLLOW -> should fail
  fd = open("link_to_f1", O_WRONLY | O_NOFOLLOW);
  if(fd < 0){
    printf("open(link_to_f1, O_WRONLY|O_NOFOLLOW) failed as expected\n");
  } else {
    printf("unexpectedly succeeded in opening symlink for write\n");
    close(fd);
  }

  // 6. Create a symlink pointing to non-existent file
  if(symlink("missing_file", "link_to_missing") < 0){
    printf("symlink to missing_file creation failed\n");
    exit(1);
  }
  printf("created symlink link_to_missing -> missing_file\n");

  fd = open("link_to_missing", O_RDONLY);
  if(fd < 0){
    printf("open(link_to_missing) failed as expected (target does not exist)\n");
  } else {
    printf("unexpectedly opened symlink to missing file\n");
    close(fd);
  }

  // 7. Demonstrate cycle detection
  symlink("cycle2", "cycle1");
  symlink("cycle1", "cycle2");
  if(open("cycle1", O_RDONLY) < 0){
    printf("opening cycle1 failed as expected due to cycle or max depth\n");
  } else {
    printf("opening cycle1 unexpectedly succeeded\n");
  }

  printf("=== Symlink test program finished ===\n");
  exit(0);
}

