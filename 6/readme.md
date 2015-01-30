#COMPILATION:
Run the habitual : 

```
 make
```

#EXECUTION:

```
To execute the program, run either:
* life_seq.out [nb_loops] [array_size] : Sequential version
* life_omp.out [nb_loops] [array_size] [nb_threads] : OpenMP version
* life_pthread [nb_loops] [array_size] [nb_threads] : pthreads version
* life_mpi_sync [nb_loops] [array_size] [nb_threads] : MPI with synchronous communications version
* life_mpi_async [nb_loops] [array_size] [nb_threads] : MPI with asynchronous communications version
* life_mpi_persist [nb_loops] [array_size] [nb_threads] : MPI with persistent communications version

#BENCHMAKS
See and run "benchmarks.rb"

You will find several functions to compute the tests.
* bench_shared_mem() : Weak scalability on OpenMP and pthreads
* bench_mpi() : Weak scalability on MPI

* bench_shared_mem_threads() : Strong scalability on OpenMP et pthreads
* bench_mpi_procs() : Strong scalability on MPI
Associated functions to generate a gnuplot script and compute the figures are also present