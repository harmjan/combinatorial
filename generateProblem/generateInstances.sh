#!/bin/bash/

# 1: folder of Instances

mkdir $1

#make Instances with different values of N and K (a is fixed for now)
for n in $(seq 0 50 1000); #n
 do
     for k in $(seq 1 10)
	do
	if [ $n -ge $k ];
	then
	./generate $n $k > `echo  $1/"n_"$n"_k_"$k"_a_5".prob`
	else
		echo "no file for n:" $n " and k:" $k	
	fi
     done
done


