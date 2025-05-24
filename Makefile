CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

dps_src_path := $(or $(DPS_ROOT), .)
dps_master.c := $(dps_src_path)/src/master/dps_master.c
dps_slave.c := $(dps_src_path)/src/slave/dps_slave.c
dps_messages.c := $(dps_src_path)/src/common/dps_messages.c

ifndef $(C_VECTOR_ROOT)
C_VECTOR_ROOT := $(dps_src_path)/lib/c_vector
include $(C_VECTOR_ROOT)/Makefile 
endif

DEBUG_FLAGS = -Werror -O0 -DDEBUG -fsanitize=undefined,address -g
RELEASE_FLAGS = -O2 -Werror

all: release

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: static dynamic

release: C_FLAGS += $(RELEASE_FLAGS)
release: static dynamic

static_dps_master.o: $(dps_master.c)
	$(CC) $(C_EXFLAGS) $(DEBUG) $(dps_master.c) $(dps_messages.c) -c 

static_dps_slave.o: $(dps_slave.c)
	$(CC) $(C_EXFLAGS) $(DEBUG) $(dps_slave.c) $(dps_messages.c)  -c 

dynamic_dps_master.so: $(dps_master.c)
	$(CC) $(C_EXFLAGS) $(DEBUG) $(dps_master.c) $(dps_messages.c) -fPIC -shared -o dps_master.so

dynamic_dps_slave.so: $(dps_slave.c)
	$(CC) $(C_EXFLAGS) $(DEBUG) $(dps_slave.c) $(dps_messages.c) -fPIC -shared -o dps_slave.so

static: static_dps_slave.o static_dps_master.o

dynamic: dynamic_dps_slave.so dynamic_dps_master.so

dps_clean:
ifeq ($(wildcard dps_slave.o), dps_slave.o)
	rm dps_slave.o
endif
ifeq ($(wildcard dps_master.o), dps_master.o)
	rm dps_master.o
endif
ifeq ($(wildcard dps_messages.o), dps_messages.o)
	rm dps_messages.o
endif
ifeq ($(wildcard dps_slave.so), dps_slave.so)
	rm dps_slave.so
endif
ifeq ($(wildcard dps_master.so), dps_master.so)
	rm dps_master.so
endif

clean: dps_clean
