#!/bin/sh

export TIMEFMT=$'\n================\nCPU\t%P\nuser\t%*U\nsystem\t%*S\ntotal\t%*E'
time mpiexec -n 1 bin/rb.out 2> time_trials.txt
time mpiexec -n 2 bin/rb.out 2>> time_trials.txt
time mpiexec -n 3 --oversubscribe bin/rb.out 2>> time_trials.txt
time mpiexec -n 4 --oversubscribe bin/rb.out 2>> time_trials.txt
