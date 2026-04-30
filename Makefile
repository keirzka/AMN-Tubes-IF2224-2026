CXX = g++
CXXFLAGS = -Wall -std=c++11

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET = $(BIN_DIR)/main

# Ambil semua file .cpp secara rekursif
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

# Ubah src/... jadi obj/...
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

.PHONY: build run clean help

build: $(TARGET)

# Link
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile + buat folder obj otomatis
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: build
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

help:
	@echo "Available targets:"
	@echo "  make build  - Compile program"
	@echo "  make run    - Build and run"
	@echo "  make clean  - Remove build files"