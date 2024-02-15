#!/bin/bash

PORT="5001"

# Define the URL to test
URL="127.0.0.1"

# Perform the GET request using curl and store the response in a variable
RESPONSE=$(curl --silent "${URL}:${PORT}")

# Read the contents of the text file into a variable
EXPECTEDFILE=expected_responce
TEMPFILE=temp.test

./server ${PORT} > /dev/null &
PID=$!

curl --silent "${URL}:${PORT}" > ${TEMPFILE}

# Check if the response matches the expected content
DIFF=$(diff ${TEMPFILE} ${EXPECTEDFILE})

if [[ -z ${DIFF} ]]; then
    echo "Test PASSED"
    rm ${TEMPFILE}
    kill ${PID}
else
    echo "Test FAILED"
    echo "DIFF: " ${DIFF}
    # rm ${TEMPFILE}
    kill ${PID}
fi