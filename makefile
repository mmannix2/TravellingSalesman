CFLAGS=-Wall -g --std=c++11 -pthread -lm
DEPS= City.h Organism.h
TARGET=tsp

all: $(TARGET)

$(TARGET):
	mpic++ -o $(TARGET) $(CFLAGS) $(DEPS) $(TARGET).cpp
	#Run with "mpirun -np X tsp Y" Where X = # processes and Y = seed.
clean:
	rm $(TARGET)
