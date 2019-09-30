#!/bin/bash

IFS=$'\n'
folder=`realpath $1`

find $folder -type f -size +$2c -size -$3c -print 2>/tmp/error.err
find $folder -type f -size +$2c -size -$3c -exec du -b {} \; | awk 'BEGIN{total=0}{total=total+$1}END{print total}' 2>/tmp/error.err
find $folder -type f -size +$2c -size -$3c | wc -l 2>/tmp/error.err

for line in $(cat /tmp/error.err)
do
	echo "$(basename $0): $line" >&2
done