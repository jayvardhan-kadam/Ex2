#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int main(int argc, char* argv[])
{
  FILE *fp;
  char hostname[128], *filename, request[1024];
  int c, s, port;
  struct hostent *hp;
  struct sockaddr_in sin;

  if(argc <= 3) {
    perror("host, port and file required");
    exit(1);    
  }

  if((port = atoi(argv[2])) == 0) {
    perror("no port");
    exit(1);
  }

  filename = argv[3];

  if ((hp = gethostbyname(argv[1])) == NULL) {
    fprintf(stderr, "%s: unknown host.\n", hostname);
    exit(1);
  }

  if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("client socket()");
    exit(1);    
  }

  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr, hp-> h_length);
  
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("client connect()");
    exit(1);  
  }
  
  fp = fdopen(s, "r");

  sprintf(request, "GET /%s\n", filename);

  send(s, request, strlen(request), 0);

  while((c = fgetc(fp)) != EOF) {
    putchar(c);
  }
  
  close(s);
  return 0;
}

 
