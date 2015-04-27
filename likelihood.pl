use strict;
use Getopt::Long;
use warnings;

#####
# Getoption Settings
# 
#
my $db_name = "";
my $filename = "";
my $debug = 0;
my $help = 0;

Getopt::Long::GetOptions (
	'f=s' => \$filename,
	'i=s' => \$db_name,
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

my $likelihood;
my $score;
my $counts;

dbmopen( my %data_list, $db_name, 0644 );
dbmopen( my %count_list, $filename, 0644);

$likelihood = 0;
foreach my $key (keys %data_list)
{
	$counts = $count_list{$key};
	chop $counts;#remove "string\0"
	
	$score = exp($counts) * $data_list{$key};
	$likelihood += $score;
}

dbmclose(%count_list);
dbmclose(%data_list);

print "LL Score: " . $likelihood . "\n";

##########main function end##########

sub usage
{
	my $prog =  __FILE__;
	print "\n";
	print " Usage:  perl $prog [-h] [-debug] [-f db_count] [-i db_name] [-d]\n";
	print " \t(-roleno-2010-0520-v1.Segmentation.likelihood)\n\n";
}
