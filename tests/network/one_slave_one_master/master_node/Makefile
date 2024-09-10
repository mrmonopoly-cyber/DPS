C_FLAGS= -Wall -Wextra -fsanitize=address -g

DPS_ROOT = ../../../../
CANLIB_ROOT = ../../can_lib

include $(DPS_ROOT)/Makefile
include $(CANLIB_ROOT)/Makefile

OBJ_FILE = c_vector.o canlib.o dps_master.o

all: main

main: main.c $(OBJ_FILE)
	gcc $(C_FLAGS) main.c ../../../test_lib.c $(OBJ_FILE) -o main

clean:
ifeq ($(wildcard main), main)
	rm main
endif
