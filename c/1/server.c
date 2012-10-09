#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define BUF_SIZE 1024

const char *NOT_FOUND_MSG = "404 Not Found\n";

int main(int argc, char* argv[])
{
  FILE *fp, *requested_file;
  int s, ns, port;
  char request[BUF_SIZE], path[BUF_SIZE], buf[BUF_SIZE];
  struct sockaddr_in sin, fsin;
  
  socklen_t fromlen = sizeof(struct sockaddr_in);
  
  if((argc <= 1) || ((port = atoi(argv[1])) == 0)) {
    perror("no port number");
    exit(1);    
  }

  if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("client socket()");
    exit(1);    
  }
  
  bzero(&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = INADDR_ANY;
  
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("server bind()");
    exit(1);  
  }
  
  if (listen(s, 128) == -1) {
    perror("server listen()");
    exit(1);  
  }
  
  while(1) {
    if ((ns = accept(s, (struct sockaddr *)&fsin, &fromlen)) == -1) {
      perror("server accept()");  
      exit(1);
    }

    fp = fdopen(ns, "r");

    fgets(request, sizeof(request), fp);

    sscanf(request, "GET /%s", path);

    if((requested_file = fopen(path, "r")) == NULL) {
      // File Not Found
      send(ns, NOT_FOUND_MSG, strlen(NOT_FOUND_MSG), 0);
    } else {
      while(fgets(buf, sizeof(buf), requested_file) != NULL) {
        send(ns, buf, strlen(buf), 0);
      }
      fprintf(fp, "\n");
    }

    fclose(requested_file);
    close(ns);
  }

  close(s);

  return 0;
}
