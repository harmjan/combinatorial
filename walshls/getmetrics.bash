#!/bin/bash

# 1: folder of Instances
# 2: exe path

function mysort { for i in ${res[@]}; do echo "$i"; done | sort -n; }

for file in $(find $1 -type f -iname '*.prob')
do
echo $file

read -a arr23 <<< "$(echo $file | tr "_" " ")"

for index in "${!arr23[@]}"
do
if [ $(echo ${arr23[index]} | grep '^k$') ]
then 
n=${arr23[index-1]}
k=${arr23[index+1]}
a=$(echo ${arr23[index+3]} | tr -d '.prob')
fi
done

for i in $(seq 1 5) #for average results
do
for j in $(seq 1 3)
do
time1=$(date +%s.%N)
./$2 < $file
time2=$(date +%s.%N)
res[$j]=$(echo $time2 - $time1 | bc)
done
res=( $(mysort) )   #take the middle result of the three due to possible os delays
printf "%d %d %d %.6f %s\n" "$n" "$k" "$a" "${res[1]}" "$file" >> allresults
done

done



#take averages

awk 'BEGIN{currentfile=""}
{
if( currentfile != $5 ) {
   if (currentfile != "")
   printf("%d %d %d %.6f %s\n",n,k,a,sum/count,currentfile);
   n=$1;
   k=$2;
   a=$3;
   currentfile=$5;
   sum=0;
   count=0; 
}
else{
sum += $4;
count++;
}}
END{printf("%d %d %d %.6f %s\n",n,k,a,sum/count,currentfile);}' allresults > avgresults









