include Makefile

# GNU/Linux
ifeq ($(OS), Linux)
	CFLAGS+=`pkg-config --cflags bsd`
	LDFLAGS+=`pkg-config --libs bsd`
endif
