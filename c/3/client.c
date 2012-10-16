#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[])
{
  FILE *fp;
  char request[BUF_SIZE], hostname[BUF_SIZE], path[BUF_SIZE], buf[BUF_SIZE], *port_p, *path_p;
  int s, port;
  struct hostent *hp;
  struct sockaddr_in sin;

  while(1) {
    while(fgets(buf, sizeof(buf), stdin)){
      if (buf[0] != '\n') break;
    };
    sscanf(buf, "http://%s", hostname);

    path_p = strchr(hostname, '/');
    *path_p++ = '\0';
    strcpy(path, path_p); 

    if((port_p = strchr(hostname, ':')) == NULL) {
      port = 80;
    } else {
      *port_p++ = '\0';
      port = atoi(port_p);
    }

    if ((hp = gethostbyname(hostname)) == NULL) {
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

    sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s:%d\r\n\r\n", path, hostname, port);

    send(s, request, strlen(request), 0);

    while(fgets(buf, sizeof(buf), fp) != NULL) {
      printf("%s", buf);
    }
    
    close(s);
  }

  return 0;
}

