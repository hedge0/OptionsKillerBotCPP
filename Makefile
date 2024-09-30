# Use Clang for compilation
CXX = clang++
CXXFLAGS = -Wall -std=c++17 -Iinclude -O3 -march=native -flto -fomit-frame-pointer -ffast-math
LDFLAGS = -lcurl -lm -flto

# vcpkg include and library paths
VCPKG_ROOT = C:/vcpkg
CXXFLAGS += -I$(VCPKG_ROOT)/installed/x64-windows/include
LDFLAGS += -L$(VCPKG_ROOT)/installed/x64-windows/lib

SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXEC = $(BIN_DIR)/app.exe

# Enable parallel compilation by default
MAKEFLAGS += -j$(shell nproc)

all: $(EXEC)

# Build the executable and link with the curl and math libraries
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

# PHONY to avoid conflicts with file names
.PHONY: all clean
