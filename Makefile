CC = gcc
C_FLAGS = -Wall -Wextra -g
C_EXFLAGS = ""

dps_src_path := $(or $(DPS_ROOT), .)
dps_slave.c := $(dps_src_path)/dps_slave.c
dps_master.c := $(dps_src_path)/dps_master.c

ifndef $(C_VECTOR_ROOT)
C_VECTOR_ROOT := $(dps_src_path)/lib/c_vector
include $(C_VECTOR_ROOT)/Makefile
endif
all: release

DEBUG_FLAGS = -O0 -fsanitize=address
RELEASE_FLAGS = -O2

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: compile

release: C_FLAGS += $(RELEASE_FLAGS)
release: compile


compile: dps_master.o dps_slave.o

dps_master.o: $(dps_master.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(dps_master.c) -c

dps_slave.o: $(dps_slave.c) 
	$(CC) $(C_FLAGS) $(C_EXFLAGS) $(DEBUG) $(dps_slave.c) -c

dps_clean:
ifeq ($(wildcard dps_slave.o), dps_slave.o)
	rm dps_slave.o
endif
ifeq ($(wildcard dps_master.o), dps_master.o)
	rm dps_master.o
endif

clean: dps_clean
