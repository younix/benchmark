PLATFORM := $(shell uname)
# GNU/Linux
ifeq  ($(PLATFORM),Linux)
	CFLAGS+=$(shell pkg-config --cflags libbsd-overlay) -D _XOPEN_SOURCE
	LDLIBS+=$(shell pkg-config --libs libbsd)
endif

include Makefile
