#include <iostream>
#include<fstream>
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
     string sensormsg,sensormsg2,mktemp,mktemp2,opcion,opcion2;
     int cksum,cksum2;
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
       cksum2=0;
       sensormsg="";
       sensormsg2="";
       mktemp="";
       mktemp2="";
       opcion="";
       opcion2="";
       if (newsockfd < 0)
            error("ERROR on accept");
       bzero(buffer,256);
       n = read(newsockfd,buffer,255);
       if (n < 0)
            error("ERROR reading from socket");
       cout<<buffer<<endl;
       sensormsg = buffer;
       sensormsg2 = buffer;
       //////
       if(buffer[0]=='U')
       {
         opcion = sensormsg.substr(0,1);
       }
       if(buffer[0]=='R')
       {
          opcion2 = sensormsg2.substr(0,1);
       }
       //////
       if(opcion=="U")
       {
         sensormsg = buffer;
         for (int x=0;x<31;x++)//checksum del mensaje de 31 bytes.
         {
           cksum = cksum + (int)sensormsg[x];
         }
         std::string compare = to_string(cksum);
         mktemp = sensormsg.substr(31,4);
         //Para la opcion de Update.
         int l;
         char ex[compare.size() + 1],ex2[mktemp.size() + 1];
         strcpy(ex, compare.c_str());
         strcpy(ex2, mktemp.c_str());
         l = strcmp(ex,ex2);
           if(l==0) //Comparacion de checksums (extraido del mensaje y comparado).
           {
             cout<<"Checksum correcto. El mensaje sera almacenado en Update CSV."<<endl;
             fstream fout;
             fout.open("medicion.csv", ios::out | ios::app);
             string s_tipo,s_nombresensor,s_dato,s_fecha,s_tiempo,s_checksum;
             s_tipo=sensormsg.substr(0,1);
             s_nombresensor=sensormsg.substr(1,8);
             s_dato=sensormsg.substr(9,8);
             s_tiempo=sensormsg.substr(17,6);
             s_fecha=sensormsg.substr(23,8);
             s_checksum=sensormsg.substr(31,4);
             fout<< s_tipo << ","
             << s_nombresensor << ","
             << s_dato << ","
             << s_tiempo << ","
             << s_fecha << ","
             << s_checksum
             << "\n";
           }
           else
           {
             cout<<"Checksum incorrecto. El mensaje no sera almacenado en Update CSV."<<endl;
           }
       }
      //Para la opcion de Request.
      if(opcion2=="R")
      {
        sensormsg2 = buffer;
        for (int x=0;x<17;x++)//checksum del mensaje de 17 bytes.
        {
          cksum2 = cksum2 + (int)sensormsg2[x];
        }
        std::string compare2 = to_string(cksum2);
        mktemp2 = sensormsg2.substr(17,4);
        int m;
        char e[compare2.size() + 1];
        char e2[mktemp2.size() + 1];
        strcpy(e, compare2.c_str());
        strcpy(e2, mktemp2.c_str());
        m = strcmp(e,e2);
          if(m==0) //Comparacion de checksums (extraido del mensaje y comparado).
          {
            cout<<"Checksum correcto. El mensaje sera almacenado en Request CSV."<<endl;
            fstream fot;
            fot.open("transaccion.csv", ios::out | ios::app);
            string S_tipo,S_observer,S_sensorname,S_checksum;
            S_tipo=sensormsg2.substr(0,1);
            S_observer=sensormsg2.substr(1,8);
            S_sensorname=sensormsg2.substr(9,8);
            S_checksum=sensormsg.substr(17,4);
            fot<< S_tipo << ","
            << S_observer << ","
            << S_sensorname << ","
            << S_checksum
            << "\n";
          }
          else
          {
            cout<<"Checksum incorrecto. El mensaje no sera almacenado en Request CSV."<<endl;
          }
      }
       ///////////
       n = write(newsockfd,"-Mensaje correcto-",18);
       for(int o=0;o<256;o++)
       {
        buffer[o]='0';
       }//Limpieza de buffer.
       if (n < 0) error("ERROR writing to socket");
     }
     close(newsockfd);
     close(sockfd);
     return 0;
}
