#include "usertraps.h"
#include "misc.h"

void main (int argc, char *argv[])
{
  char *ptr1;
  char *ptr2;
  char *ptr3;
  int i;
  Printf("Starting dynamic heap management test (pid: %d)\n", getpid());

  // Test 1: Allocate a small block
  Printf("Test 1: Allocate 2 KB\n");
  ptr1 = (char*)malloc(2048);

  if (ptr1 != 0) {
    Printf("Successfully allocated 2048 bytes at virtual address %d\n", (int)ptr1);

    // Write to memory to trigger page fault
    for(i = 0; i < 2048; i++) {
      ptr1[i] = 'a';
    }
    Printf("Successfully accessed ptr1\n");    
  }
  else {
    Printf("Failed to allocate 2048 bytes\n");    
  }

  // Test 2: Allocate another block, 8K
  Printf("Test 2: Allocate 8KB\n");
  ptr2 = (char*)malloc(8192);

  if (ptr2 != 0) {
    Printf("Successfully allocated 8KB at virtual address %d\n", (int)ptr2);
    // Write to memory on multiple pages
    for(i = 0; i < 8192; i+=1024) {
      ptr2[i] = 'b';
    }
  }
  else {
    Printf("Failed to allocate 8192 bytes\n");    
  }

  // Test 3: Allocate another block, 32K
  Printf("Test 2: Allocate 32KB\n");
  ptr3 = (char*)malloc(32768);

  if (ptr3 != 0) {
    Printf("Successfully allocated 32KB at virtual address %d\n", (int)ptr3);
    // Write to memory on multiple pages
    for(i = 0; i < 32768; i +=4096) {
      ptr3[i] = 'c';
    }
  }
  else {
    Printf("Failed to allocate 32768 bytes\n");    
  }

  // Test 4: Free blocks
  Printf("Test 4: Free memory\n");
  mfree(ptr1);
  mfree(ptr2);
  mfree(ptr3);
  Printf("Memory freed\n");

  // Test 5: Allocate more than heap size
  Printf("Test 5: Allocate more than heap size\n");
  ptr1 = malloc(70000);
  if (ptr1 == 0) {
    Printf("Success: 70000 byte allocation unsuccessful\n");    
  }
  else {
    Printf("ERROR: Successfully allocated 70000 block\n");
  }
  
  Printf("\nDynamic heap test complete\n");
  Exit();
}
