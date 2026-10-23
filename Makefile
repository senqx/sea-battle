CXX ?= g++
CPPFLAGS ?=
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS ?=
SOURCES = Main.cpp game/SBGame.cpp helpers/ClientServerConfig.cpp helpers/IPPortConfig.cpp
OBJECTS = $(SOURCES:.cpp=.o)

SeaBattle: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

input_tests: tests/input_tests.cpp helpers/IPPortConfig.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

test: SeaBattle input_tests
	./input_tests
	python3 tests/game_tests.py

clean:
	$(RM) $(OBJECTS) $(OBJECTS:.o=.d) SeaBattle input_tests

-include $(OBJECTS:.o=.d)
.PHONY: test clean
