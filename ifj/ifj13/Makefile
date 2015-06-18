# Makefile


#
#
# Definice promennych
CC=gcc
CFLAGS=-std=c99 -Wall -W -Wextra -pedantic 
RM=rm -f

#
#
# Seznam vytvorenych souboru
all: interpret

#
#
# GCC a zavislosti pro interpret
interpret: main.o scanner.o parser.o ial.o instlist.o expr_list.o inc_func.o interpret.o
	$(CC) interpret.o inc_func.o instlist.o ial.o parser.o scanner.o expr_list.o main.o -o $@

#
#
# GCC a zavislosti pro scanner
main.o: main.c scanner.h parser.h ial.h instlist.h inc_func.h interpret.h
	$(CC) $(CFLAGS) -c main.c -o $@

#
#
# GCC a zavislosti pro scanner
scanner.o: scanner.c scanner.h
	$(CC) $(CFLAGS) -c scanner.c -o $@

#
#
# GCC a zavislosti pro parser
parser.o: parser.c parser.h scanner.h
	$(CC) $(CFLAGS) -c parser.c -o $@

#
#
# GCC a zavislosti pro stable
ial.o: ial.c ial.h 
	$(CC) $(CFLAGS) -c ial.c -o $@

#
#
# GCC a zavislosti pro expr_list
expr_list.o: expr_list.c expr_list.h
	$(CC) $(CFLAGS) -c expr_list.c -o $@


#
#
# GCC a zavislosti pro scanner
instlist.o: instlist.c instlist.h scanner.h
	$(CC) $(CFLAGS) -c instlist.c -o $@

#
#
# GCC a zavislosti pro inc_ins
inc_func.o: inc_func.c inc_func.h parser.h scanner.h
	$(CC) $(CFLAGS) -c inc_func.c -o $@

#
#
# GCC a zavislosti pro interpret
interpret.o: interpret.c ial.h instlist.h
	$(CC) $(CFLAGS) -c interpret.c -o $@

#
#
# clean parametr makra, pro smazani prekladacem vytvorenych souboru
.PHONY: clean

clean:
	$(RM) *.o interpret

