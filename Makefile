ifneq ($(USE_LUAJIT),)
LUA_CPPFLAGS=-I/usr/include/luajit-2.0
LUA_LIBS=-lluajit-2.0
else
ifeq ($(LUA_VERSION),)
LUA_VERSION=5.2
endif
LUA_CPPFLAGS=-I/usr/include/lua$(LUA_VERSION)
LUA_LIBS=-llua$(LUA_VERSION)
endif

ifneq ($(DEBUG),)
EXTRA_CFLAGS+= -g -O0
endif

CFLAGS=-Wall -Werror -pedantic -std=c99 -fPIC $(EXTRA_CFLAGS) $(LUA_CPPFLAGS)
LDFLAGS=-Wl,--no-undefined
LIBS=$(LUA_LIBS)

.PHONY: all
all: lextlib.o

lextlib.o: lextlib.c lextlib.h lextlib_global.h lextlib_lua52.h

.PHONY: clean
clean:
	$(RM) *.so *.o

.SUFFIXES: .c .o .so

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.o.so:
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
