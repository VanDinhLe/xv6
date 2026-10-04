#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// print inf relates to proc
void
eval_rtime(int time, int pid){
  if(time == 0)
  {
    printf("pid: %d has not been scheduled yet\n", pid);
  }
  else if (time < 0){
    printf("pid: %d has not been created\n", pid);
  } else {
    printf("pid: %d has running time of :%d\n", pid, time);
  }
}

int
main(int argc, char * argv[])
{
  int pid, time;
  if(argc < 2){
    pid = getpid();
    printf("pid %d\n", pid);
    for(int i = 0; i < 100000000; i++);
    time = ticks_running(pid);
    printf("syscall tick_run: %d\n", time);
    printf("job length %d\n", sjf_job_length(pid));
    eval_rtime(time, pid);
    exit(0);
  }
  
  for(int i = 1; i < argc; i++)
  {
    pid = atoi(argv[i]); 
    printf("pid %d\n", pid);
    time = ticks_running(pid);
    printf("syscall tick_run: %d\n", time);
    printf("job length %d\n", sjf_job_length(pid));
    eval_rtime(time, pid);
  }
  exit(0);
}
