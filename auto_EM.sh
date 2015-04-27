#this is a program for auto segmente.
#create at 2015/3/11 ---weisiang
#!/bin/bash -
read -e -p "key in the corpus's path : " corpusPath
read -e -p "key in the language : " language
read -e -p "key in the number of training data : " amount
read -e -p "key in the segment length Start : " lengthStart
read -e -p "key in the segment length End : " lengthEnd
read -e -p "key in the threshold srart : " thresholdStart
read -e -p "key in the threshold end : " thresholdEnd
#lengthStart#######################

for((j=$thresholdStart;j!=$thresholdEnd+1;j++))
do
	for((i=$lengthStart;i!=$lengthEnd+1;i++))
	do
		touch tempLog
		echo 'i='"$i" >> tempLog
		echo 'j='"$j" >> tempLog
		mkdir $amount
		mkdir -p $amount.$language.threshold$j/length_$i 
		folder="$amount.$language.threshold$j/length_$i"
		touch $folder/log
		EM_Start=`date +%s`
		/home/nlp/EM_Segment_sum/TrainEM.csh -i $corpusPath -f $amount/$amount -n 5 -t $j -l $i
		EM_End=`date +%s`
		timeDiff=$(($EM_End-$EM_Start))
		echo "EM.length.$i = $timeDiff" > $folder/log
		/home/nlp/EM_Segment_sum/showDb.sh $amount/$amount.5 > dbm.en
		sed 's/\t//;1,2d' dbm.en |awk -F"\t" '{print $2"\t"$1}' > dbm.en-ok
		rm dbm.en 
		mv dbm.en-ok dbm.en
		mv dbm.en $amount/dbm.en.$i
		mv $amount/* $folder	
		rm -rf $amount
	done
done
