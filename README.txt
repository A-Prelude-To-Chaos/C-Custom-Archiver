# CARCH: C Custom Archiver

THIS PROJECT COMPRESSES FILES to store information in an efficient form for sharing etc

i have used a "priority queue" (custom made)
and also a method to store bits 

Instructions
First run make
you will find an exectable named carch 

to compress files use this format
./carch c <archive_name.carch> <file1> <file2> ...

now you will find archive_name.carch which stores info of all ur files 
you may use this file for sharing efficiently and when you require the contents again
extract it using this convention
./carch x <archive_name.carch>

you will get ur files back

run make clean to decompile it.
