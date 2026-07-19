CXX      ?= g++
CXXFLAGS ?= -std=c++2a -g -fpermissive

# Homebrew installs these keg-only, so ask brew where they live.
OPENSSL_PREFIX := $(shell brew --prefix openssl@3 2>/dev/null)
LUA_PREFIX     := $(shell brew --prefix lua 2>/dev/null)

CPPFLAGS += -I. -I$(LUA_PREFIX)/include/lua -I$(OPENSSL_PREFIX)/include
LDFLAGS  += -L$(LUA_PREFIX)/lib -L$(OPENSSL_PREFIX)/lib
LDLIBS   += -llua -lcrypto -pthread

exe: main.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) exe

.PHONY: clean
