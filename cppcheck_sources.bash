#!/bin/bash

#./clean

output="cppcheckout"
sorted_output="sorted_cppcheckout"

workingdir=`pwd`
workingdir=$(echo $workingdir | sed 's/\//\\\//g')
#echo $workingdir

time ( find . -maxdepth 2 \( -name "*.h" -o -name "*.cpp" -o -name "*.hpp" \) | tr '\n' ' ' | sed "s/.*/cppcheck --force --enable=all -v --language=c++ & 2> $output\n/" | bash )
sed -ri "s/^\[([^]]+)\]: (.*)/$workingdir\/\1: \2/" $output
sort -V $output > $sorted_output

#sort -V $output | sed -r "s/\[([^]]+)\]: (.*)/$workingdir\/\1: \2/" > $sorted_output
