#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  void *ptr1;
  void *ptr2;
  void *ptr3;
  void *ptr4;

  Printf("Starting heap management test (pid: %d)\n", getpid());

  // Test 1: Allocate a small block
  Printf("Test 1: Allocate 40 bytes\n");
  ptr1 = malloc(40);

  if (ptr1 != 0) {
    Printf("Successfully allocated 40 bytes at virtual address 0x%d\n", (int)ptr1);
  }
  else {
    Printf("Failed to allocate 40 bytes\n");    
  }

  // Test 2: Allocate another block, 100 bytes
  Printf("Test 2: Allocate 100 bytes\n");
  ptr2 = malloc(100);

  if (ptr2 != 0) {
    Printf("Successfully allocated 100 bytes at virtual address 0x%d\n", (int)ptr2);
  }
  else {
    Printf("Failed to allocate 100 bytes\n");    
  }

  // Test 3: Allocate a large block, 2000 bytes
  ptr3 = malloc(2000);
  if (ptr3 != 0) {
    Printf("Successfully allocated 100 bytes at virtual address 0x%d\n", (int)ptr3);
  }
  else {
    Printf("Failed to allocate 100 bytes\n");    
  }

  // Test 4: Free blocks
  Printf("Freeing 40 bytes\n");
  mfree(ptr1);

  // Test 5: Free blocks
  Printf("Freeing 100 bytes\n");
  mfree(ptr2);

  // Test 6: Free blocks
  Printf("Freeing 2000 bytes\n");
  mfree(ptr3);

  // Test 7: out of bounds allocation
  ptr4 = malloc(5000);
  if (ptr4 != 0) {
    Printf("ERROR: Successfully allocated 5000 blocks at virtual address 0x%d\n", (int)ptr4);
  }
  else {
    Printf("Success: 5000 byte allocation unsuccessful\n");
  }
  
  Printf("\nHeap test complete\n");
  Exit();
}
