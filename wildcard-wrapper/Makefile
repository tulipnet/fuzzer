CC=gcc
CC_OPTIONS_BASE=-Wall -Wextra -Werror -Wno-unknown-pragmas -Wno-unknown-attributes
SRC_BASE=$(filter-out apply_wildcard.c put_concrete_data_on_2nd_and_next_wildcards.c concretize_wildcard.c, $(wildcard *.c))
OBJ_BASE=$(SRC_BASE:.c=.o)
OBJ_i386_BASE=$(SRC_BASE:.c=-i386.o)

ifeq ($(DEBUG), 1)
	CC_OPTIONS=-g -DNO_CRASH_WHEN_THERE_IS_NO_WILDCARD $(CC_OPTIONS_BASE)
else
	CC_OPTIONS=$(CC_OPTIONS_BASE)
endif

.PHONY: wrapper wrapper-i386 toolbox clean

%.o: %.c
	${CC} -c $^ ${CC_OPTIONS} -o $@

%-i386.o: %.c
	${CC} -c -m32 $^ ${CC_OPTIONS} -o $@

wrapper: $(OBJ_BASE)
	ld -relocatable $^ -o wrapper_final.o

wrapper-i386: $(OBJ_i386_BASE)
	ld -melf_i386 -relocatable $^ -o wrapper_final-i386.o

toolbox: $(OBJ_BASE) apply_wildcard.o put_concrete_data_on_2nd_and_next_wildcards.o concretize_wildcard.o
	gcc $(OBJ_BASE) apply_wildcard.o -o apply_wildcard
	gcc $(OBJ_BASE) put_concrete_data_on_2nd_and_next_wildcards.o -o put_concrete_data_on_2nd_and_next_wildcards
	gcc $(OBJ_BASE) concretize_wildcard.o -o concretize_wildcard

clean:
	rm -f *.o apply_wildcard put_concrete_data_on_2nd_and_next_wildcards concretize_wildcard