#!/bin/bash

for file in exemples/*.txt
do
    result=$(./ndet -isdet "$file" | grep -oE "[^ ]+$")
    echo "$file : $result"
done
