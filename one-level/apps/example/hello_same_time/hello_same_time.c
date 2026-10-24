#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int i;
  volatile int counter = 0; // volatile keeps the compiler from optimizing this out
  sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  if (argc != 2) { 
    Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
    Exit();
  } 

 Printf("hello_world start counting(%d)\n", getpid());
   
  // Convert the command-line strings into integers for use as handles
  s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Count to a large number
  for (i = 0; i < 100000; i++) {
    counter ++;
  }

  Printf("hello_world  finished counting(%d)\n", getpid());
  // // Signal the semaphore to tell the original process that we're done
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("hello_world (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
    Exit();
  }

  exit();
}
