C_FLAGS= -Wall -Wextra -g -fsanitize=address

DPS_ROOT = ../../../../
CANLIB_ROOT = ../../can_lib

include $(DPS_ROOT)/Makefile
include $(CANLIB_ROOT)/Makefile

OBJ_FILE = dps_slave.o c_vector.o c_check_input.o canlib.o

all: main

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) main.c ../../../test_lib.c $(OBJ_FILE) -o main

clean:
	rm main
