BASEDIR = src/

CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=`sdl2-config --cflags --libs` -lm
I=-I includes/
EXECUTABLE=main

SOURCES = $(BASEDIR)main.c \
	$(BASEDIR)utils.c \
	$(BASEDIR)environment.c

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
	rm -f $(BASEDIR)*.o $(EXECUTABLE)
	@echo "Cleaning.. done"

%.o : %.c
	$(CC) $(I) -o $@ -c $< $(CFLAGS) $(LDFLAGS)
	@echo "Compiling ($<).. ok"
	
