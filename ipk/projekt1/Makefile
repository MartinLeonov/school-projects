CC=g++49
CFLAGS=-std=c++11 -static-libstdc++ -Wall -W -Wextra -pedantic -g -Werror
RM=rm -f

all: ftpclient

ftpclient: ftpclient.cpp
	$(CC) $(CFLAGS) ftpclient.cpp -o $@

.PHONY: clean
clean:
	$(RM) *.o ftpclient
