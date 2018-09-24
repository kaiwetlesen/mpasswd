CC=gcc
CFLAGS=-I. -lcrypt -lini_config -DUSE_SHADOW_AUTH
DEPS = hellomake.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

mpasswd: mpasswd.o auth.o shadowauth.o
	$(CC) $(CFLAGS) -o $@ $^

build: mpasswd

.PHONY: clean

clean:
	rm -rf *.o mpasswd
