#!/bin/bash

while getopts 'v' c
do
  case $c in
    v) VERBOSE=-v ;;
  esac
done

shift $(($OPTIND - 1))

if [ $# != 1 ]; then
  echo "Usage: $0 prn-file"
  exit 1
fi
file=$1

if test $VERBOSE; then
  set -x
else
  # suppress segmentation fault output
  exec 2> /dev/null
fi

python/rtl2svg.py -c $file $file.svg
svg2pdf $file.svg $file.pdf
pdftoppm -q -r 36 -mono $file.pdf tmpfile 2>/dev/null
# Segmentation fault - it has happened
if [ $? -eq 139 ]; then
  exit 1
fi
mv tmpfile-000001.pbm $file.pbm