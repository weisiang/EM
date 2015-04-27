use strict;
use Getopt::Long;
use warnings;

#####
# Getoption Settings
# 
#
my $db_name = "";
my $filename = "";
my $delimiter = "_SEG_DEL_";
my $whitespace = "_SEG_WSPACE_";
my $SegLen = 4;
my $debug = 0;
my $help = 0;

Getopt::Long::GetOptions (
	'f=s' => \$filename,
	'i=s' => \$db_name,
	'd=s' => \$delimiter,
	'w=s' => \$whitespace,
	'l=s' => \$SegLen,
	'debug' => \$debug,
	'h' => \$help
);

if($help == 1)
{
	&usage();
	exit 0;
}

if($filename eq "")
{
  &usage();
  die "No file name!!\n";
}

if($db_name eq "")
{
	&usage();
	die "No database name!!\n";
}

##########main function start##########

my @data;
my $row;
my $i;

open (IN, $filename) || die "$!\n";
dbmopen( my %data_list, $db_name, 0644 );

$row = 0;
while(my $line = <IN>)
{
	chomp $line;
        #flin add
	$data[$row] = &trim($line);
	++$row;
}
close(IN);

for($i=0; $i<$row; ++$i)
{
	&argmax($data[$i], \%data_list, $SegLen);
}
dbmclose(%data_list);

##########main function end##########

sub argmax
{
	my ($line , $db_ref , $SegLen) = @_;
	my @delta;
	my @backTracing;
	my $word_prob;
	my $tmpscore;
	my $max;
	my $argmax;
	my $index;
	my $buff;
	my $i;
	my $j;

	#flin modify

	my @toks = split (/\s+/ , $line);

	##
	##init
	##
	$delta[0] = 0;
	$backTracing[0] = 0;
	for($i=1 ; $i<=@toks ; ++$i)
	{
		$max = -100000000000;
		for($j=$i ; $j>0 ; --$j)
		{
			if(($i-$j) < $SegLen)#flin modify
			{
				#print "($j, $i) = " . &getString(\@toks, $j, $i) . "\n";
				$word_prob = &getWordProb(&getString(\@toks, $j, $i));#$word_prob = &getWordProb(&getString(\@toks, $j, $i) , $db_ref);
				$tmpscore = $delta[$j-1] + $word_prob;
				
				if($debug)
				{
					print "$tmpscore = $delta[$j-1] D(" . ($j-1) . ") + $word_prob (" . &getString(\@toks, $j, $i) . ")\n";
				}
				
				if($tmpscore > $max)
				{
					$max = $tmpscore;
					$argmax = $j-1;
				}
				
				if($debug)
				{
					print "Delta_$i:$max : back to $argmax\n";
				}
			}
			
			$delta[$i] = $max;
			$backTracing[$i] = $argmax;
		}
	}

	#if($debug)
	#{
	#	print "$line\n";
	#	for (my $i=0; $i<=@toks; ++$i)
	#	{
	#		print "Delta_$i:$delta[$i] : back to $backTracing[$i]\n";
	#	}
	#}

	$index = @toks;
	$buff = "";
	while($index != 0)
	{
		$buff = &getString(\@toks, $backTracing[$index]+1, $index) . $whitespace . $buff;
		#print "($backTracing[$index], $index) = " . $buff . "\n";
		$index = $backTracing[$index];
	}

	#flin modify
	$buff =~ s/\s+/$delimiter/g;#" " -> $delimiter
	$buff =~ s/$whitespace/ /g;#$whitespace -> " "

	chop $buff;

	print "$buff\n";
}

sub getString
{
	my ($toks_ref, $i, $j) = @_;
	my $buff = "";
	--$i;
	--$j;

	for(;$i<=$j;++$i)
	{
		$buff .= $toks_ref->[$i] . " ";
	}
	chop $buff;
	
	return $buff;
}

sub getWordProb
{
	my ($word) = @_;#my ($word , $data_list) = @_;
	my $word_prob = 0;

	if(defined($data_list{$word}))
	{
		$word_prob = $data_list{$word};
	}else{
		my @tok = split (/\s+/ , $word);
		$word_prob = -10000 * @tok;
	}
	return $word_prob;
}

# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
        my $string = shift;
        $string =~ s/^\s+//;
        $string =~ s/\s+$//;
        return $string;
}

sub usage
{
	my $prog =  __FILE__;
	print "\n";
	print " Usage:  perl $prog [-h] [-debug] [-f filename] [-i db_name] [-d] [-w] \n";
	print " \t(-roleno-2010-0520-v1.Segmentation)\n\n";
}
