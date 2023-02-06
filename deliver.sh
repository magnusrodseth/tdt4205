#!/usr/local/bin/zsh
#
# This script is used to deliver assignments.
# 
# Usage: deliver.sh <assignment-directory>

STUDENT_NAME="magnrod"

# Check for the assignment directory
if [ $# -ne 1 ]; then
    echo "Usage: deliver.sh <assignment-directory>"
    exit 1
fi

# Zip up the assignment using `tar`
tar -czf $STUDENT_NAME.tar.gz $1
