PLATFORM := $(shell uname)
# GNU/Linux
ifeq  ($(PLATFORM),Linux)
	CFLAGS+=$(shell pkg-config --cflags libbsd)
	LDLIBS+=$(shell pkg-config --libs libbsd)
endif

include Makefile
