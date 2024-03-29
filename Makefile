CC=gcc
BUILDOPTS=$(CPPFLAGS) $(CFLAGS) -I. $(LDFLAGS) -lcrypt -lini_config -lssh2 -g -DUSE_SHADOW_AUTH -DUSE_MCTP_SSH
DEPS = hellomake.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -c -o $@ $<

mpasswd: mpasswd.o auth.o shadowauth.o pwstrength.o chpasswd.o appconfig.o getpass.o sshclient.o
	$(CC) $(BUILDOPTS) -o $@ $^
	sudo chown root:root $@ && sudo chmod u+s $@

build: mpasswd

.PHONY: clean

clean:
	rm -rf *.o mpasswd
