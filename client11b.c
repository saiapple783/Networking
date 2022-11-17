// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>

#define PORT 10010
#define MAXLINE 1024

int main(int argc, char **argv)
{
	int sockfd;
	char *buffer = malloc(sizeof(char) * MAXLINE);
	char *hello = malloc(sizeof(char) * MAXLINE);
	struct sockaddr_in servaddr;
	struct hostent *he;
	char *ip = malloc(sizeof(char) * 100);
	struct in_addr **addr_list;
	struct timeval start, end;
	double secs = 0;

	// Creation code of  socket file descriptor(sockfd)
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Error in creation of socket");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	if ((he = gethostbyname(argv[1])) == NULL)
	{
		printf("Invalid Host name");
	}
	addr_list = (struct in_addr **)he->h_addr_list;
	strcpy(ip, inet_ntoa(*addr_list[0]));
	printf("server ip address is %s \n", ip);

	// server Info
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip);

	int n, len;
	printf("Enter the messages to sent to server\n");
	while (fgets(hello, MAXLINE, stdin) != NULL)
	{

		sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
		gettimeofday(&start, NULL);
		printf("Message that sent from client to server .\n");
		printf("time taken to send message %d \n", start.tv_usec);

		if ((n = recvfrom(sockfd, (char *)buffer, MAXLINE, 0, (struct sockaddr *)&servaddr, &len)) <= 0)
		{
			printf("Read Error on Client");
		}
		gettimeofday(&end, NULL);

		buffer[n] = '\0';
		printf("Echo recieved by Client from Server : %s\n", buffer);
		printf("time taken to reciecve by client from server %d  \n", end.tv_usec);
		secs = (double)(end.tv_usec - start.tv_usec);
		printf("\n Round Trip Time(RTT) in microseconds is : %f \n", secs);
		printf("\n Enter the new message to be sent to server or press ctrl+c to exit the loop \n");
	}
	close(sockfd);
	return 0;
}
