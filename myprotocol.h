#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>	/* printf() and fprintf() */
#include	<stdlib.h>	/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>

#define	NAMEMAX	24
#define	RESPONSEMAX	255

struct Player
{
    int registered;
    //struct sockaddr_in * socketAddr;
    //char * name;
    //char * ipAddr;
    //char * port;
    char name[15];
    char ipAddr[15];
    char port[4];
};

struct Game
{
    int ID;
    int numPlayers;
    struct Player caller;
    struct Player players[10];
};

typedef struct node {
    int val;
    struct Game games;
    struct Player playerList;
    struct node * next;
} node_t;

struct bingoBoard {
    int board[5][5];
};

struct Message
{
    int type;  // 0 = register, 1 = query players, 2 = start, 3 = query games, 4 = end, 5 = deregister
    char playerName[15];
    char ipAddr[15];
    char port[4];
    struct Player caller;
    struct sockaddr_in * socketAddr;	// type 0
    int numPlayers;		// type 1, 2
    int gameID;		// type 4
    int success;
    struct Game game;
    struct node gameList;
    struct Player playerList[10];
};