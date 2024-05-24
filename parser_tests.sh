#!/bin/bash

OUT_EXEC="$1"
PARSER_TEST_DIR="./parser_tests"

echo "Running parser tests..."
for test_file in ${PARSER_TEST_DIR}/*.c; do
    if [ "$test_file" != "${PARSER_TEST_DIR}/main.c" ]; then
        echo -e "\n\n\nRunning $test_file"
        ./${OUT_EXEC} < $test_file
        if [ $? -ne 0 ]; then
            echo "Parser Test FAILED with error on $test_file"
        else
            echo "Parser Test PASSED for $test_file"
        fi
    else
        echo -e "\n\n\nSkipping $test_file"
    fi
done
