#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  // Test of unallocated memory access
  int* ptr;
  int deref;
  // Test for unallocated memory access.
  ptr = (int*)(3*1024*1024);
  deref = *ptr;
  Printf("not_currently_allocated: deref = %d\n", deref);
  exit();
}
