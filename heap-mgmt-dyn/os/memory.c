//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

#define HEAP_MAX_SIZE 65536
#define HEAP_MIN_BLOCK 32
#define HEAP_MAX_ORDER 11
#define NUM_TREE_NODES 4095

static uint32 freemap[(512 + 31)/32];
uint32 ref_count[512 + 31];  // Number of pages
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

typedef struct {
    int is_free;      // 1 if free, 0 if allocated/split
    int is_split;     // 1 if split into children
    int size;         // Size of the block
    int order;        // Order (0 to 7)
    int addr;         // Offset from the start of the heap
} BuddyNode;

// Global buddy tree for the current process (assuming 1 process for simplicity here, 
// normally this might go in the PCB or a 2D array if supporting multiple processes)
static BuddyNode heap_tree[NUM_TREE_NODES];
static int heap_initialized = 0;

// Helper to get left child index
static inline int get_left_child(int index) { return 2 * index + 1; }
// Helper to get right child index
static inline int get_right_child(int index) { return 2 * index + 2; }
// Helper to get parent index
static inline int get_parent(int index) { return (index - 1) / 2; }

void InitHeapTree() {
    int i;
    for (i = 0; i < NUM_TREE_NODES; i++) {
        heap_tree[i].is_free = 0;
        heap_tree[i].is_split = 0;
    }
    // Initialize root node
    heap_tree[0].is_free = 1;
    heap_tree[0].size = HEAP_MAX_SIZE;
    heap_tree[0].order = HEAP_MAX_ORDER;
    heap_tree[0].addr = 0;
    heap_initialized = 1;
}

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
uint32 MemoryGetSize() {
  dbprintf ('m', "total memory: 0x%x ", *((int *)DLX_MEMSIZE_ADDRESS) );
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}

//----------------------------------------------------------------------
//
//	allocate_node: find block
//
//----------------------------------------------------------------------
int allocate_node(int node_idx, int target_order) {
  int left_res;
  int left;
  int right;
  int child_order;
  int child_size;

  // If node split, try children
  if (heap_tree[node_idx].is_split) {
    left_res = allocate_node(get_left_child(node_idx), target_order);
    if (left_res != -1) {
      return left_res;
    }  
    return allocate_node(get_right_child(node_idx), target_order);
  }

  // Node not split
  if (heap_tree[node_idx].is_free && heap_tree[node_idx].order >= target_order) {
    // Correct size
    if (heap_tree[node_idx].order == target_order) {
      heap_tree[node_idx].is_free = 0;
      return node_idx;
    }

    // Not correct size so split
    heap_tree[node_idx].is_free = 0;
    heap_tree[node_idx].is_split = 1;

    left = get_left_child(node_idx);
    right = get_right_child(node_idx);
    child_order = heap_tree[node_idx].order-1;
    child_size = heap_tree[node_idx].size / 2;

    // Left child
    heap_tree[left].is_free = 1;
    heap_tree[left].is_split = 0;
    heap_tree[left].order = child_order;
    heap_tree[left].size = child_size;
    heap_tree[left].addr = heap_tree[node_idx].addr;

    // Right child
    heap_tree[right].is_free = 1;
    heap_tree[right].is_split = 0;
    heap_tree[right].order = child_order;
    heap_tree[right].size = child_size;
    heap_tree[right].addr = heap_tree[node_idx].addr +child_size;

    // prints
    dbprintf ('m', "Created a left child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n",
            heap_tree[left].order, heap_tree[left].addr, heap_tree[left].size,
            heap_tree[node_idx].order, heap_tree[node_idx].addr, heap_tree[node_idx].size);
    dbprintf ('m', "Created a right child node (order = %d, addr = %d, size = %d) of parent (order = %d, addr = %d, size = %d)\n",
            heap_tree[right].order, heap_tree[right].addr, heap_tree[right].size,
            heap_tree[node_idx].order, heap_tree[node_idx].addr, heap_tree[node_idx].size);
    
    // Allocate in left child
    return allocate_node(left, target_order);    
  }
  // Can't allocate
  return -1;
}
//----------------------------------------------------------------------
//
//	malloc
//
//----------------------------------------------------------------------
void* malloc(int memsize)
{
  int adjusted_size;
  int req_size;
  int target_order;
  int allocated_idx;
  uint32 vaddress;
  uint32 paddress;

  if (!heap_initialized) {
    InitHeapTree();
  }

  if ((memsize <= 0) || (memsize > HEAP_MAX_SIZE)) {
    return NULL;
  }

  // Block size multiple of 4
  adjusted_size = memsize;
  if (adjusted_size % 4 != 0) {
    adjusted_size += (4 - (adjusted_size % 4));
  }

  req_size = HEAP_MIN_BLOCK;
  target_order = 0;
  while (req_size < adjusted_size) {
    req_size *= 2;
    target_order++;
  }

  allocated_idx = allocate_node(0, target_order);
  if (allocated_idx == -1) {
    // No blocks large enough
    return NULL;  
  }

  // Heap starts at virtual page 4
  vaddress = (4 * MEM_PAGE_SIZE) + heap_tree[allocated_idx].addr;
  paddress = MemoryTranslateUserToSystem(currentPCB, vaddress);

  // Required print messages
  dbprintf ('m',"Allocated the block: order = %d, addr = %d requested mem size = %d, block size = %d\n",
        heap_tree[allocated_idx].order, heap_tree[allocated_idx].addr, memsize, heap_tree[allocated_idx].size);
  dbprintf( 'm',"Created a heap block of size %d bytes: virtual address 0x%x, physical address 0x%x.\n",
        memsize, vaddress, paddress);
  return (void*)vaddress;
}

