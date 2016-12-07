CC=g++
CFLAGS=-Wall -g --std=c++11 -pthread -lm
DEPS= City.h Organism.h
TARGET=tsp

all: $(TARGET)

$(TARGET):
	$(CC) $(TARGET).cpp $(DEPS) -o $(TARGET) $(CFLAGS)
clean:
	rm $(TARGET)
