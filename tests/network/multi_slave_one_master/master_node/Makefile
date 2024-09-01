C_FLAGS= -Wall -Wextra -g -fsanitize=address

DPS_ROOT = ../../../../
CANLIB_ROOT = ../../can_lib

include $(DPS_ROOT)/Makefile
include $(CANLIB_ROOT)/Makefile

OBJ_FILE = dps_master.o c_vector.o canlib.o

all: main

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) $(OBJ_FILE) main.c ../../../test_lib.c -o main

clean:
	rm main
