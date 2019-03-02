#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
using namespace std;
//*Autor: Raul Ernesto Perez Barcenas*//
//*Matricula: 148661*//
//*Version: 1.0*//
//*Asignatura: Programacion Integrativa (UACJ)*//
//Ambito global.
char tipo;
string nombresensor,nombresensor2,tiempo,fecha,nombreobservador;
string mensaje, mensaje2;
int checksum, checksum2;
float datos;
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
void Update()
{
  tipo = 'U';
  std::string x_str2(1, tipo);
  string x_str;
  do
  {
    cout<<"Ingresa el nombre del sensor:"<<endl;
    cin>>nombresensor;
    cin.ignore();
  }while (nombresensor.length()!=8);
  char array[8];
  cout<<"Ingresa el dato de medicion:"<<endl;
  cin>>datos;
  cin.ignore();
  if(datos>=-99999.9 && datos<=999999.9)
  {
    x_str = std::to_string(datos);
    for(int i=0;i<8;i++) //Conversion a Char array.
    {
      array[i] = x_str[i];
    }
    for(int i=0;i<8;i++) //Conversion a string longitud 8.
    {
      x_str[i]=array[i];
    }
  }
  else
  {
    error("No ingresaste un valor valido, prueba nuevamente.");
    exit(0);
  }
  do
  {
    cout<<"Ingresa la fecha:"<<endl;
    cin>>fecha;
    cin.ignore();
  }while (fecha.length()!=8);
  do
  {
    cout<<"Ingresa el tiempo:"<<endl;
    cin>>tiempo;
    cin.ignore();
  }while (tiempo.length()!=6);
  mensaje=x_str2+nombresensor+x_str+fecha+tiempo;
  for(int y=0;y<mensaje.length();y++)
  {
    checksum = checksum + (int) mensaje[y];
  }
  std::string Schecksum = std::to_string(checksum);
  mensaje=mensaje+Schecksum;
}
void Request()
{
  tipo = 'R';
  std::string x_s(1, tipo);
  do
  {
    cout<<"Ingresa el nombre del sensor:"<<endl;
    cin>>nombresensor2;
    cin.ignore();
  }while(nombresensor2.length()!=8);
  do
  {
    cout<<"Ingresa el nombre del observante:"<<endl;
    cin>>nombreobservador;
    cin.ignore();
  }while (nombreobservador.length()!=8);
  mensaje2=x_s+nombreobservador+nombresensor2;
  for(int w=0;w<mensaje2.length();w++)
  {
    checksum2 = checksum2 + (int) mensaje2[w];
  }
  std::string Schecksum2 = std::to_string(checksum2);
  mensaje2=mensaje2+Schecksum2;
}
void menu()
{
  cout<<"Selecciona tu tipo de usuario: U (Sensor) y R (Observador): "<<endl;
  cin>>tipo;
  if(tipo=='u'||tipo=='U')
  {
    Update();
  }
  else if (tipo=='r'||tipo=='R')
  {
    Request();
  }
  else
  {
    exit(0);
  }
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    menu();
    if(tipo=='U')
    {
      bzero(buffer,256);
      char cstr[35];
  	  strcpy(cstr, mensaje.c_str());
      strncpy(buffer, cstr, 35);
    }
    else if(tipo=='R')
    {
      bzero(buffer,256);
      char cstr2[21];
  	  strcpy(cstr2, mensaje2.c_str());
      strncpy(buffer, cstr2, 21);
    }
    else
    {
      exit(0);
    }
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
