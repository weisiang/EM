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
my $total_count;
my @strarr;

open (IN, $filename) || die "$!\n";

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

#
#count all phrase.
#flin modify
#
$total_count = 0;
foreach my $key (keys %data_list)
{
	@strarr = split(/\s+/, $key);
	
	if($data_list{$key} >= $threshold || @strarr == 1)
	{
		$total_count += $data_list{$key};
	}else{
		delete $data_list{$key};
	}
}

#
#re-count the phrase.
#flin modify
#
foreach my $key (keys %data_list)
{
	$data_list{$key} = log($data_list{$key} / $total_count);
	print "$key\t$data_list{$key}\n";
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
	print " \t(-roleno-2010-0520-v1.CountNgram)\n";
	print " \tInput:\tn-gram phrases (the result of GetNgram.pl)\n";
	print " \tOutput:\tword, prob\n\n";
}
