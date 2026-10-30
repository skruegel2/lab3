#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  // Test of invalid memory access
  int* ptr;
  int deref;

  // Test for out of invalid memory access.
  ptr = (int*)(4*1024*1024 + 10);
  deref = *ptr;
  Printf("hello_world: deref = %d\n", deref);
  exit();
}
