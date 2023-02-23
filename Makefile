CC = gcc
CFLAGS =
LDFLAGS = -lm -lasound

SOURCES = mfgen.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = mfgen

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
