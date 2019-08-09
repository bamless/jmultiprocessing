DESTDIR=/usr/local/lib/jstar

all: dir bin/libmultiprocessing.so

.PHONY: dir
dir:
	@mkdir -p bin

bin/libmultiprocessing.so: src/multiprocessing.c
	gcc -Wall -Wextra -std=c99 -fPIC -Wno-unused-parameter -O3 -s -shared -o $@ $< -Wl,-rpath /usr/local/lib -ljstar

.PHONY: install
install:
	mkdir -p $(DESTDIR)
	cp bin/libmultiprocessing.so src/multiprocessing.jsr $(DESTDIR)

.PHONY: clean
clean:
	rm -rf bin
