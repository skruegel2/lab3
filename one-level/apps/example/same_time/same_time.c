#include "usertraps.h"
#include "misc.h"

#define HELLO_WORLD "hello_same_time.dlx.obj"

void main (int argc, char *argv[])
{
  int num_hello_world = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes

  if (argc != 2) {
    Printf("Usage: %s <number of hello world processes to create>\n", argv[0]);
    Exit();
  }

  Printf("same_time (%d): same_time started\n", getpid());

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("same_time (%d): Bad sem_create\n", getpid());
    Exit();
  }

  ditoa(s_procs_completed, s_procs_completed_str);

  // First loop: Create all 30 processes simultaneously
  for(i = 0; i < num_hello_world; i++) {
    Printf("same_time (%d): Creating hello world #%d\n", getpid(), i);
    process_create(HELLO_WORLD, s_procs_completed_str, NULL);
  }

  // Second loop: Wait for all 30 processes to complete
  for(i = 0; i < num_hello_world; i++) {
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("same_time (%d): Bad sem_wait\n", getpid());
      Exit();
    }
  }

}
