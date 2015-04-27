#!/usr/bin/perl -w
use strict;
use Getopt::Long;
use warnings;

my $db_name = "";
my $db_prob = "";
my $db_prob_tmp = "";
my $prob_tmp_flag = 0;
my %prob_tmp_list;
my $debug = 0;
my $help = 0;

Getopt::Long::GetOptions (
	'db-count=s' =>	\$db_name,
	'db-prob=s' => \$db_prob,
	'db-prob-tmp=s' => \$db_prob_tmp,
	'debug' => \$debug,
	'h' => \$help
);

if($help == 1)
{
	&usage();
	exit 0;
}

if( ($db_name eq "") || ($db_prob eq "") )
{
	&usage();
	die "No database name!!\n";
}

if($db_prob_tmp ne "")
{
	$prob_tmp_flag = 1;
}

##########main function start##########

my %list;
my $total_count;
my $score;

dbmopen( my %data_list, $db_name, 0644 );
dbmopen( my %prob_list, $db_prob, 0644 );

if($prob_tmp_flag != 0)
{
	dbmopen( %prob_tmp_list, $db_prob_tmp, 0644 );
}

$total_count = 0;
foreach my $key (keys %data_list)
{	
	#copy the db_expected_count
	$list{$key} = $data_list{$key};

	#$list{$key} is a string, such as "-9.035\0"
	#remove the '\0' by chop.
	chop $list{$key};

	#evaluate total_count
	$list{$key} = exp($list{$key});
	if($debug)
	{
		print "$list{$key}\t\t$key\n";
	}
	$total_count += $list{$key};
}
dbmclose(%data_list);

if($debug)
{
	print "\nTotal Count: $total_count\n\n";
	print "Prob\t\t\tln_prob\t\t\twords\n";
}

foreach my $key (keys %list)
{
	#flin modify
	$score = $list{$key} / $total_count;
	$prob_list{$key} = log($score);#update prob
	#$prob_list{$key} = $score;#update prob
	if($prob_tmp_flag != 0)#copy prob db to prob tmp db
	{
		$prob_tmp_list{$key} = $prob_list{$key};
	}
	if($debug)
	{
		print "$score\t$prob_list{$key}\t$key\n";
	}
}

if($debug)
{
	print "\n";
}

dbmclose(%prob_list);
if($prob_tmp_flag != 0)
{
	dbmclose(%prob_tmp_list);
}

##########main function end##########

sub usage
{
	my $prog =  __FILE__;
	print "\n";
	print " Usage:  perl $prog [-h] [-debug] [-db-count db_name] [-db-prob db_name] [-db-prob-tmp db_name]\n";
	print " \t(-roleno-2010-0520-v1.Maximization)\n";
	print " \t-db-count db_name: read expected counts\n";
	print " \t-db-prob db_name: write word prob.\n";
	print " \t-db-prob-tmp db_name: write word prob as same as -db-prob\n\n";
}
