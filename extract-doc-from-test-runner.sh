#!/bin/bash

# extract the strategic lines of a test runner (`Test(...)` lines and `/// ...` lines)
# and transform them to generate a human readable specification of the behavioural tests.

if [ $# -lt 1 ]; then
  echo "Usage: extract-doc-from-test-runner.sh <path_to_test_runner_source>"
  echo
  echo "e.g.: extract-doc-from-test-runner.sh cmnlib-term/src-tests/vt-input/TestRunner--VtInputSource.cpp"
  exit -1
fi

echo "> _BEGIN extracted from \`$(basename "$1")\`_"
grep -h "\(^Test\)\|\(/// [_>]\)" "$1" | sed -E -e "s,^[ ]*/// ,\n," | sed -E -e "s,^Test[(],\n<hr>\n\n### ," | sed -E -e "s,[)] [{]$, ###,"
echo -e "\n<hr>\n"
echo "> _END extracted from \`$(basename "$1")\`_"

