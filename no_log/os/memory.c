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

static uint32 freemap[(512 + 31)/32];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

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
    dbprintf('m',"pg_num_fault=%d pte=0x%x\n",
       pg_num_fault,
       pcb->pagetable[pg_num_fault]);    
    pcb->npages++;
    return MEM_SUCCESS;
  } 
  else {
    dbprintf ('m', "MemoryPageFaultHandler: page fault, killing process\n");
    ProcessKill();
  }
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
  MemorySetFreemap (page, 1);
  nfreepages += 1;
  dbprintf ('m',"Freed page 0x%x, %d remaining.\n", page, nfreepages);
}

uint32 MemorySetupPte (uint32 page) {
  dbprintf ('m', "MemorySetupPte\n");
  return ((page * MEM_PAGE_SIZE) | MEM_PTE_VALID);  
}
