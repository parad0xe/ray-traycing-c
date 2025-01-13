BASE_DIR = src/
INCLUDE_DIR = includes/

CC=gcc
CFLAGS=-Wall -Wextra -Ofast
LDFLAGS=`sdl2-config --cflags --libs` -lm
I=-I $(INCLUDE_DIR)
EXECUTABLE=main

SOURCES = $(BASE_DIR)main.c \
	$(BASE_DIR)utils.c \
	$(BASE_DIR)environment.c

HEADERS = $(INCLUDE_DIR)types.h \
	$(INCLUDE_DIR)constants.h

OBJECTS = $(SOURCES:.c=.o)

.SILENT:

.PHONY: all
all: $(EXECUTABLE)

.PHONY: main
main : $(OBJECTS)
	@echo ""
	$(CC) $(I) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	@echo "Compiling ($(EXECUTABLE)).. done"

.PHONY: clean
clean:
	rm -f $(BASE_DIR)*.o $(EXECUTABLE)
	@echo "Cleaning.. done"

%.o : %.c $(HEADERS)
	$(CC) $(I) -o $@ -c $< $(CFLAGS) $(LDFLAGS)
	@echo "Compiling ($<).. ok"
	
