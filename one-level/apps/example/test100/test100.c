#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  int idx;
  Printf("100 sequential process test.\n");
  for(idx = 0; idx < 100; idx++)
  {
    Printf("idx = %d\n", idx);
    process_create("helloworld.dlx.obj", NULL);
    yield();
  }
  exit();
}
