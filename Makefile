CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

LIBRARY = libhf.a
PREFIX ?= /usr/local

.PHONY: test library install clean

test:
	$(MAKE) -C tst

library: $(LIBRARY)

install:
	find include -type f -exec install -Dm 644 "{}" "$(PREFIX)/{}" \;
	mkdir -p $(PREFIX)/lib
	install -m 644 $(LIBRARY) $(PREFIX)/lib

clean:
	rm $(LIBRARY)

$(LIBRARY):
	$(CC) -Wall -O2 -Iinclude src/hf.c -c -o hf.o
	$(AR) cr $(LIBRARY) hf.o
