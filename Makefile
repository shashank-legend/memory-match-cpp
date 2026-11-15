CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS += -Iinclude -MMD -MP

BUILD    := build
TARGET   := memory-match
TEST_BIN := $(BUILD)/run_tests

LIB_SRC  := src/Card.cpp src/Player.cpp src/Board.cpp src/Game.cpp src/InputUtils.cpp
LIB_OBJ  := $(LIB_SRC:src/%.cpp=$(BUILD)/%.o)
MAIN_OBJ := $(BUILD)/main.o
TEST_OBJ := $(BUILD)/test_main.o

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(LIB_OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TEST_BIN): $(LIB_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD)/%.o: tests/%.cpp | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD) $(TARGET)

-include $(wildcard $(BUILD)/*.d)
