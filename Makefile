CXX = g++
CXXFLAGS = -Wall -std=c++11

# Definisi Folder
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Mencari file sumber
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Mengubah src/file.cpp menjadi obj/file.o
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Nama target dengan path folder bin
TARGET = $(BIN_DIR)/main

.PHONY: build run clean help

# Target 'build' sekarang bergantung pada keberadaan folder
build: $(BIN_DIR) $(OBJ_DIR) $(TARGET)

# Link file objek menjadi eksekusi
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Kompilasi file .cpp menjadi .o di dalam folder obj
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule untuk membuat direktori jika belum ada
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

run: build
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

help:
	@echo "Available targets:"
	@echo "  make build  - Compile the program (outputs to bin/ and obj/)"
	@echo "  make run    - Build and run the program"
	@echo "  make clean  - Remove bin/ and obj/ directories"
	@echo "  make help   - Show this help message"