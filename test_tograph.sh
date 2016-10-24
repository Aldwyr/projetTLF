#!/bin/bash

for file in exemples/*.txt
do
    ./ndet -nop "$file" test -g
    result=$(diff test.gv "${file:0:-4}.gv")
    if [ "$result" == "" ]
    then echo "$file : OK"
    else echo "$file : NOT OK"
    fi
done
