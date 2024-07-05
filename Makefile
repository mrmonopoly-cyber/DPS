C_FLAGS =-Wall -Wextra  -fsanitize=address -g

dps_src_path := $(or $(DPS_ROOT), .)
dps_slave.c := $(dps_src_path)/dps_slave.c

ifndef $(C_VECTOR_ROOT)
C_VECTOR_ROOT := $(dps_src_path)/lib/c_vector
include $(C_VECTOR_ROOT)/Makefile
endif

all: dps_slave.o

dps_slave.o: $(dps_slave.c)
	gcc $(C_FLAGS) $(dps_slave.c) -c

dps_clean:
	rm dps_slave.o


clean: dps_clean
