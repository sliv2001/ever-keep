all: release debug

release:
	gcc ever-keeper.c -std=c11 -D DAEMON -o ever-keeper.d

debug:
	gcc -O0 -g ever-keeper.c -std=c11 -o test
