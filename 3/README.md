#COMPILATION:
Run the habitual : 

```
 make
```
#EXECUTION:

* In order to run the program: 
```
mpiexec -np <nbProcesses> nbProcesses ./main.out <InMat1> <InMat2> <OutMat> <nbIter>
``` 
Where:
 
* InMat1, InMat2 : filenames for the input matrixes
* OutMat: Name of the output matrix
* nbIter: The number of times the computation must be done. (This is used for our benchmarks)

#BENCHMAKS
If you want to run the benchmarks, you have to edit the ruby script
 benchmark.rb, and run it:
 
* gen_proc_cst() : Generates all the matrixes until a certain length passed as argument, and runs the multiplication with a constant number of processes.
* gen_all_proc_var() : Generates one big matrix and runs the benchmark with ascending number of processes.
* gen_all_triple_proc_cst():  Was used to generate some of the graphics used in the report.