CXX = gcc
CXXFLAGS =-c -Wall -std=c++11 
LIBS = -lSDL2 -lstdc++

TARGET = snake

SRCS = main.cpp

OBJS =$(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)