CC=gcc
CFLAGS=-Wall -g --std=c99 -pthread -lm
TARGET=tsp

all: $(TARGET)

$(TARGET):
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS)
clean:
	rm $(TARGET)
