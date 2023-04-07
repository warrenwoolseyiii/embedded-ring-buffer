#!/bin/sh
echo "Running pre-commit hook..."
# Change the current working directory to the root of the project
./utilities/version/version.sh
./utilities/format/format.sh