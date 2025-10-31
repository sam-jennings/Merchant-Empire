CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = merchant_empire
SOURCES = main.cpp Card.cpp Contract.cpp Player.cpp Council.cpp Game.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run