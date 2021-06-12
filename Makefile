.PHONY: clean
.DEFAULT: octreegen
CC = gcc
CFLAGS = -std=c99 -Wall
LFLAGS = -lm
ifneq ($(BUILD), debug)
        BUILD = release
endif

ifeq ($(BUILD), debug)
        CFLAGS += -O0 -g
else
        CFLAGS += -O3 -DNDEBUG
endif

octreegen: *.c *.h
	$(CC) $(CFLAGS) -o octreegen *.c $(LFLAGS)
clean:
	rm -f octreegen
