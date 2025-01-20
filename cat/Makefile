CC = gcc
FLG = -std=c11 -Wall -Werror -Wextra

all: s21_cat

s21_cat:
	$(CC) $(FLG) s21_cat.c table.c -o s21_cat

testcat:
	$(CC) $(FLG) s21_cat.c table.c -o test/s21_cat

noflags:
	$(CC) s21_cat.c table.c -o s21_cat

valgrind: s21_cat
	valgrind --tool=memcheck --leak-check=yes  ./s21_cat

clear:
	rm s21_cat

rebuild: clear all