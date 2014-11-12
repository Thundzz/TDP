#!/bin/bash

mpiexec ./main.out; 
make plot;
eog particles.gif;