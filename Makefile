CXX = g++ 
CXXFLAGS = -lpthread -g

TARGET1 = tour_test2
TARGET2 = tour_test

SOURCE1 = tour_test2.cpp
SOURCE2 = tour_test.cpp

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SOURCE1)
	$(CXX) $(SOURCE1) -o $(TARGET1) $(CXXFLAGS)

$(TARGET2): $(SOURCE2)
	$(CXX) $(SOURCE2) -o $(TARGET2) $(CXXFLAGS)

val1: $(TARGET1)
	valgrind --leak-check=full ./$(TARGET1) $(ARGS)

val2: $(TARGET2)
	valgrind --leak-check=full ./$(TARGET2) $(ARGS)

hel1: $(TARGET1)
	valgrind --tool=helgrind ./$(TARGET1) $(ARGS)

hel2: $(TARGET2)
	valgrind --tool=helgrind ./$(TARGET2) $(ARGS)

.PHONY: clean val1 val2 hel1 hel2
clean:
	rm -f $(TARGET1) $(TARGET2)