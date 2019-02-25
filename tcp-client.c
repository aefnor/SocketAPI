#include        <sys/socket.h>  /* for socket() and bind() */
#include        <stdio.h>       /* printf() and fprintf() */
#include        <stdlib.h>      /* for atoi() and exit() */
#include        <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include        <sys/types.h>
#include        <string.h>
#include        <unistd.h>
#include 		<unistd.h>
#include 		<pthread.h>

#include "myprotocol.h"

#define ECHOMAX 255             /* Longest string to echo */
#define BACKLOG 128

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

//for the caller
void * hostGame(void * arg){

    struct Message * temp = (struct Message *)arg;
    struct Message m = *temp;
    printf("in thread: %s\r\n", m.game.caller.name);

    struct Message incomingM;
    struct Message outgoingM;

    struct sockaddr_in callerAddr;
    struct sockaddr_in playerAddr[m.numPlayers];

    node_t * head = NULL;
    head = malloc(sizeof(10));
    head->val = 0;
    head->next = NULL;

    int newSock;
    int fdPlayers[m.numPlayers];
//    struct sockaddr_in playerAddr;
    struct Message portRequest;
    struct Message portResponse;

    portRequest.type = 6;

//    newSock = socket(AF_INET, SOCK_STREAM, 0);
//
//    memset(&callerAddr, 0, sizeof(callerAddr));   /* Zero out structure */
//    callerAddr.sin_family = AF_INET;                /* Internet address family */
//    callerAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
//    inet_pton(AF_INET, m.game.caller.ipAddr, &callerAddr.sin_addr);
//    callerAddr.sin_port = (unsigned short)*m.game.caller.port;      /* Local port */
//
//    if (bind(newSock, (struct sockaddr *) &callerAddr, sizeof(callerAddr)) < 0)
//        DieWithError("server: bind() failed");
//    if ((listen(newSock, 5)) != 0) {
//        printf("Listen failed...\n");
//        exit(0);
//    }
//    else
//        printf("Server listening..\n");

    int bingo[5][5];
    int row;
    int row1;
    int col;
    int num=50;
    int num1=0;
    int value=0;

//    randomize();
//    clrscr();

    for(row=0;row<5;row++)
    {

        for(col=0;col<5;col++)
        {
            (rand()%num+1);

            bingo[row][col]=(rand()%num+1);

        }
        num1+=15;
    }
//    for(row=0;row<5;row++)
//    {
//        for(row1=1;row<row1;row1++)
//        {
//            do
//            {
//                value=rand()%num+1;
//                bingo[row][row] = value+num1;
//            }while(bingo[row]==bingo[row1]);
//        }
//    }

    printf("B\tI\tN\tG\tO\n\n");
    for(row=0;row<5;row++)
    {
        for(col=0;col<5;col++)
        {
            if(row==2 && col==2)
            {
                printf("FREE\t");
            }
            else
                printf("%d\t", bingo[col][row]);
        }
        printf("\n\n");
    }
//    len = sizeof(cli);
    //outgoingM.type = 6;
    /*
    int i;
    for(i = 0; i < rcvMessage.numPlayers; i++)
    {
        // get connections from each player

    }
    */
    // run the game automatically
}

//for the players
void * playGame(void * arg){
    struct Message * temp = (struct Message *)arg;
    struct Message m = *temp;
    printf("in thread: %s\r\n", m.game.caller.name);

    struct Message incomingM;
    struct Message outgoingM;

    int newSock;
    struct sockaddr_in calleraddr;

    if (arg != 3)
        DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );

    newSock = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&calleraddr, sizeof(calleraddr));
    calleraddr.sin_family = AF_INET;
    calleraddr.sin_port = (unsigned short)*m.game.caller.port;
    inet_pton(AF_INET, m.game.caller.ipAddr, &calleraddr.sin_addr);

    connect(newSock, (struct sockaddr *) &calleraddr, sizeof(calleraddr));
    // write new port to caller

}

