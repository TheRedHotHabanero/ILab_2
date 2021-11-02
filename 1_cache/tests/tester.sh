#! /bin/bash

files=./*.dat
status=true

for fname in $files
do
  ../build/CachePr < $fname > $fname.res
  bname=`basename $fname .dat`

  diff $fname.res $bname.ans
  if (($? != 0))
  then
    echo " ------------------------------------- "
    echo "❘           FAILED $fname""        ❘"
    echo " ------------------------------------- "
    status=false
  fi

  rm $fname.res
done

if $status
then
  echo " ------------------------------------- "
  echo "❘               PASSED               ❘"
  echo " ------------------------------------- "
fi
