#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <num_threads>"
  exit 1
fi

NUM_THREADS=$1
RUNS=10

for i in $(seq 1 $RUNS); do
  echo "Run $i/$RUNS with $NUM_THREADS thread(s)..."
  echo "$NUM_THREADS" | ./dist/threads
  [ $i -lt $RUNS ] && sleep 1
done
