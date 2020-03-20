#!/bin/bash

#execute from the project root
for filename in ./tests/succeed/q*.txt; do
  printf "Testing $(basename $filename): "
  ./build/Release/bin/grammar "$filename" > /dev/null && echo "Success!"
done
