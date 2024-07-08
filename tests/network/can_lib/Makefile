C_FLAGS =-Wall -Wextra  -fsanitize=address -g

canlib_src_path := $(or $(CANLIB_ROOT), .)
canlib.c := $(canlib_src_path)/canlib.c

all: canlib.o

canlib.o: $(canlib.c)
	gcc $(C_FLAGS) $(canlib.c) -c

canlib_clean:
	rm canlib.o

clean: canlib_clean
