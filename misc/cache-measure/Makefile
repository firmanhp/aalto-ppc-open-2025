# Compiler and flags
CXX := g++
CXXFLAGS := -O3 -march=native -std=c++20 -Wall -Wextra
BENCHFLAGS := -I../../benchmark/include -L../../benchmark/build/src
LDFLAGS := -lbenchmark -lpthread

# Source and target
SRC := cache.cc
BIN := cache

# Default target
all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(BENCHFLAGS) $< -o $@ $(LDFLAGS)

# bind to one core
# note: https://google.github.io/benchmark/user_guide.html#output-files
# csv is deprecated, use json
run: $(BIN)
	taskset -c 0 ./$(BIN) --benchmark_report_aggregates_only=true --benchmark_out=results.json --benchmark_out_format=json

clean:
	rm -f $(BIN)

.PHONY: all run clean
