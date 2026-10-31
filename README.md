# lab3
Question 1
    User test program 1
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the hello_world app by executing mainframer.sh 'cd apps/example/hello_world && make clean'
    4. Build the hello_world app by executing mainframer.sh 'cd apps/example/hello_world && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u hello_world.dlx.obj > ./../log.txt
    6. Examine log.txt at ./one-level and see print hello_world from hello_world.dlx.obj.
    User test program 2
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the invalid_access app by executing mainframer.sh 'cd apps/example/invalid_access && make clean'
    4. Build the invalid_access app by executing mainframer.sh 'cd apps/example/invalid_access && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u invalid_access.dlx.obj ./../log.txt  
    6. Examine log.txt at ./one-level and see TRAP_ADDRESS after reference to illegal address.
    User test program 3
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the not_currently_allocated app by executing mainframer.sh 'cd apps/example/not_currently_allocated && make clean'
    4. Build the not_currently_allocated app by executing mainframer.sh 'cd apps/example/not_currently_allocated && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u not_currently_allocated.dlx.obj ./../log.txt  
    6. Examine log.txt at ./one-level and see TRAP_PAGEFAULT after reference to unallocated address.
    User test program 4
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the recursion app by executing mainframer.sh 'cd apps/example/recursion && make clean'
    4. Build the recursion app by executing mainframer.sh 'cd apps/example/recursion && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u recursion.dlx.obj ./../log.txt  
    6. Examine log.txt at ./one-level.  Each recursion will print 'depth'.  Eventually no more stack can be 
        allocated, and the program fails with a TRAP_PAGEFAULT.
    User test program 5
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the makeprocs app by executing mainframer.sh 'cd apps/example/makeprocs && make clean'
    4. Build the makeprocs app by executing mainframer.sh 'cd apps/example/makeprocs && make'
    3. Clean the hello_sequential app by executing mainframer.sh 'cd apps/example/hello_sequential && make clean'
    4. Build the hello_sequential app by executing mainframer.sh 'cd apps/example/hello_sequential && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u makeprocs.dlx.obj 100 ./../log.txt  
    6. Examine log.txt at ./one-level and watch 100 instances of the hello_sequential application execute.
    User test program 6
    1. Clean the OS by changing directory to one-level and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to one-level and executing mainframer.sh 'cd os && make'
    3. Clean the same_time app by executing mainframer.sh 'cd apps/example/same_time && make clean'
    4. Build the same_time app by executing mainframer.sh 'cd apps/example/same_time && make'
    3. Clean the hello_same_time app by executing mainframer.sh 'cd apps/example/hello_same_time && make clean'
    4. Build the hello_same_time app by executing mainframer.sh 'cd apps/example/hello_same_time && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u same_time.dlx.obj 30 ./../log.txt  
    6. Examine log.txt at ./one-level and watch 30 instances of the hello_same application execute at the same time by
        using the printfs 'hello_world finished counting.'

Files modified:
OS
1. memory.c
2. process.c
3. traps.c
4. memory.h
5. memory_constants.h
6. process.h
Apps
1. hello_world
2. invalid_access
3. not_currently_allocated
4. recursion
5. makeprocs
6. hello_sequential
7. same_time
8. hello_same_time

Question 2
    User test program 1
    1. Clean the OS by changing directory to fork and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to fork and executing mainframer.sh 'cd os && make'
    3. Clean the fork_test_1 app by executing mainframer.sh 'cd apps/example/fork_test_1 && make clean'
    4. Build the fork_test_1 app by executing mainframer.sh 'cd apps/example/fork_test_1 && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u fork_test_1.dlx.obj > ./../log.txt
    6. Examine log.txt at ./fork and see pid printf from parent and child generated by fork_test_1.dlx.obj. Also
    see the parent and child page table entries.  After the fork there is a TRAP_ROP_ACCESS exception, with parent
    and child page table entries.
Files modified:
OS
1. memory.c
2. process.c
3. traps.c
4. memory.h
5. memory_constants.h
6. process.h
Apps.
1. fork_test_1.c

Question 3.
    User test program 1
    1. Clean the OS by changing directory to heap-mgmt and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to heap-mgmt and executing mainframer.sh 'cd os && make'
    3. Clean the heap_test app by executing mainframer.sh 'cd apps/example/heap_test && make clean'
    4. Build the heap_test app by executing mainframer.sh 'cd apps/example/heap_test && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u heap_test.dlx.obj > ./../log.txt
    6. Examine log.txt at ./heap-mgmt and seven numbered allocation tests with printf's beginning 'Test '.  You
    will see allocations of various sizes followed by a failed allocation (too large).

Files modified:
OS
1. memory.c
2. process.c
3. traps.c
4. memory.h
5. memory_constants.h
6. process.h
Apps.
1. heap_test.c    

Question 4.
    User test program 1
    1. Clean the OS by changing directory to heap-mgmt-dyn and executing mainframer.sh 'cd os && make clean'
    2. Build the OS by changing directory to heap-mgmt-dyn and executing mainframer.sh 'cd os && make'
    3. Clean the heap_test app by executing mainframer.sh 'cd apps/example/dyn-heap_test && make clean'
    4. Build the heap_test app by executing mainframer.sh 'cd apps/example/dyn-heap_test && make'
    5. Change to the bin directory and run dlxsim -D m -x os.dlx.obj -a -D m -u dyn-heap-test.dlx.obj > ./../log.txt
    6. Examine log.txt at ./heap-mgmt and seven numbered allocation tests with printf's beginning 'Test '.  You
    will see allocations of various sizes followed by a failed allocation (too large).

Files modified:
OS
1. memory.c
2. process.c
3. traps.c
4. memory.h
5. memory_constants.h
6. process.h
Apps.
1. dyn_heap_test.c 