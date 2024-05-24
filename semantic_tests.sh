#!/bin/bash

OUT_EXEC="$1"
SEMANTIC_TEST_DIR="./semantic_tests"

echo "Running semantic tests..."
for test_file in ${SEMANTIC_TEST_DIR}/*.c; do
    if [ "$test_file" != "${SEMANTIC_TEST_DIR}/main.c" ]; then
        echo -e "\n\n\nRunning $test_file"
        ./${OUT_EXEC} < $test_file
        if [ $? -ne 0 ]; then
            echo "Semantic Test FAILED with error on $test_file"
        else
            echo "Semantic Test PASSED for $test_file"
        fi
    else
        echo -e "\n\n\nSkipping $test_file"
    fi
done
