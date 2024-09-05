C_FLAGS= -Wall -Wextra -g -fsanitize=address

DPS_ROOT = ../..

include $(DPS_ROOT)/Makefile

OBJ_FILE = dps_slave.o c_vector.o

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile


compile: main

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) main.c $(OBJ_FILE) ../test_lib.c -o main

clean:
	rm main
