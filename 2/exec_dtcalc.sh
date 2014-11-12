#!/bin/bash

mpiexec ./dtcalc_test.out; 
make plot;
eog particles.gif;