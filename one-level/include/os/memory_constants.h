#ifndef	_memory_constants_h_
#define	_memory_constants_h_

//------------------------------------------------
// #define's that you are given:
//------------------------------------------------

// We can read this address in I/O space to figure out how much memory
// is available on the system.
#define	DLX_MEMSIZE_ADDRESS	0xffff0000

// Return values for success and failure of functions
#define MEM_SUCCESS 1
#define MEM_FAIL -1

//--------------------------------------------------------
// Put your constant definitions related to memory here.
// Be sure to prepend any constant names with "MEM_" so 
// that the grader knows they are defined in this file.

//--------------------------------------------------------
#define MEM_MAX_PHYS_SIZE 0x200000 // 2 MB of physical memory
#define MEM_MAX_VIRT_SIZE 0x400000 // 4 MB of virtual memory
#define MEM_PTE_VALID 0x00000001 // Page table entry is valid
#define MEM_PTE_DIRTY 0x00000002 // Page table entry is dirty
#define MEM_PAGE_SIZE 4096	// Size of a page in bytes
#define MEM_ADDRESS_OFFSET_MASK 0x00000FFF // Mask to get offset within a page
#define MEM_PTE_MASK 0xFFFFF000 // Mask to get the page frame address from a PTE
#define MEM_NUM_PAGES (MEM_MAX_PHYS_SIZE / MEM_PAGE_SIZE) // Number of pages in physical memory

#define MEM_L1FIELD_FIRST_BITNUM 12
#define MEM_MAX_VIRTUAL_ADDRESS 0x003FFFFF // Maximum virtual address (4 MB - 1)
#endif	// _memory_constants_h_
