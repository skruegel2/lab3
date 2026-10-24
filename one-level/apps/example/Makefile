default:
	cd makeprocs; make
	cd hello_world; make
	cd same_time; make
	cd hello_same_time; make

clean:
	cd makeprocs; make clean
	cd hello_world; make clean
	cd same_time; make clean
	cd hello_same_time; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 5; ee469_fixterminal