//----------------------------------------------------------------------
//
//	mfree
//
//----------------------------------------------------------------------
int mfree(void *ptr)
{
  uint32 heap_base;
  uint32 vaddr;
  uint32 paddr;
  int target_addr;
  int curr;
  int left;
  int right;
  int bytes_freed;
  int buddy;
  int parent;
  int left_node;
  int right_node;

  if (ptr == NULL) {
    return -1;
  }

  // Calculate the heap offset (starts at virtual page 4)
  heap_base = 4 * MEM_PAGE_SIZE;
  vaddr = (uint32)ptr;

  // Error if ptr not in heap
  if ((vaddr < heap_base) || (vaddr >= heap_base + HEAP_MAX_SIZE)) {
    return -1;
  }

  target_addr = vaddr - heap_base;

  // Search in buddy tree
  curr = 0;
  while (heap_tree[curr].is_split) {
    left = get_left_child(curr);
    right = get_right_child(curr);
    // Select the correct side
    if (target_addr >= heap_tree[right].addr) {
      curr = right;
    }
    else {
      curr = left;
    }
  }

  // Verify block is allocated and address match
  if (heap_tree[curr].is_free || heap_tree[curr].addr != target_addr) {
    return -1;
  }

  // Output free messages
  paddr = MemoryTranslateUserToSystem(currentPCB, vaddr);
  bytes_freed = heap_tree[curr].size;

  dbprintf ('m',"Freeing heap block of size %d bytes: virtual address 0x%x, physical address 0x%x.\n",
        bytes_freed, vaddr, paddr);
  dbprintf ('m', "Freed the block: order = %d, addr = %d, size = %d\n",
        heap_tree[curr].order, heap_tree[curr].addr, heap_tree[curr].size);

  // Node now free
  heap_tree[curr].is_free = 1;

  // Rejoin buddies if possible, recursively
  while (curr > 0) {
    parent = get_parent(curr);

    // Get index
    if (curr % 2 != 0) {
      buddy = curr + 1; // buddy is right node
    }
    else {
      buddy = curr - 1; // buddy is left node
    }

    // Buddy free and not split?
    if (heap_tree[buddy].is_free && !heap_tree[buddy].is_split) {
      // Identify left and right
      left_node = (curr % 2 != 0) ? curr : buddy;
      right_node = (curr % 2 != 0) ? buddy : curr;

      // Merge into parent
      heap_tree[parent].is_free = 1;
      heap_tree[parent].is_split = 0;

      // Clean up children
      heap_tree[left_node].is_free = 0;
      heap_tree[right_node].is_free = 0;

      // Printfs
      dbprintf ('m', "Coalesced buddy nodes (order = %d, addr = %d, size = %d) & (order = %d, addr = %d, size = %d)\n",
            heap_tree[left_node].order, heap_tree[left_node].addr, heap_tree[left_node].size,
            heap_tree[left_node].order, heap_tree[left_node].addr, heap_tree[left_node].size);
      dbprintf ('m', "into the parent node (order = %d, addr = %d, size = %d)\n",
            heap_tree[parent].order, heap_tree[parent].addr, heap_tree[parent].size);

      // Move up to parent node
      curr = parent;
    }
    else {
      break;    // stop coalescing
    }
  }
  return bytes_freed;
}

//----------------------------------------------------------------------
//
//	MemorySetFreemap
//
//----------------------------------------------------------------------
void MemorySetFreemap (int p, int b)
{
  uint32	wd = p / 32;
  uint32	bitnum = p % 32;
//  dbprintf ('m', "MemorySetFreemap: %d\n", GetCurrentPid());

  freemap[wd] = (freemap[wd] & invert(1 << bitnum)) | (b << bitnum);
  // dbprintf ('m', "Set freemap entry %d to 0x%x.\n",
	//     wd, freemap[wd]);
}

//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int		maxpage = MemoryGetSize () / MEM_PAGE_SIZE;
  int		curpage;
  dbprintf ('m', "MemoryModuleInit\n");
  dbprintf ('m', "lastosaddress = 0x%x\n", lastosaddress);

  pagestart = (lastosaddress + MEM_PAGE_SIZE - 4) / MEM_PAGE_SIZE;
  dbprintf ('m', "pagestart = 0x%x\n", pagestart);
  freemapmax = (maxpage+31) / 32;
  dbprintf ('m', "freemap has %d entries, memory size is 0x%x.\n",
	    freemapmax, maxpage);
  for (curpage = 0; curpage < maxpage; curpage++) {
    MemorySetFreemap (curpage, 0);
  }
  for (curpage = 0; curpage < freemapmax; curpage++) {
    // Initially, all pages are considered in use.  This is done to make
    // sure we don't have any partially initialized freemap entries.
    freemap[curpage] = 0;
  } 

  nfreepages = 0;
  for (curpage = pagestart; curpage < maxpage; curpage++) {
    nfreepages += 1;
    MemorySetFreemap (curpage, 1);
  }
  // Init ref count array to 0
  for (curpage = 0; curpage < maxpage; curpage++) {
    ref_count[curpage] = 0;
  }
  dbprintf ('m', "Initialized %d free pages.\n", nfreepages);
}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {

  
  int	page = addr / MEM_PAGE_SIZE;
  int offset = addr % MEM_PAGE_SIZE;

  dbprintf ('m', "MemoryTranslateUserToSystem: page %d offset %d\n", page, offset);

  // if (page > pcb->npages) {
  //   dbprintf ('m', "MemoryTranslateUserToSystem: page %d npages %d\n", page, pcb->npages);
  //  return (0);
  // }
  return ((pcb->pagetable[page] & MEM_PTE_MASK) + offset);
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGE_SIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  uint32 fault_address;     // Virt address that caused fault
  uint32 sp;                // Current stack pointer
  uint32 newPage;            // New page to allocate for stack
  uint32 pg_num_sp;          // Page number of current stack pointer
  uint32 pg_num_fault;       // Page number of fault address
  uint32 heap_base_page;
  uint32 max_heap_pages;
  uint32 target_addr;
  int curr;
  int left;
  int right;

  dbprintf ('m', "MemoryPageFaultHandler\n");
  fault_address = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  sp = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER];
  sp -= 8;

  pg_num_sp = sp >> MEM_L1FIELD_FIRST_BITNUM;
  pg_num_fault = fault_address >> MEM_L1FIELD_FIRST_BITNUM;
  dbprintf ('m', "MemoryPageFaultHandler: fault_address=0x%x, sp=0x%x, pg_num_sp=%d, pg_num_fault=%d\n", fault_address, sp, pg_num_sp, pg_num_fault);

  // If the fault page is greater that the adjusted stack pointer page, then
  // then this is valid stack growth and we need to allocate a new page for the stack.
  if ((pg_num_fault == pg_num_sp) || 
      (pg_num_fault == (pg_num_sp + 1))) {
    if (pcb->pagetable[pg_num_fault] & MEM_PTE_VALID) {
      dbprintf ('m', "MemoryPageFaultHandler: page already allocated for stack\n");
      return MEM_SUCCESS;
    }
    // Allocate new page for stack. If this fails, kill process.
    // If success, setup pte and increment npages.
    dbprintf ('m', "MemoryPageFaultHandler: allocating new page for stack\n");
    newPage = MemoryAllocPage();
    if (newPage == 0) {
      dbprintf ('m', "MemoryPageFaultHandler: could not allocate new page for stack\n");
      ProcessKill();
      return MEM_FAIL;
    }
    pcb->pagetable[pg_num_fault] = MemorySetupPte(newPage);   
    pcb->npages++;
    return MEM_SUCCESS;
  } 

  // Now handle heap growth
  heap_base_page = 4;
  max_heap_pages = 16;

  if ((pg_num_fault >= heap_base_page) && (pg_num_fault < (heap_base_page + max_heap_pages))) {
    // Find offset relative to start of heap
    target_addr = fault_address - (heap_base_page * MEM_PAGE_SIZE);

    // Traverse the buddy tre to find the leaf node covering this address
    curr = 0;
    while (heap_tree[curr].is_split) {
      left = get_left_child(curr);
      right = get_right_child(curr);
      if (target_addr >= heap_tree[right].addr) {
        curr = right;
      }
      else {
        curr = left;
      }
    }
    // Already backed by physical page?
    if (heap_tree[curr].is_free == 0) {
      if (pcb->pagetable[pg_num_fault] & MEM_PTE_VALID) {
        return MEM_SUCCESS;   // Already backed by physical page
      }

      // Allocate a new physical page
      newPage = MemoryAllocPage();
      if (newPage == 0) {
        printf("Could not allocate physical page for dynamic heap\n");        
        ProcessKill();
        return MEM_FAIL;
      }

      pcb->pagetable[pg_num_fault] = MemorySetupPte(newPage);
      pcb->npages++;
      printf("Allocated a physical page at %d to back up virtual page %d in the heap\n", newPage, pg_num_fault);
      return MEM_SUCCESS;
    }
  }
  // Illegal access
  dbprintf('m', "MemoryPageFaultHandler: page fault, killing process\n");
  ProcessKill();
  return MEM_FAIL;
}

