#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int child_pid;
  // if (argc != 2) {
  //   Printf("Usage: %s <number of hello world processes to create>\n", argv[0]);
  //   Exit();
  // }
  Printf("Parent pid: (%d)\n", getpid());

  child_pid = fork();

  if (child_pid != 0) {
    // This is the parent process
    Printf("This is the parent process pid: (%d)\n", getpid());
    Printf("Child process pid: (%d)\n", child_pid);
  } else {
    // This is the child process
    Printf("This is the child process pid: (%d)\n", getpid());
  }
}
