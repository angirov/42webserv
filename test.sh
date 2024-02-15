#!/bin/bash

PORT="5001"

# Define the URL to test
URL="127.0.0.1"

# Perform the GET request using curl and store the response in a variable
RESPONSE=$(curl --silent "${URL}:${PORT}")

# Read the contents of the text file into a variable
EXPECTEDFILE=expected_responce
TEMPFILE_1=temp_1.test
TEMPFILE_2=temp_2.test

./server ${PORT} > /dev/null &
PID=$!

curl --silent "${URL}:${PORT}" > ${TEMPFILE_1}
curl --silent "${URL}:${PORT}" > ${TEMPFILE_2}

# Check if the response matches the expected content
DIFF_1=$(diff ${TEMPFILE_1} ${EXPECTEDFILE})
DIFF_2=$(diff ${TEMPFILE_2} ${EXPECTEDFILE})

if [[ -z ${DIFF_1} && -z ${DIFF_2} ]]; then
    echo "Test PASSED"
    sleep 5
    rm ${TEMPFILE_1} ${TEMPFILE_2}
    kill ${PID}
else
    echo "Test FAILED"
    echo "DIFF_1: " ${DIFF_1}
    echo "DIFF_2: " ${DIFF_2}
    rm ${TEMPFILE_1} ${TEMPFILE_2}
    kill ${PID}
fi