#
# Makefile for screenhack-rebel
#

CFLAGS=-g -Wall `pkg-config --cflags gtk+-2.0` -DHOOK_GDK_SPAWN

all: screenhack-rebel.so

screenhack-rebel.so: LDLIBS=-ldl

%.so: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -fpic -shared -o $@