void str_cli(FILE *fp, int sockfd)
{
    ssize_t n;
    int i;
    char sendline[ECHOMAX], recvline[ECHOMAX];
    char myName[15], myAddr[15], myPort[4];

    pthread_t thread_id;
    struct Message rcvMessage;


    while(1)
    {

        if (fgets(sendline, ECHOMAX, fp) != NULL) {

            struct Message m;
            const char delimiter[2] = " ";
            char * firstWord = strtok(sendline, delimiter);

            if (strcmp(firstWord, "register") == 0) // match
            {
                m.type = 0;
                strcpy(m.playerName, strtok(NULL, " "));
                strcpy(m.ipAddr, strtok(NULL, " "));
                strcpy(m.port, strtok(NULL, " "));

                strcpy(myName, m.playerName);
                strcpy(myAddr, m.ipAddr);
                strcpy(myPort, m.port);

                write(sockfd, &m, sizeof(m));

            }
            else if (strcmp(firstWord, "query") == 0)
            {
                char * secondWord = strtok(NULL, " ");
                if (strcmp(secondWord, "players") == 0)
                {
                    fputs("type 1\r\n", stdout);
                    m.type = 1;
                    write(sockfd, &m, sizeof(m));
                }
                else if (strcmp(secondWord, "games") == 0)
                {
                    fputs("type 3\r\n", stdout);
                    m.type = 3;
                    write(sockfd, &m, sizeof(m));
                }
                else
                {
                    // error
                    printf("you have a query error\r\n");
                }
            }
            else if (strcmp(firstWord, "start") == 0)
            {
                fputs("type 2\r\n", stdout);
                char * secondWord = strtok(NULL, " ");	// word "game", does nothing
                char charNum[3];
                strcpy(charNum, strtok(NULL, " "));
                int intNum = 0;
                intNum = *charNum - '0';
                printf("%d\r\n", intNum);

                m.type = 2;
                m.numPlayers = intNum;
                strcpy(m.port, myPort);
                write(sockfd, &m, sizeof(m));
            }
            else if (strcmp(firstWord, "end") == 0)
            {
                fputs("type 4\r\n", stdout);
                m.type = 4;
//                m.numPlayers = intNum;
                strcpy(m.port, myPort);
                write(sockfd, &m, sizeof(m));
            }
            else if (strcmp(firstWord, "deregister") == 0)
            {
                fputs("type 5\r\n", stdout);
            }
            else
                fputs("error\r\n", stdout);



            if ( (n = read(sockfd, &rcvMessage, sizeof(rcvMessage))) == 0)
                DieWithError("str_cli: server terminated prematurely");

            switch(rcvMessage.type)
            {
                case 0:	// register
                    if (rcvMessage.success == 1)
                        printf("SUCCESS\r\n");
                    else
                        printf("NOPE\r\n");
                    break;
                case 1:	// query players
                    printf("Number of players: %d\r\n", rcvMessage.numPlayers);
                    for(i = 0; i < rcvMessage.numPlayers; i++)
                    {
                        printf("Name: %s\r\n", rcvMessage.playerList[i].name);
                        printf("IP Address: %s\r\n", rcvMessage.playerList[i].ipAddr);
                        printf("Port: %s\r\n", rcvMessage.playerList[i].port);
                    }
                    break;
                case 2:	// start game
                    printf("players requested: %d\r\n", rcvMessage.numPlayers);
                    if (rcvMessage.success == 0)
                    {
                        printf("Failure\r\n");
                        break;
                    }

                    printf("Caller:\r\n");
                    printf("\tName: %s\r\n", rcvMessage.game.caller.name);
                    printf("\tIP Address: %s\r\n", rcvMessage.game.caller.ipAddr);
                    printf("\tPort: %s\r\n", rcvMessage.game.caller.port);

                    printf("Players:\r\n");
                    for(i = 0; i < rcvMessage.numPlayers; i++)
                    {
                        printf("\tName: %s\r\n", rcvMessage.playerList[i].name);
                        printf("\tIP Address: %s\r\n", rcvMessage.playerList[i].ipAddr);
                        printf("\tPort: %s\r\n", rcvMessage.playerList[i].port);
                    }
                    //gcc -pthread -o client tcp-client.c
                    //creates a thread for the caller
                    //caller thread
                    pthread_create(&thread_id, NULL, hostGame, (void *)&rcvMessage);
                    //thread should go out and connect to the players
                    break;
                case 3:	// query games
                    puts("Querying games");
//                    outgoingM.gameList = *head;

                    node_t * current = &rcvMessage.gameList;

                    if(current == NULL){
                        puts("No games to query!");
                        break;
                    }
                    while (current != NULL) {
                        printf("\nGame %d\r\n", current->games.ID);
                        printf("Caller:\r\n");
                        printf("\tName: %s\r\n", current->games.caller.name);
                        printf("\tIP Address: %s\r\n", current->games.caller.ipAddr);
                        printf("\tPort: %s\r\n", current->games.caller.port);

                        printf("Players:\r\n");
                        for(i = 0; i < current->games.numPlayers; i++)
                        {
                            printf("\tName: %s\r\n", current->games.players[i].name);
                            printf("\tIP Address: %s\r\n", current->games.players[i].ipAddr);
                            printf("\tPort: %s\r\n", current->games.players[i].port);
                        }

//                        printf("GamesList %d\n", outgoingM.gameList.val);
//                        printf("GamesList %d\n", outgoingM.gameList.games.numPlayers);
//                        printf("%d\n", current->val);
//                        printf("%d\n", current->games.numPlayers);
//                        puts(current->games.caller.name);
//                        puts(current->games.caller.ipAddr);
//                        puts(current->games.caller.port);
                        current = current->next;
                    }
                    break;
                case 4:	// end game
                    puts("\nYou game had now ended! Thanks for playing!\n");
                    current = &rcvMessage.gameList.games;
                    int gameId = rcvMessage.gameList.games.ID;

                    while (current != NULL) {
                        puts("Testing");
                        if(current->games.ID == gameId){
//                            current->val = NULL;
//                            current->playerList = [];
                            free(current);
                        }
                        else{
                            current = current->next;
                        }
//                        rcvMessage.gameList = *current;
                    }
                    break;
                case 5:	// deregister player
                    break;
                case 6:	// caller requesting port
                    printf("port requested!!\r\n");

                    // call player thread
                    pthread_create(&thread_id, NULL, playGame, (void *)&rcvMessage);
                    break;
                default:
                    printf("transmission failure\r\n");
                    break;
            }
            //recvline[ n ] = '\0';
            //fputs(recvline, stdout);

        }
    }
}


int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 3)
        DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);		/* do it all */

    exit(0);
}