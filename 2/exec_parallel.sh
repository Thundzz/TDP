#!/bin/bash

mpiexec -np 2 ./simulation.out; 
gnuplot plotparallel.plt;
eog particles.gif;