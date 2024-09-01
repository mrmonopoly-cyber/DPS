C_FLAGS= -Wall -Wextra -g -fsanitize=address

DPS_ROOT = ../..

include $(DPS_ROOT)/Makefile

OBJ_FILE = dps_slave.o c_vector.o

all: main

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) main.c $(OBJ_FILE) ../test_lib.c -o main

clean:
	rm main
