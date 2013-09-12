#
# Makefile for screenhack-rebel
#

CFLAGS=-Wall

all: screenhack-rebel.so

screenhack-rebel.so: LDLIBS=-ldl

%.so: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -fpic -shared -o $@
