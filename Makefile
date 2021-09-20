hello: src/hello-world.cc
	mpic++ $< -o $@
	mpirun $@
