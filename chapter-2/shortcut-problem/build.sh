#!/usr/bin/env bash
# build_shortcut.sh
# Usage:
#   ./build_shortcut.sh [baseline|other]

SRC="shortcut_problem.cc"
OUT="shortcut_problem"
GLOBAL_FLAGS="-O3 -march=native -std=c++20 -Wall -Wextra"
BENCH_FLAGS="-I../../benchmark/include -L../../benchmark/build/src"
LINK_FLAGS="-lbenchmark -lpthread"

# Check if the first argument is "baseline"
if [[ "$1" == "v0" ]]; then
    echo "Building v0 version..."
    FLAG="-DV0"
    SUFFIX="_v0"
elif [[ "$1" == "openmp" ]]; then
    echo "Building v0 version..."
    FLAG="-fopenmp -DOPENMP"
    SUFFIX="_omp"
else
    echo "Unknown version"
    exit
fi

set -x
g++ -g $GLOBAL_FLAGS $BENCH_FLAGS $FLAG "$SRC" $LINK_FLAGS -o "${OUT}${SUFFIX}"
g++ -S $GLOBAL_FLAGS $BENCH_FLAGS $FLAG "$SRC" $LINK_FLAGS -o "${OUT}${SUFFIX}.S"
set +x
echo "Build complete."
