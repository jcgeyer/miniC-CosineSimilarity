#!/bin/bash

OUT_EXEC="$1"
PARSER_TEST_DIR="./builder_tests"

echo "Running builder tests..."
for test_file in ${PARSER_TEST_DIR}/*.c; do
    if [ "$test_file" != "${PARSER_TEST_DIR}/main.c" ]; then
        echo -e "\n\n\nRunning $test_file"
        ./${OUT_EXEC} < $test_file
    else
        echo -e "\n\n\nSkipping $test_file"
    fi
done
