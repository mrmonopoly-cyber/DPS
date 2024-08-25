CC = gcc

C_FLAGS =-Wall -Wextra  -fsanitize=address -g

dps_src_path := $(or $(DPS_ROOT), .)
dps_slave.c := $(dps_src_path)/dps_slave.c
dps_master.c := $(dps_src_path)/dps_master.c

ifndef $(C_VECTOR_ROOT)
C_VECTOR_ROOT := $(dps_src_path)/lib/c_vector
include $(C_VECTOR_ROOT)/Makefile
endif

all: master slave

master: dps_master.o
slave : dps_slave.o

dps_master.o: $(dps_master.c)
	$(CC) $(C_FLAGS) $(dps_master.c) -c

dps_slave.o: $(dps_slave.c)
	$(CC) $(C_FLAGS) $(dps_slave.c) -c

dps_clean:
	rm dps_slave.o
	rm dps_master.o


clean: dps_clean
