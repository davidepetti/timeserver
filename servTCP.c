/* servTCP.c   riceve stringa, trasla i caratteri di 2, e 
               rispedisce al mittente
   su SunOS compilare con gcc -o servTCP.c -lsocket -lnsl servTCP.c
   su linux               gcc -o servTCP servTCP.c                   

   eseguire ad esempio su 130.136.2.7 lanciando la seguente riga di comandi:
   servTCP 5001 
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <time.h>

#define SOCKET_ERROR   ((int)-1)
#define SIZEBUF 10000

void usage(void)
{  printf ("usage: servTCP LOCAL_PORT_NUMBER\n"); exit(1); }

int main(int argc, char *argv[])
{
  #define MAXSIZE 100 
  struct sockaddr_in Local, Cli;
  char string_remote_ip_address[100];
  short int remote_port_number, local_port_number;
  int socketfd, newsocketfd, OptVal, len, ris;
  int n;
  char buf[MAXSIZE];
  time_t ticks;
	
  if(argc!=2) { printf ("necessario 1 parametri\n"); usage(); exit(1);  }
  else {
    local_port_number = atoi(argv[1]);
  }

  /* get a stream socket */
  printf ("socket()\n");
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd == SOCKET_ERROR) {
    printf ("socket() failed, Err: %d \"%s\"\n", errno,strerror(errno));
    exit(1);
  }

  /* avoid EADDRINUSE error on bind() */
  OptVal = 1;
  printf ("setsockopt()\n");
  ris = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char *)&OptVal, sizeof(OptVal));
  if (ris == SOCKET_ERROR)  {
    printf ("setsockopt() SO_REUSEADDR failed, Err: %d \"%s\"\n", errno,strerror(errno));
    exit(1);
  }

  /* name the socket */
  memset ( &Local, 0, sizeof(Local) );
  Local.sin_family		=	AF_INET;
  /* indicando INADDR_ANY viene collegato il socket all'indirizzo locale IP     */
  /* dell'interaccia di rete che verr� utilizzata per inoltrare il datagram IP  */
  Local.sin_addr.s_addr	=	htonl(INADDR_ANY);         /* wildcard */
  Local.sin_port		=	htons(local_port_number);
  printf ("bind()\n");
  ris = bind(socketfd, (struct sockaddr*) &Local, sizeof(Local));
  if (ris == SOCKET_ERROR)  {
    printf ("bind() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    exit(1);
  }

  printf ("listen()\n");
  ris = listen(socketfd, 10 );
  if (ris == SOCKET_ERROR)  {
    printf ("listen() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    exit(1);
  }

  /* wait for connection request */
  memset ( &Cli, 0, sizeof(Cli) );
  len=sizeof(Cli);
  printf ("accept()\n");
  newsocketfd = accept(socketfd, (struct sockaddr*) &Cli, &len);
  if (newsocketfd == SOCKET_ERROR)  {
    printf ("accept() failed, Err: %d \"%s\"\n",errno,strerror(errno));
    exit(1);
  }

  /* acquisizione e conversione del tempo */
  ticks = time(NULL);
  snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
  /* scrittura */
  printf ("write()\n", strlen(buf));
  n = write(newsocketfd, buf, strlen(buf));
  if(n<0) {
    char msgerror[1024];
    sprintf(msgerror,"write() failed [err %d] ",errno);
    perror(msgerror); return(1);
  }
  
  /* chiusura */
  printf ("close()\n");
  close(newsocketfd);
  close(socketfd);

  return(0);
}

