C_FLAGS =-Wall -Wextra  -fsanitize=address -g

dps_src_path := $(or $(DPS_ROOT), .)
dps.c := $(dps_src_path)/dps.c

ifndef $(C_VECTOR_ROOT)
C_VECTOR_ROOT := $(dps_src_path)/lib/c_vector
include $(C_VECTOR_ROOT)/Makefile
endif

all: dps.o

dps.o: $(dps.c)
	gcc $(C_FLAGS) $(dps.c) -c

dps_clean:
	rm dps.o


clean: dps_clean
