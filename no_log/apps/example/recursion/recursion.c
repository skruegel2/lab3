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
  // Now print a message to show that everything worked
  Printf("recursion (%d)\n", getpid());
  recurse(0);
  exit();
}
