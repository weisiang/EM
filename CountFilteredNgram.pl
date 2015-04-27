#!/usr/bin/perl -w
use strict;
use Getopt::Long;
use warnings;

my $filename = '';
my $threshold = 5;
my $help = 0;

Getopt::Long::GetOptions (
	'i=s' => \$filename,
	't=s' => \$threshold,
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

my %data_list;
my @strarr;

open (IN, $filename) || die "$!\n";

#count all phrase.
while(my $line = <IN>)
{
	chomp $line;
	$line = &trim($line);

	if(defined($data_list{$line}))
	{
		++$data_list{$line};
	}else{
		$data_list{$line} = 1;
	}
}
close(IN);

foreach my $key (keys %data_list)
{
	@strarr = split(/\s+/, $key);
	
	if(!($data_list{$key} < $threshold && $data_list{$key} > 2 && @strarr != 1))
	{
		delete $data_list{$key};
	}
}

#print all filtered and sorted phrase
foreach my $key ( sort { $data_list{$b} <=> $data_list{$a} } keys %data_list)
{
	print "$data_list{$key}\t$key\n";
}

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
	print " Usage:  perl $prog [-h]  [-i filename] [-t threshold value]\n";
	print " \t(flin write 2011.05.10 CountFilteredNgram.pl)\n";
	print " \tInput:\tn-gram phrases (the result of GetNgram.pl)\n";
	print " \tOutput:\tword, prob\n\n";
}
