This repository contains an implementation of the local searcher described in the paper by Darrell Whitley and Wenxiang Chen (<http://dl.acm.org/citation.cfm?id=2330351>). This software was written for the course Combinatorial Solvers at the TU Delft.

# Structure

There are 4 different components to this project, a problem instance generator and 3 solvers. Each component has it's own folder and Makefile. All data passed to and received from a component go via standard in/out. You can generate an instance with N=100 and K=10 and have the walshls solver solve it with:

	echo "100 10" | ./generateProblem/generate | ./walshls/walshls

Most of the problem instances that we used to generate the graphs in the report for this course are also included in this repository.

# Running a test

Most components have a few constants above in the file you can change to switch between printing the result and timing the algorithm. If you compiled the naive and walshls solver with TIMING=true you can generate a gnuplot data file by running the graphs.sh file, afterwards you can plot the results with gnuplot plot.gnu.

# Problem instance file format

```
Two integers, first N than K
The start bitstring consisting of N space separated 1's or 0's
Than N lines with:
  K ints which map the bits from total bitstring to the subbitstring
  2^K floats, the output from the subfunctions iterate from subbitstring 00..0 to 11..1
```
