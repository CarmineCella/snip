# Makefile for Snip

CXX = g++
CXXFLAGS = -Wall -std=c++20 -O2
TARGET = snip
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
DEPS = $(SRC:.cpp=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJ) $(DEPS) $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
