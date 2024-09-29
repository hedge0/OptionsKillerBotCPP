CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -I/mingw64/include/eigen3 -IC:/Users/Elias/AppData/Local/Programs/Python/Python312/Include -IC:/Users/Elias/AppData/Local/Programs/Python/Python312/Lib/site-packages/pybind11/include -O3 -march=native -flto -fomit-frame-pointer -ffast-math
LDFLAGS = -lcurl -lm -flto -LC:/Users/Elias/AppData/Local/Programs/Python/Python312/libs -lpython312

SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/app.exe

# Detect number of cores and set parallel compilation flag accordingly
NPROC = $(shell nproc 2>/dev/null || sysctl -n hw.ncpu)
MAKEFLAGS += -j$(NPROC)

all: $(EXEC)

# Build the executable and link with the necessary libraries
$(EXEC): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile individual source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# PHONY target to avoid conflicts with filenames
.PHONY: all clean
