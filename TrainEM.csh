#! /bin/csh -f

####### parameter setting #######
# setting the program options
set cmdOpts = "hi:f:n:t:l:"
# settinh the program name
set prog = `basename $0`

set Usage = ( \
	"Usage: $prog [-h] (-roleno-2010-0520-v1.initial.word.prob)" \
	"[-i filename] [-f db_name] [-n number] [-t threshold] [-l max phrase length]" \
	"" \
)

set usgOpts = ( \
	"-h help : show this help message" \
	"-i filename: input file" \
	"-f db_name: output db" \
	"-n number: training times" \
	"-t threshold: init prob gate" \
	"-l max phrase length : max number of word in phrase" \
)

# man getopt
@ dbxlvl = 0;

set args = (`getopt $cmdOpts $*`)

#argument initial
set filename = ''
set fflag = 0
set db_name = ''
set dflag = 0
set number = 2
set threshold = 5
set MaxPhraseLen = 4		#Default Max Phrase Length

####### arugment setting #######
while ( "$args[1]" != "--" )
	switch ($args[1])
		case -h :
			if ( $dbxlvl >= 9 ) then
				echo "$#usgOpts"
				echo "$usgOpts"
			endif

			# print help/usage messages
			echo "$Usage[1]"
			@ iUsg = 2
			while ( $iUsg <= $#Usage )
				echo "	$Usage[$iUsg]"
				@ iUsg ++
			end

			@ iOpt = 1
			while ( $iOpt <= $#usgOpts )
				echo "	$usgOpts[$iOpt]"
				@ iOpt ++
			end

			shift args
			echo "Help message"
			exit 1
			breaksw
		case -i :
			set filename = $args[2] ; shift args ; shift args
			@ fflag = 1
			breaksw
		case -f :
			set db_name = $args[2] ; shift args ; shift args
			@ dflag = 1
			breaksw
		case -n :
			set number = $args[2] ; shift args ; shift args
			breaksw
		case -t :
			set threshold = $args[2] ; shift args ; shift args
			breaksw
		case -l :
			set MaxPhraseLen = $args[2] ; shift args ; shift args
			breaksw
		case default :
			breaksw
	endsw
end

####### Main Program #######
if (! $fflag ) then
	echo "No Input Data."
	exit -1
endif

if (! $dflag ) then
	echo "No db_name."
	exit -1
endif

# Check variables is well setting
#
set db_count = $db_name"_count"
set n_txt = `mktemp`
set counts = `mktemp`

echo "Generate n-gram"
set n = 1
while ($n <= $MaxPhraseLen)
	perl GetNgram.pl -f $filename -n $n >> $n_txt
	@ n++
end
cat $n_txt > $db_name"_n_txt"#copy temp file to $db_name"_n_txt"

perl CountNgram.pl -i $n_txt -t $threshold > $counts
cat $counts > $db_name"_counts"#copy temp file to $db_name"_counts"

echo "Build DBM"
perl BuildDbm.pl -input-file $counts -db-name $db_name -delimiter "\t"

echo "Start EM"
set i = 1
set tmp_db_count = ""
while ($i <= $number)
	set tmp_db_count = $db_count"."$i
	set tmp_db	 = $db_name"."$i

	echo "NO."$i" EM"
	./EM -f $filename -d $db_name -c $tmp_db_count -l $MaxPhraseLen > $db_name".debug."$i
	perl reEstimation.pl -db-count $tmp_db_count -db-prob $db_name -db-prob-tmp $tmp_db
	perl likelihood.pl -f $tmp_db_count -i $tmp_db > $db_name".LL."$i
	perl Segmentation.pl -f $filename  -i $tmp_db -l $MaxPhraseLen > $db_name".seg."$i
	perl score.pl -f  $db_name".seg."$i -i $tmp_db > $db_name".score."$i
	@ i++
end

rm $n_txt $counts
