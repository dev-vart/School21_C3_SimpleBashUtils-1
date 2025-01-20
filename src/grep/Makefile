CC = gcc
FLG = -std=c11 -Wall -Werror -Wextra

all: s21_grep

s21_grep:
	$(CC) $(FLG) s21_grep.c -o s21_grep

testgrep:
	$(CC) $(FLG) s21_grep.c -o test/s21_grep

noflags:
	$(CC) s21_grep.c -o s21_grep

valgrind: s21_grep
	valgrind --tool=memcheck --leak-check=yes  ./s21_grep

clear:
	rm s21_grep

rebuild: clear all