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
