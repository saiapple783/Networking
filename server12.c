#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 10020 /*port*/
#define LISTENQ 8 /*maximum number of client connections */

int main (int argc, char **argv)
{
 int listenfd, connfd, n;
 socklen_t clilen;
 unsigned char buf[9];
 struct sockaddr_in cliaddr, servaddr;
 uint32_t bi,ai, answer; 
 unsigned char f[14];
 char r[64];
 char c;
 char e; 
	
 //creation of the socket
 listenfd = socket (AF_INET, SOCK_STREAM, 0);
	
 //Info of the socket address 
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
 servaddr.sin_port = htons(SERV_PORT);
	
 bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
 listen(listenfd, LISTENQ);
	
 printf("%s\n","Server is running...waiting for message requests.");
	
 for ( ; ; ) {		
  clilen = sizeof(cliaddr);
  connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen); // request is recieved
				
  while ( (n = recv(connfd, buf, MAXLINE,0)) > 0)  {
  answer = 0;
   buf[n] = 0;
   ai = (buf[1]<<24)+(buf[2]<<16)+(buf[3]<<8)+(buf[4]);
   bi = (buf[5]<<24)+(buf[6]<<16)+(buf[7]<<8)+(buf[8]);
   c= buf[0];
   e = '1';
   
   
   switch(c) { 
   
   case '+':
    answer = ai + bi;
    break;
   
   case '-':    
    answer = ai - bi;   
    break;
    
   case 'x':
    answer = ai * bi;
    break;
    
   case '/':
   if (bi != 0) { //if valid answer
    answer = ai / bi;
   } 
   else { //if invalid answer
    answer = 0;
    e = '2';
   } break;
    
    default:
      printf("%s", "Invalid operator");
      e = '2';
      }
   
   
f[0] = c & 0XFF;
f[1] = ai >> 24 & 0xFF;
f[2] = ai >> 16 & 0XFF;
f[3] = ai >> 8 & 0xFF;
f[4] = ai & 0xff ;
f[5] = bi >> 24 & 0xFF;
f[6] = bi >> 16 & 0XFF;
f[7] = bi >> 8 & 0xFF;
f[8] = bi  & 0xFF;
f[9] = answer >> 24 & 0xFF;
f[10] = answer >> 16 & 0XFF;
f[11] = answer >> 8 & 0xFF;
f[12] = answer  & 0xFF;
f[13] = e & 0xff;
   
   
   
  f[n] = 0;   
  send(connfd, f, sizeof(f), 0);
  }
			
 if (n < 0) {
  perror("Read error"); 
  exit(1);
 }
 close(connfd);
	
 }
 //close listening socket
 close(listenfd); 
}
