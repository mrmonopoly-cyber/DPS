CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

DPS_ROOT = ../..

include $(DPS_ROOT)/Makefile

all: release

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean compile

compile: main

OBJ_FILE = dps_master.o c_vector.o

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) main.c ../test_lib.c $(OBJ_FILE) -o main

clean:
ifeq ($(wildcard main),main)
	rm main
endif
