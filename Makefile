CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g

SRC_DIR   := src
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/shell

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all run clean

all: $(TARGET)

# Link step: combine all object files into the executable.
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile step: one .cpp -> one .o. -MMD -MP also writes a .d file listing
# the headers this file included, so editing a header triggers a rebuild.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
