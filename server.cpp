#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
//*Autor: Raul Ernesto Perez Barcenas*//
//*Matricula: 148661*//
//*Version: 1.0*//
//*Asignatura: Programacion Integrativa (UACJ)*//
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     ////////
     string sensormsg,mktemp;
     char csum[4];
     int cksum;
     ////////
     if (argc < 2)
     {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);
     while ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) >= 0)
     {
       cksum=0;
       if (newsockfd < 0)
            error("ERROR on accept");
       bzero(buffer,256);
       n = read(newsockfd,buffer,255);
       if (n < 0) error("ERROR reading from socket");
       printf("Sensor: %s\n",buffer);
       //////////
       sensormsg = buffer;
       for (int x=0;x<31;x++)//checksum del mensaje de 31 bytes.
       {
         cksum = cksum + (int)sensormsg[x];
       }
       std::string compare = to_string(cksum);
       mktemp = sensormsg.substr(31,4);
       int l;
       char ex[compare.size() + 1],ex2[mktemp.size() + 1];
       strcpy(ex, compare.c_str());
       strcpy(ex2, mktemp.c_str());
       l = strcmp(ex,ex2);
       if(l==0) //Comparacion de checksums (extraido del mensaje y comparado).
       {
         cout<<"Checksum correcto. El mensaje sera almacenado en CSV."<<endl;
       }
       else
       {
         cout<<"Checksum incorrecto. El mensaje no sera almacenado."<<endl;
       }
       ///////////
       n = write(newsockfd,"I got your message",18);
       if (n < 0) error("ERROR writing to socket");
     }
     close(newsockfd);
     close(sockfd);
     return 0;
}
