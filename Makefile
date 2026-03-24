CXX = g++
SRCS = $(wildcard src/*.cpp)
TARGET = main.exe
CXXFLAGS = -Wall -std=c++11

.PHONY: build run clean help

build: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: build
	.\$(TARGET)

clean:
	rm -f $(TARGET) *.o

help:
	@echo "Available targets:"
	@echo "  make build  - Compile the program"
	@echo "  make run    - Build and run the program"
	@echo "  make clean  - Remove compiled files"
	@echo "  make help   - Show this help message"
