GCC := gcc

OUTPUT := computeMe
SOURCES := $(wildcard *.c src/*.c)
CCFLAGS := -lm -lpthread -mavx

all: $(OUTPUT)

$(OUTPUT):
	$(GCC) -o $(OUTPUT) $(CCFLAGS) $(SOURCES)

clean:
	rm $(OUTPUT)