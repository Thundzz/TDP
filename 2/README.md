# Compilation:

* use *make*

# Execution of the program:

* Launch the simulation
	* Execute ./perf.rb

* Change the number of processes 
	* Open perf.rb
	* Replace the first argument of the simulate function call by another value

* Change the number of atoms per process
	* Open simulation.c
	* Replace the value of #define NB_PARTICLES by the value of your choice
	* Open perf.rb
	* Replace the second argument of the simulate function call by NB_PARTICLES

	* The values of NB_PARTICLES and the argument of simulate() MUST be EQUAL

* Change the initial pattern
	* Open perf.rb
	* Replace the third argument of the simulate() call
		* "sun" creates satellites gravitating around a sun
		* Any other value inputs random particles in the field with nil speed 