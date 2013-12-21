#!/bin/bash

# Generate a lot of instances with a low N and K so they are
# easy to check by hand, then see where it goes wrong
for n in $(seq 1 50)
do
	for k in $(seq 1 10)
	do
		if (( $k <= $n ))
		then
			echo "Comparing naive and walshls for N=$n and K=$k"
			problemFile=./problemInstances/generated/random_n_${n}_k_${k}

			# Only generate an instance if it doesn't exists already
			if [ ! -f "${problemFile}" ]
			then
				echo "$n $k" | ./generateProblem/generate > ${problemFile}
			fi

			# It's a bit bodged but this makes it easy to compare results
			./naive/naive     < ${problemFile} | grep "Local optimum" > naive_tmp
			./walshls/walshls < ${problemFile} | grep "Local optimum" > walshls_tmp
			diff naive_tmp walshls_tmp
		fi
	done
done
