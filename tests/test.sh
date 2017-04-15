#!/usr/bin/env bash

# Unit test to run again an Express server defined in examples/Index.re
#
# This test uses 'curl' to query the web server and output the 
# HTTP response in 'test.data'. 
#
# It then makes a diff with the expected data stored in 'reference.data'

TEST_DATA=test.data

clean_previous_test() {
  rm -f $TEST_DATA
}

function print_test_title() {
  echo >> $TEST_DATA
  echo "-- $1--" >> $TEST_DATA
}

print_test_url() {
  curl -X $1 http://localhost:3000$2 2>&1 >> test.data
}

run_test() {
  print_test_title "$1"
  print_test_url "$2" "$3"
}

clean_previous_test;

run_test 'Root path' 'GET' '/'
run_test 'Invalid path' 'GET' '/invalid-path'
run_test 'Static middleware' 'GET' '/static/test.data'
run_test 'POST + Query param (valid)' 'POST' '/123/id'
run_test 'POST + Query param (invalid)' 'POST' '/999/id'

# compare test output to reference data

REFERENCE_DATA=reference.data
diff $TEST_DATA $REFERENCE_DATA
