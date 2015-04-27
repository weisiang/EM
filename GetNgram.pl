#!/usr/bin/perl -w
use strict;
use Getopt::Long;
use warnings;

my $filename = '';
my $output = "";
my $n_gram = 1;
my $help = 0;

Getopt::Long::GetOptions (
	'f=s' => \$filename,
	'n=s' => \$n_gram,
	'h' => \$help
);

if($help == 1)
{
	&usage();
	exit 0;
}

if($filename eq "")
{
	die "No file data!!\n";
}

##########main function start##########

my $N;
my $i;

open (IN, $filename) || die "$!\n";

while(my $line = <IN>)
{
	chomp $line;
	#flin modify
	$line = &trim($line);
	my @tokens = split (/\s+/ , $line);

	#This method copy GetNgram in WS
	#Rewrite to perl form
	for($i=0 ; $i <= $#tokens ; $i++)
	{
		$output = "";
		
		for($N=0 ; ($N < $n_gram) && ($i+$N < @tokens) ; $N++)
		{
			$output .= $tokens[$i+$N] . " ";
		}
		
		if($N == $n_gram)
		{
			chop $output;#remove last char " "
			print "$output\n";
		}
	}
}
close(IN);

##########main function end##########

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
	print " Usage:  perl $prog [-h]  [-f filename] [-n n-gram]\n";
	print " \t(-roleno-2010-0519-v1.GetNgram)\n";
	print " \tInput:\tcorpus\n";
	print " \tOutput:\twords(n-gram)\n\n";
}
