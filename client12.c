#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 10020 /*port*/

int main(int argc, char **argv) 
{
 int sockfd, n;
 struct sockaddr_in servaddr; 
 unsigned char buf[9], f[14];
 uint32_t a,b,ai,bi,answer;
 char c,e;
 
 
 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=2) {
  perror("Usage: TCPClient <IP address of the server"); 
  exit(1);
 }
 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
	
    
 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }
	
 
 //Connection of the client to the socket 
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
   
while (1) { 

   do { 
   printf("\nEnter a operator (+,-,x,/): ");
   scanf(" %c", &c);
   }while (c != '+' && c != '-' && c != 'x' && c != '/' || c == '\\'); // '\' breaks this code
  
   printf("\nEnter a number: ");
   scanf(" %u", &a);
   printf("\nEnter a number: ");
   scanf(" %u", &b);
   
  
buf[0] = c & 0XFF;
buf[1] = a >> 24 & 0xFF;
buf[2] = a >> 16 & 0XFF;
buf[3] = a >> 8 & 0xFF;
buf[4] = a & 0xff ;
buf[5] = b >> 24 & 0xFF;
buf[6] = b >> 16 & 0XFF;
buf[7] = b >> 8 & 0xFF;
buf[8] = b  & 0xFF;
 

   send(sockfd, buf, sizeof(buf), 0);
		
  if ((n=recv(sockfd, f, MAXLINE,0)) == 0){
   //error: server terminated prematurely
   perror("The server disconnted"); 
   exit(4);
  }
  
  printf("%s", "\nString received from the server: ");  
  ai = (f[1]<<24)+(f[2]<<16)+(f[3]<<8)+(f[4]);
  bi = (f[5]<<24)+(f[6]<<16)+(f[7]<<8)+(f[8]);
  c= f[0];
  answer = (f[9]<<24)+(f[10]<<16)+(f[11]<<8)+(f[12]);
  e = f[13];  
  f[n]=0;
  printf("Message From Server is : %c%u%u%u%c\n", c,ai,bi,answer,e); // exactly from server
  if (e == '1') {
      printf("%s %u%c%u = %u\n", "Answer Calculated is: ", ai,c,bi,answer); // if is valid
  } else {
 printf("%s", "Invalid result."); // if is not valid
 }
 }
 exit(0);
}



