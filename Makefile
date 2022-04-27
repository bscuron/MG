CC=gcc
FLAGS=-Wall -Werror -O3

mg:
	$(CC) -o mg mg.c $(FLAGS)
