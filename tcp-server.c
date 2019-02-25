#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>	/* printf() and fprintf() */
#include	<stdlib.h>	/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include	<sys/time.h>
#include	<errno.h>

#include "myprotocol.h"


#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define MAXPLAYERS 10

void DieWithError(const char *errorMessage) /* External error handling function */
{
    perror(errorMessage);
    exit(1);
}

/*
void EchoString(int sockfd)
{
	struct Player players[MAXPLAYERS];
	int playerIndex = 0;

    ssize_t n;
    //char line[ECHOMAX];
	struct Message incomingM;
	struct Message outgoingM;
    for ( ; ; ) {
	    if ( (n = read(sockfd, &incomingM, sizeof(incomingM)) == 0 ))
   	    	return;

		printf("incomingM type: %d\r\n", incomingM.type);

		switch (incomingM.type)
		{
		case 0:	// register player
			printf("Registered player\r\n");
			struct Player p;
			p.registered = 1;

			memset(p.name, '\0', sizeof(p.name));
			memset(p.ipAddr, '\0', sizeof(p.ipAddr));
			memset(p.port, '\0', sizeof(p.port));

			strcpy(p.name, incomingM.playerName);
			strcpy(p.ipAddr, incomingM.ipAddr);
			strcpy(p.port, incomingM.port);

			players[playerIndex] = p;
			playerIndex++;

			outgoingM.type = 0;

			outgoingM.registerSuccess = 1;
			write(sockfd, &outgoingM, sizeof(outgoingM) );
			break;
		case 1:	// query players
			printf("Query players\r\n");
			outgoingM.type = 1;
			outgoingM.numPlayers = playerIndex;

			int i;
			for(i = 0; i < playerIndex; i++)
				outgoingM.playerList[i] = players[i];

			write(sockfd, &outgoingM, sizeof(outgoingM) );
			break;
		case 2:	// start game
			printf("%d\r\n", incomingM.numPlayers);
			outgoingM.type = 2;
			outgoingM.numPlayers = incomingM.numPlayers;
			write(sockfd, &outgoingM, sizeof(outgoingM) );
			break;
		case 3:	// query games
			break;
		case 4:	// end game
			break;
		case 5:	// deregister player
			break;
		default:
			printf("DEFAULT\r\n");
			break;
		}

        //write(sockfd, &newM, n );
    }
}
*/
int main(int argc, char **argv)
{
    struct Player players[MAXPLAYERS];
    struct Game games[10];
    int playerIndex = 0;
    int gameIndex = 0;
    int randInts[MAXPLAYERS];
    int j = 0;
    int portStart = 8500;
    int portIndex = 0;
    int newPort = 0;



    ssize_t n;
    //char line[ECHOMAX];
    struct Message incomingM;
    struct Message outgoingM;
    node_t * head = NULL;
    head = malloc(sizeof(10));
    head->val = 0;
    head->next = NULL;

    int sock, connfd;                /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

    fd_set readfds;
    int opt = 1;
    int master_socket , addrlen , new_socket , client_socket[30] ,
            max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    char buffer[1025];
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }


    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage: %s <TDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("server: socket() failed");

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0 )
    {
        DieWithError("server: socket setup failed");
    }


    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("server: bind() failed");

    if (listen(sock, BACKLOG) < 0 )
        DieWithError("server: listen() failed");

    cliAddrLen = sizeof(echoClntAddr);
    //connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );

    //EchoString(connfd);

    while(1)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(sock, &readfds);
        max_sd = sock;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
        struct timeval tv = {3, 0};
        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if(activity == 0){
            continue;
        }

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(sock, &readfds))
        {
            puts("Here");
            if ((new_socket = accept(sock,(struct sockaddr *)&echoServAddr, &cliAddrLen))<0)
            {
                DieWithError("server: accept() failed");
            }

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            if (client_socket[i] == 0)
                break;

            sd = client_socket[i];
            puts("Reading user input\n");
//            int timer = select( max_sd, &readfds , NULL , NULL , &tv);

//            if (timer > 0 ){
//            int timer = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
//            if(timer == 0) {
//                printf("From select: \n%d\n", timer);
//                continue;
//            }
//            else{
//                printf("From select: \n%d\n", timer);
//            }
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(sd, &fds);

            struct timeval t = {7, 0};
//            select(sd + 1, &incomingM, NULL, NULL, &t);
            n = read(sd, &incomingM, sizeof(incomingM));
//            int timer = select( read(sd, &incomingM, sizeof(incomingM)), &readfds , NULL , NULL , &tv);
            if ( n == 0 )
                break;


            printf("incomingM type: %d\r\n", incomingM.type);

            switch (incomingM.type)
            {
                case 0:	// register player
                    printf("Registered player\r\n");
                    struct Player p;
                    p.registered = 1;

                    memset(p.name, '\0', sizeof(p.name));
                    memset(p.ipAddr, '\0', sizeof(p.ipAddr));
                    memset(p.port, '\0', sizeof(p.port));

                    strcpy(p.name, incomingM.playerName);
                    strcpy(p.ipAddr, incomingM.ipAddr);
                    strcpy(p.port, incomingM.port);

                    players[playerIndex] = p;
                    playerIndex++;

                    outgoingM.type = 0;
                    outgoingM.success = 1;

                    write(sd, &outgoingM, sizeof(outgoingM) );
                    break;
                case 1:	// query players
                    printf("Query players\r\n");
                    outgoingM.type = 1;
                    outgoingM.numPlayers = playerIndex;

                    for(i = 0; i < playerIndex; i++)
                        outgoingM.playerList[i] = players[i];

                    write(sd, &outgoingM, sizeof(outgoingM) );
                    break;
                case 2:	// start game
                    printf("%d\r\n", incomingM.numPlayers);
                    outgoingM.type = 2;
                    if (playerIndex <= incomingM.numPlayers)
                    {
                        outgoingM.success = 0;
                        write(sd, &outgoingM, sizeof(outgoingM) );
                        break;
                    }

                    outgoingM.success = 1;
                    struct Game newGame;
                    memset(randInts, 0, sizeof(randInts));
                    for(i = 0; i < playerIndex; i++)
                    {
                        if(strcmp(players[i].port, incomingM.port) == 0)
                        {
                            newGame.caller = players[i];
                            randInts[i] = 1;
                            break;
                        }
                    }
                    struct Message portRequest;
                    portRequest.type = 6;
                    newPort = portStart + portIndex;
                    portIndex++;
                    if (portIndex >= 499)
                        portIndex = 0;
                    //way to convert an int to a char* arr
                    sprintf(portRequest.port, "%d", newPort);
                    //portRequest.ipAddr = newGame.caller.ipAddr;
                    //take caller ip address and put it inside the port request
                    strcpy(portRequest.ipAddr, newGame.caller.ipAddr);
                    j = 0;
                    while (j < incomingM.numPlayers)
                    {
//                        puts("Alive");
                        int randy = rand() % 1222;
                        int r = rand() % playerIndex;
                        if (randInts[r] == 0)
                        {
                            randInts[r] = 1;
                            outgoingM.playerList[r] = players[r];
                            newGame.players[j] = players[r];
                            newGame.numPlayers++;
                            //write(client_socket[r], &portRequest, sizeof(portRequest));
                            //while(read(client_socket[r], &incomingM, sizeof(incomingM)) == 0){ }	// wait for player to respond

                            //printf("request successful\r\n");

                            j++;
                        }
//                        puts("Alive2");
//                        j++;
                    }
                    outgoingM.numPlayers = incomingM.numPlayers;
                    newGame.ID = gameIndex;
                    gameIndex++;

                    //Adding games to games list
                    outgoingM.game = newGame;
                    head->val = head->val+1;
                    head->playerList = *outgoingM.playerList;
                    head->games = newGame;
                    head->next = NULL;

                    //socket discriptor
                    write(sd, &outgoingM, sizeof(outgoingM) );
                    break;
                case 3:	// query games
                    puts("Querying games");
                    outgoingM.type = 3;
                    if(head != NULL)
                        outgoingM.gameList = *head;
                    else
                        puts("No games to query!");
                    node_t * current = head;

                    while (current != NULL) {
                        printf("GamesList %d\n", outgoingM.gameList.val);
                        printf("GamesList %d\n", outgoingM.gameList.games.numPlayers);
                        printf("%d\n", current->val);
                        printf("%d\n", current->games.numPlayers);
                        puts(current->games.caller.name);
                        puts(current->games.caller.ipAddr);
                        puts(current->games.caller.port);
                        current = current->next;
                    }
                    write(sd, &outgoingM, sizeof(outgoingM) );
                    break;
                case 4:	// end game
                    puts("Ending your game");
                    outgoingM.game = incomingM.game;
                    int gameId = outgoingM.game.ID;

                    while (head != NULL) {
                        puts("Testing");
                        if(head->games.ID == gameId){
                            head = NULL;
                        }
                        if(head != NULL){
                            head = head->next;
                        }
                    }
                    outgoingM.type = 4;
//                    outgoingM.gameList = *head;
//                    outgoingM.game
                    write(sd, &outgoingM, sizeof(outgoingM));
                    break;
                case 5:	// deregister player
                    break;
                default:
                    printf("DEFAULT\r\n");
                    break;
            }

            /*
            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&echoServAddr , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                          inet_ntoa(echoServAddr.sin_addr) , ntohs(echoServAddr.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                //Echo back the message that came in

                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }

            }
            */
        }
    }

    //close(connfd);
}