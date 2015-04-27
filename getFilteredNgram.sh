#!/bin/bash


#$1 = init SegLen , $2 = final SegLen , $3 = corpus language (ex: en,fr)
let t=$2-$1

echo "Exp total times : $t"

for (( i=$1 ; i<=$2 ; i++ ))
do
	#echo "i = $1 , m = $2 , t = $t"
	./CountFilteredNgram.pl -i ExpResult/db-$3N$i\_n_txt -t 5 > "FilteredPhrase/$3N$i.filtered"
done
