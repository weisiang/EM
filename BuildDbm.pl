#! /usr/bin/perl
use Getopt::Long;
use strict;
use warnings;

my $db_name="";
my $input_file="";
my $delimiter="\t";
my $readonly=0;
my $help=0;
my $print=0;

GetOptions(
	"input-file=s" => \$input_file,
	"db-name=s" => \$db_name,
	"delimiter=s" => \$delimiter,
	"ro|r" => \$readonly,
	"h|H|help|HELP" => \$help,
	"p" => \$print
);

if($help == 1)
{
	&usage();
	exit 0;
}

if($db_name eq "")
{
	die "No database name!!\n";
}

if($readonly == 1)
{
	print "Read only mode ...\n";
	&readdata($db_name);
	exit 0;
}

if($input_file eq "")
{
	die "No input file!!\n";
}

##########main function start##########

#print "Building database \"$db_name\" from file \"$input_file\" ...\n";#flin

open(FHD, $input_file) || die "$!\n"; # $!the current system error message from errno
dbmopen( my %data_list, $db_name, 0644 ); 

while(my $line=<FHD>)
{
	#flin modify
	chomp $line;
	$line = &trim($line);
	my @data = split(/$delimiter/ , $line);
	
	if($#data+1 != 2)
	{
		#flin modify
		#print $#data,"\n",@data,"\n";
		#print "File format error!!\n";
		dbmclose(%data_list);
		close(FHD);
		die "File format error!!\n";
	}
	
	#flin modify
	$data_list{$data[0]} = $data[1];
}

dbmclose(%data_list);
close(FHD);

#flin modify
#print "Database \"$db_name\" built completely.\n\n";
print "Database build Success!!\n";
if($print == 1)
{
	print "Printing the content of database \"$db_name\" ...\n";
	&readdata($db_name);
}
	
##########main function end##########	

sub readdata
{
	my $dbname = $_[0];

	dbmopen( my %db, $dbname, 0444 );

	print "Database: $db_name\n";
	foreach my $key (keys %db)
	{
		print "$db{$key}\t\t$key\n";
	}
	dbmclose(%db);
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
	print " \tUsage:  perl $prog [-input-file file] [-db-name db] [-ro|-r] [-delimiter delimiter] [-p]\n";
	print " \tFunction: Create a DB file from a text table (2009-06-15)\n";
	print " \t-input-file\t要建成資料庫的檔案\n";
	print " \t-db-name\t資料庫名稱\n";
	print " \t-delimiter\t分隔符號(Default:\\t)\n";
	print " \t-p\t\t列印資料庫建立之後的結果(預設不列印)\n";
	print " \t-ro|-r\t\t唯讀\n\n";
}
