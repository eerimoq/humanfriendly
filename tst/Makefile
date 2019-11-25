INC += \
	. \
	../include
SRC += \
	nala.c \
	nala_mocks.c \
	../src/hf.c \
	main.c
CFLAGS += \
	-Werror \
	-O0 \
	-no-pie \
	-g \
	-Wall \
	-fprofile-arcs \
	-ftest-coverage \
	-fsanitize=address \
	$(INC:%=-I%)
NALA ?= nala

test: nala_mocks.h
	$(CC) $(CFLAGS) $$(cat nala_mocks.ld) $(SRC) -o main
	./main

nala_mocks.h: main.c
	[ -f nala_mocks.h ] || (touch nala_mocks.h)
	$(CC) $(INC:%=-I%) -E main.c | $(NALA) generate_mocks
