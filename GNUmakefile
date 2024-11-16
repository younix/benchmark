PLATFORM := $(shell uname)
# GNU/Linux
ifeq  ($(PLATFORM),Linux)
	CFLAGS+=$(shell pkg-config --cflags libbsd-overlay)
	CFLAGS+=-D _XOPEN_SOURCE -D _POSIX_C_SOURCE=200112L
	LDLIBS+=$(shell pkg-config --libs libbsd)
endif

include Makefile
