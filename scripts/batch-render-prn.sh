#!/bin/bash

echo "Running tests"
srcdir=test-data/corel
for f in $srcdir/*.prn; do
  testcase=`basename $f .prn`
  echo "*$testcase...."
  scripts/prn-to-pdf.sh $f
done