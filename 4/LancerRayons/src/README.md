#COMPILATION:
Run the habitual : 

```
 make
 make install
```
That will create three binaries in the bin/ folder :
*	lanceur: for the ray tracing program using static distribution with
	task stealing
*	lanceur_static: for the ray tracing program 
*	lanceur_jouet: for the toy program using static distribution with or without 
	task stealing

#EXECUTION:

* In order to run the ray tracing program, lanceur and lanceur_static: 
```
mpiexec -np <nbProcesses> ./lanceur <scn filename>
``` 

* In order to run the toy program, lanceur_jouet: 
```
mpiexec -np <nbProcesses> ./lanceur_jouet -n <nb_small_tasks> -N <nb_big_tasks> [OPTIONS]
``` 
* Different options can be set for the toy program
	* -b or --bad  				creates a bad distribution by giving all the big tasks to the processes
								that have the lowest rank, and the small tasks to the others.
								(By default, the big tasks are distributed equal among the processes)
	* -t or --time  [ARGUMENT]   sets the time in usec of the small tasks. Big tasks are 10 times longer.
								(Default 1000 us for small tasks, 10000 us for big tasks)
	* -s or --nosteal    	if this option is entered, task stealing will be disabled 
	* -h or --help        	prints the help

#BENCHMAKS
See and run benchmark.rb

* Call perf function with 3 arguments
	* argument 1 : max number of processes to run the benchmark
	* argument 2 : program name (lanceur, lanceur_static or lanceur_jouet)
	* argument 3 : .scn file name