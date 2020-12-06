all:
	gcc -O0 -g ever-keeper.c -std=c11 -o test

release:
	gcc ever-keeper.c -std=c11 -o ever-keeper.d
