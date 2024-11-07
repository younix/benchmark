PLATFORM := $(shell uname)
# GNU/Linux
ifeq  ($(PLATFORM),Linux)
	CFLAGS+=$(shell pkg-config --cflags libbsd) -D _XOPEN_SOURCE
	LDLIBS+=$(shell pkg-config --libs libbsd)
endif

include Makefile
