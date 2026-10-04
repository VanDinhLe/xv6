// user/schedtest.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N_CHILDREN 3
#define ITERATIONS 100000000 // A large number for a noticeable run time

void
eval_rtime(int time, int pid){
  printf("########################\n");
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

// Function that consumes CPU time in user space
void
busy_loop(int pid, int extra)
{
    // A dummy calculation to ensure the compiler doesn't optimize the loop away
    volatile int x = 0;

    printf("PID %d start at ticks %d burst %d\n", pid, uptime(), sjf_job_length(pid));

    for (int i = 0; i < ITERATIONS + extra; i++) {
        x += i;

        // Optional: Periodically call a system call (like getpid or sleep(0))
        // to force a trap into the kernel and update runtime counters.
        if (i % (ITERATIONS / 10) == 0) {
            // Uncomment if you want more verbose output during testing
            //printf("PID %d progress: %d\n", pid, i / (ITERATIONS / 10) * 10);
        }
    }

    printf("PID %d finished at%d.\n", pid, uptime());
    //eval_rtime(ticks_running(pid), pid);
    exit(x);
}

int
main(int argc, char *argv[])
{
    printf("--- Scheduler Test: Creating %d children ---\n", N_CHILDREN);
    int extra = 0;
    for (int i = 0; i < N_CHILDREN; i++) {
        int pid = fork();

        if (pid < 0) {
            // Fork failed
            fprintf(2, "schedtest: fork failed\n");
            break;
        } else if (pid == 0) {
            // Child process executes the busy loop
            busy_loop(getpid(), extra);
            // Child process exits here
        }
	//pause(10);
	extra += 10000;
        // Parent process continues to the next loop iteration to fork the next child
    }

    // Parent waits for all children to finish
    int pid_waited;
    int status;
    while ((pid_waited = wait(&status)) != -1) {
        printf("Parent: Child PID %d exited with status %d.\n", pid_waited, status);
    }

    printf("--- All children finished. Test complete. ---\n");
    exit(0);
}
