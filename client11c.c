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
#include <pthread.h>
#include <limits.h>

#define PORT 10010
#define MAXLINE 1024
#define MSIZE 10000

//driver code
int main(int argc, char **argv)
{
	int sockfd;
	char *buffer = malloc(sizeof(char) * MAXLINE);
	char *hello = malloc(sizeof(char) * MAXLINE);
	struct sockaddr_in servaddr;
	struct hostent *he;
	char *ip = malloc(sizeof(char) * 100);
	struct in_addr **addr_list;
	struct timeval start[MSIZE], end[MSIZE];
	double secs = 0;
	int fd[2];
	pipe(fd);
	int lost[MSIZE];
	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 5000;
	//child  pid_t forkstatus;

	// Creating socket file descriptor(sockfd)
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	if ((he = gethostbyname(argv[1])) == NULL)
	{
		printf("host name is not found");
	}
	addr_list = (struct in_addr **)he->h_addr_list;
	strcpy(ip, inet_ntoa(*addr_list[0]));
	printf("server ip address is %s \n", ip);

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(ip);

	int n, k = 0, len;
	printf("Client in processing\n");
	int i, j;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);
	int forkstatus = fork();
	if (forkstatus == 0)
	{
		close(fd[1]);
		for (j = 0; j < MSIZE; j++)
		{
			if ((n = recv(sockfd, (char *)buffer, MAXLINE, 0)) == -1)
			{
				printf("Read Error on Client");
			}
			else
			{
				buffer[n] = 0;
				int i = atoi(buffer) - 1;
				gettimeofday(&end[i], NULL);
				printf("time at which %d is recieved is %d \n", i + 1, end[i].tv_usec);

				printf("Echo from Server : %s\n", buffer);
			}
		}
		int start[MSIZE];
		int p = read(fd[0], start, sizeof(start));
		int max = 0, min = INT_MAX;
		int sumavg = 0, avg;
		for (i = 0; i < p / 4; i++)
		{
			if (start[i] == 0 || end[i].tv_usec == 0)
			{
				printf("%d is lost \n", i + 1);
				lost[k++] = i + 1;
			}
			else
			{
				if ((end[i].tv_usec - start[i]) < 0)
				{
					continue;
				}
				sumavg = sumavg + end[i].tv_usec - start[i];
				if ((end[i].tv_usec - start[i]) < min)
				{
					min = (end[i].tv_usec - start[i]);
				}
				if ((end[i].tv_usec - start[i]) > max)
				{
					max = (end[i].tv_usec - start[i]);
				}
			}
		}
		avg = sumavg / (MSIZE - k);
		if (k > 0)
		{
			printf("echoes lost are \n");
			for (i = 0; i < k; i++)
			{
				printf("%d\t", lost[i]);
			}
		}
		printf("\n");
		printf("Highest RTT in microseconds is  = %d \n Lowest RTT in microseconds is =%d  \n Average RTT in microseconds is = %d \n", max, min, avg);
	}
	else
	{
		close(fd[0]);
		int sendtime[MSIZE];
		for (i = 0; i < MSIZE; i++)
		{
			sprintf(hello, "%d", i + 1);
			sendto(sockfd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
			gettimeofday(&start[i], NULL);
			printf("Message is sucessfully sent to server .\n");
			printf( "taken time which %d  message is sent is %d \n", i + 1, start[i].tv_usec);
			sendtime[i] = start[i].tv_usec;
		}
		write(fd[1], sendtime, sizeof(sendtime));
	}

	close(sockfd);
	return 0;
}