//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------
int MemoryAllocPage(void) {
  static int	mapnum = 0;
  int		bitnum;
  uint32	v;
  dbprintf ('m', "MemoryAllocPage\n");

  if (nfreepages == 0) {
    return (0);
  }
  dbprintf ('m', "Allocating memory, starting with page %d\n", mapnum);
  while (freemap[mapnum] == 0) {
    mapnum += 1;
    if (mapnum >= freemapmax) {
      mapnum = 0;
    }
  }
  v = freemap[mapnum];
  for (bitnum = 0; (v & (1 << bitnum)) == 0; bitnum++) {
  }
  freemap[mapnum] &= invert(1 << bitnum);
  v = (mapnum * 32) + bitnum;

  //Set ref count for new page
  ref_count[v] = 1;


  dbprintf ('m', "Allocated memory, from map %d, page %d, map=0x%x.\n",
	    mapnum, v, freemap[mapnum]);
  nfreepages -= 1;
  return (v);  
}

//----------------------------------------------------------------------
//
//	MemoryFreePage
//
//	Free a page of memory.
//
//----------------------------------------------------------------------
void MemoryFreePage(uint32 page)
{
  dbprintf ('m', "MemoryFreePage\n");

  // If ref count > 0, decrement
  if (ref_count[page] > 0) {
    ref_count[page]--;
  }

  // Free the memory only if no processes using it
  if (ref_count[page] == 0) {
    MemorySetFreemap(page, 1);
    nfreepages++;
    dbprintf ('m',"Freed page 0x%x, %d remaining.\n", page, nfreepages);
  }
}

uint32 MemorySetupPte (uint32 page) {
  dbprintf ('m', "MemorySetupPte\n");
  return ((page * MEM_PAGE_SIZE) | MEM_PTE_VALID);  
}
