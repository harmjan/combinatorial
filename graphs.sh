#!/bin/bash

echo "# N K naive-runtime naive-iterations naive-fitness walshls-preprocess-runtime walshls-iterations-runtime walshls-iterations walshls-fitness" > results.dat

# Generate a lot of instances with a wide range of N and K
# and let the programs time the results
for n in $(seq 100 100 10000)
do
	for k in $(seq 1 10)
	do
		if (( $k <= $n ))
		then
			echo "Running timing tests with naive and walshls for N=$n and K=$k"
			problemFile=./problemInstances/generated/random_n_${n}_k_${k}

			# Only generate an instance if it doesn't exists already
			if [ ! -f ${problemFile} ]
			then
				echo "$n $k" | ./generateProblem/generate > ${problemFile}
			fi

			# Append the timing results without newlines space separated to the file
			echo -n "$n $k " >> results.dat
			./naive/naive     < ${problemFile} >> results.dat
			echo -n " " >> results.dat
			./walshls/walshls < ${problemFile} >> results.dat
			echo "" >> results.dat
		fi
	done
	# This newline makes gnuplot understand the data better
	echo "" >> results.dat
done
