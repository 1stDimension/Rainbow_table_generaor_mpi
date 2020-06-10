#!/bin/sh

time mpiexec -n 1 bin/rb.out > /dev/null;
time mpiexec -n 2 bin/rb.out > /dev/null;
time mpiexec -n 3 bin/rb.out > /dev/null;
time mpiexec -n 4 bin/rb.out > /dev/null;
