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
my $debug = 0;
my $help = 0;

Getopt::Long::GetOptions (
	'i=s' => \$db_name,
	'f=s' => \$filename,
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
	$data[$row] = &trim($line);
	++$row;
}
close(IN);

for($i=0; $i<$row; ++$i)
{
	&argmax($data[$i], \%data_list);
}
dbmclose(%data_list);

##########main function end##########

sub argmax
{
	my ($line , $db_ref) = @_;
	my $score;
	my $i;

	#flin modify
	my @toks = split (/\s+/ , $line);

	$score = 0;
	for($i=0; $i<@toks;  ++$i)
	{
		$toks[$i] =~ s/$delimiter/ /g;
		$score += &getWordProb($toks[$i]);#$score += &getWordProb($toks[$i] , $db_ref);
	}
	print "$score" . "\n";
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
	print " Usage:  perl $prog [-h]  [-f filename] [-i db_name] [-d]\n";
	print " \t(-roleno-2010-0520-v1.Segmentation.Score.Product)\n\n";
}
