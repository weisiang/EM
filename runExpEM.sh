#!/bin/bash

#$1 = start max phrase length , $2 = final max phrase length , $3 = cleared corpus filename , $4 = corpus language (ex: en,fr)
#$5 = EM loop times , $6 = threshold
let t=$2-$1+1

echo "Exp total times : $t"

for (( i=$1 ; i<=$2 ; i++ ))
do
	#echo "i = $1 , m = $2 , t = $t"
	./TrainEM.csh -i "$3.$4" -f "EM.rlt/db-$4.n$5.t$6.l$i" -n $5 -t $6 -l $i 1> "EM.rlt/EM.out-$4.n$5.t$6.l$i" 2> "EM.rlt/EM.err-$4.n$5.t$6.l$i" &
done
