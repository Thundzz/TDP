#COMPILATION:
Run the habitual : 

```
 make
 make install
```
That will create three binaries in the bin/ folder :
*	lanceur: for the ray tracing program using static distribution with
	task stealing
*	lanceur_: for the ray tracing program 
*	lanceur_jouet: for the toy program using static distribution with or without 
	task stealing

#EXECUTION:

* In order to run the ray tracing program, lanceur and lanceur_: 
```
mpiexec -np <nbProcesses> ./lanceur <scn filename>
``` 

* In order to run the toy program, lanceur_jouet: 
```
mpiexec -np <nbProcesses> ./lanceur_jouet -n <nb_tasks> [OPTIONS]
``` 
* Different options can be set for the toy program
	* -u or --uniform  sets the same time for every task on every process (enabled by default)
	* -l or --unlucky  [ARGUMENT] creates a bad distribution by extending the time of
								the tasks given to the process which index is set as the argument
	* -t or --time  [ARGUMENT]   sets the time in usec of every task if the uniform distribution is set, 
								if the unlucky distribution is set, this option sets the time
								of every tasks not given to the unlucky process
								(1000)
	* -s or --nosteal    	if this option is entered, task stealing will be disabled 
	* -h or --help        	prints the help

#BENCHMAKS
Run benchmark.rb