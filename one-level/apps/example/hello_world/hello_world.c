#include "usertraps.h"
#include "misc.h"

void recurse(int depth)
{
    char buffer[1024];

    Printf("depth=%d\n", depth);

    recurse(depth + 1);
}

void main (int argc, char *argv[])
{
  // Test of invalid, unallocated memory access
  // int* ptr;
  // int deref;
  // sem_t s_procs_completed; // Semaphore to signal the original process that we're done

  // if (argc != 2) { 
  //   Printf("Usage: %s <handle_to_procs_completed_semaphore>\n"); 
  //   Exit();
  // } 

  // Convert the command-line strings into integers for use as handles
  // s_procs_completed = dstrtol(argv[1], NULL, 10);

  // Test for out of invalid memory access.
  // ptr = (int*)(4*1024*1024 + 10);
  // Test for unallocated memory access.
  // ptr = (int*)(3*1024*1024);
  // deref = *ptr;
  // Printf("hello_world: deref = %d\n", deref);
  // Now print a message to show that everything worked
  Printf("hello_world (%d)\n", getpid());
  recurse(0);
  // // Signal the semaphore to tell the original process that we're done
  // if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
  //   Printf("hello_world (%d): Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
  //   Exit();
  // }

  // Printf("hello_world (%d): Done!\n", getpid());
}
