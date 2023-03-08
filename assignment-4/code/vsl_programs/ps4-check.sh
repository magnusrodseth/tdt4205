#!/bin/bash

cd ps4-symbols;

# Compare all *.symbols with the corresponding file in ./suggested/*.symbols
for file in *.symbols; do
    echo "Checking '$file' against 'suggested/$file'";
    # Use `diff` to check for differences, and only print if no differences are found
    diff -s $file suggested/$file;

    # If the exit code is 0, then the files are the same
    if [ $? -eq 0 ]; then
        echo "No differences found";
    fi
done

if [ -d ../ps4-symbols ]; then
    echo "Removing old ps4-symbols directory";
    rm -rf ../ps4-symbols;
fi