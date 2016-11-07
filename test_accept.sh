for file in exemples/*.txt
do
    result=$(./ndet -accept $file aba)
    echo "$file: $result"
done
