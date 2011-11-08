#!/bin/bash

cd $CC_BASE

dir="$1"

[ -z "$dir" ] && dir="$CC_TEST_DATA"

function tailcases {
  findcases $dir | while read casedir; do
    casename="`basename $casedir`"
    echo "`dirname $casedir`/out/$casename/$casename.log"
  done | xargs tail -n0 -q --retry -f 
}

tailcases 2> /dev/null

