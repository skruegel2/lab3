default:
	cd makeprocs; make
	cd hello_world; make
	cd heap_test; make
	cd dyn_heap_test; make

clean:
	cd makeprocs; make clean
	cd hello_world; make clean
	cd heap_test; make clean
	cd dyn_heap_test; make clean
run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal
