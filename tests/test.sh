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
  curl --cookie-jar - -X $1  -w "\nstatus: %{http_code}" http://localhost:3000$2 2>&1 >> $TEST_DATA
}

run_test() {
  print_test_title "$1"
  print_test_url "$2" "$3" "$4"
}

# Run test server in background and save PID
cd ..
node lib/js/example/Index.js &
TEST_SERVER_PID=$!
cd tests

# Ugly hack to wait for the server to start
sleep 2s

clean_previous_test;

run_test 'Root path' 'GET' '/'
run_test 'Invalid path' 'GET' '/invalid-path'
run_test 'Static middleware' 'GET' '/static/test.data'
run_test 'POST + Query param (valid)' 'POST' '/123/id'
run_test 'POST + Query param (invalid)' 'POST' '/999/id'
run_test 'PUT + Query param (valid)' 'PUT' '/123/id'
run_test 'PUT + Query param (invalid)' 'PUT' '/999/id'
run_test 'PUT + Query param (valid)' 'PATCH' '/123/id'
run_test 'PUT + Query param (invalid)' 'PATCH' '/999/id'
run_test 'DELETE + Query param (valid)' 'DELETE' '/123/id'
run_test 'DELETE + Query param (invalid)' 'DELETE' '/999/id'
run_test 'baseUrl property' 'GET' '/baseUrl'
run_test 'hostname property' 'GET' '/hostname'
# run_test 'ip property' 'GET' '/ip'
run_test 'method property' 'GET' '/method'
run_test 'method originalUrl' 'GET' '/originalUrl'
run_test 'method path' 'GET' '/path'
run_test 'method path' 'GET' '/protocol'
run_test 'Query parameters' 'GET' '/query?key=value'
run_test 'Fresh' 'GET' '/fresh'
run_test 'Stale' 'GET' '/stale'
run_test 'Secure' 'GET' '/secure'
run_test 'XHR' 'GET' '/xhr'
run_test 'Redirect' 'GET' '/redir'
run_test 'Redirect with Code' 'GET' '/redircode'
run_test 'Non 200 Http status' 'GET' '/not-found'
run_test 'Non 200 Http status' 'GET' '/error'
run_test 'Promise Middleware' 'GET' '/promise'
run_test 'Failing Promise Middleware' 'GET' '/failing-promise'
run_test 'Can catch Ocaml Exception' 'GET' '/ocaml-exception'
run_test 'Can use express router' 'GET' '/testing/testing/123'
run_test 'Can specify that a router behaves in a case sensitive manner' 'GET' '/router-options/case-sensitive'
run_test 'Can specify that a router behaves in a case sensitive manner' 'GET' '/router-options/Case-sensitive'
run_test 'Can specify that a router behaves in a strict manner' 'GET' '/router-options/strict'
run_test 'Can specify that a router behaves in a strict manner' 'GET' '/router-options/strict/'
run_test 'Can bind middleware to a particular param' 'GET' '/param-test/123'
run_test 'Can set cookies' 'GET' '/cookie-set-test'

run_cookie_clear_test() {
  print_test_title "$1"
  curl -i -X $2  -w "\nstatus: %{http_code}\n" http://localhost:3000$3 2>&1 | grep -Fi Set-Cookie >> $TEST_DATA
  # curl -X $2  -w "\nstatus: %{http_code}\n" http://localhost:3000$3 2>&1 >> $TEST_DATA
}

run_cookie_clear_test 'Can clear cookies' 'GET' '/cookie-clear-test'


run_json_test() {
  print_test_title "$1"
  curl  -X POST -H "Content-Type: application/json" -w "\nstatus: %{http_code}" -d "$3" http://localhost:3000$2 2>&1 >> $TEST_DATA
}

run_json_test 'Can accept JSON using builtin middleware' '/builtin-middleware/json-doubler' '{ "number": 4 }'

run_urlencoded_test() {
  print_test_title "$1"
  curl  -X POST -H "Content-Type: application/x-www-form-urlencoded" -w "\nstatus: %{http_code}" -d "$3" http://localhost:3000$2 2>&1 >> $TEST_DATA
}

run_urlencoded_test 'Can accept UrlEncoded body using builtin middleware' '/builtin-middleware/urlencoded-doubler' 'number=4'

run_header_test() {
  print_test_title "$1"
  curl -X "$2" -H "$3" http://localhost:3000$4 2>&1 >> $TEST_DATA
}

run_header_test 'Accepts' 'GET' 'Accept: audio/*; q=0.2, audio/basic' \
  '/accepts'

run_header_test 'Accepts Charsets' 'GET' 'Accept-Charset: UTF-8' \
  '/accepts-charsets'

run_header_test 'Get' 'GET' 'key: value' \
  '/get'

run_text_test() {
  print_test_title "$1"
  curl -X POST -H "Content-Type: text/plain" -d "$3" http://localhost:3000$2 2>&1 >> $TEST_DATA
}

run_text_test "Can parse text using bodyparser middleware" "/router4/text-body" 'This is a test body'

run_response_header_test() {
  print_test_title "$1"
  curl -i -X $2  -w "\nstatus: %{http_code}\n" http://localhost:3000$3 2>&1 | grep -Fi X-Test-Header >> $TEST_DATA
}

run_response_header_test 'Can set response header via setHeader' 'GET' '/response-set-header'

run_text_test "Can the user user the javascipt http object directly" "/get-request-count"

# Stop server
kill $TEST_SERVER_PID

# compare test output to reference data

REFERENCE_DATA=reference.data
diff $TEST_DATA $REFERENCE_DATA
