#!/bin/bash

rm results.csv

# Generate a lot of instances with a wide range of N and K
# and let the programs time the results
for n in $(seq 100 100 1000)
do
	for k in $(seq 1 $n)
	do
		if [ $n -ge $k ];
		then
		if [ $k -le 10 ];
		then
			echo "Running timing tests with naive and walshls for N=$n and K=$k";
			problemFile=./problemInstances/generated/random_n_${n}_k_${k}
			./generateProblem/generate $n $k > ${problemFile}

			# It's a bit bodged but this makes it easy to compare results
			echo -n "$n;$k;" >> results.csv
			./naive/naive     < ${problemFile} >> results.csv
			echo -n ";" >> results.csv
			./walshls/walshls < ${problemFile} >> results.csv
			echo "" >> results.csv
		fi
		fi
	done
done

