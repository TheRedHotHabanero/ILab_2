#! /bin/bash

files=./*.dat

for fname in $files
do
  ../build/CachePr < $fname > $fname.res
  bname=`basename $fname .dat`

  diff $fname.res $bname.ans
  if (($? != 0))
  then
    echo
    echo "FAILED $fname"
    echo '-------------------------------------'
  fi

  rm $fname.res
done
