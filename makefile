manager:
	gcc tcp-server.c myprotocol.h -o manager

bingo:
	gcc tcp-client.c myprotocol.h -o bingo
