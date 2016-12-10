CFLAGS=-Wall -g --std=c++11 -pthread -lm
DEPS= City.h Organism.h
TARGET=tsp

all: $(TARGET)

$(TARGET):
	#$g++ $(TARGET).cpp $(DEPS) -o $(TARGET) $(CFLAGS)
	mpic++ -o $(TARGET) $(CFLAGS) $(DEPS) $(TARGET).cpp

clean:
	rm $(TARGET)
