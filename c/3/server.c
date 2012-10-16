#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define BUF_SIZE 1024

struct HTTPRequest {
  char *method;
  char *path;
  char *host;
};

struct HTTPRequest *read_request(FILE *fp);
void free_request(struct HTTPRequest *request);

int main(int argc, char* argv[])
{
  FILE *fp, *requested_file;
  int s, ns, port, pid;
  char buf[BUF_SIZE];
  struct sockaddr_in sin, fsin;
  struct HTTPRequest *request;
  
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

    pid = fork();

    if (pid == 0) {
      fp = fdopen(ns, "r+");

      request = read_request(fp);

      if ((requested_file = fopen(request->path, "r")) == NULL) {
        // Not Found
        fprintf(fp, "HTTP/1.1 404 Not Found\r\n");
        fprintf(fp, "Content-Type: text/html; charset=us-ascii\r\n\r\n");
        fflush(fp);
      } else {
        fprintf(fp, "HTTP/1.1 200 OK\r\n");
        fprintf(fp, "Content-Type: text/html; charset=us-ascii\r\n\r\n");
        fflush(fp);

        while (fgets(buf, sizeof(buf), requested_file) != NULL) {
          write(ns, buf, strlen(buf));
        }
      }

      fclose(requested_file);
      close(ns);
      free_request(request);
      exit(0);
    } else {
      close(ns);
    }
  }

  close(s);

  return 0;
}

struct HTTPRequest* read_request(FILE *fp) {
  char buf[BUF_SIZE], method[BUF_SIZE], path[BUF_SIZE], host[BUF_SIZE];
  struct HTTPRequest *request;
  request = malloc(sizeof(struct HTTPRequest));

  // Read Request-Line
  fgets(buf, sizeof(buf), fp);
  sscanf(buf, "%s /%s HTTP/1.1", method, path);

  request->method = malloc(strlen(method) + 1);
  strcpy(request->method, method);

  request->path = malloc(strlen(path) + 1);
  strcpy(request->path, path);
  
  // Read Header
  fgets(buf, sizeof(buf), fp);
  sscanf(buf, "Host: %s", host);
  fflush(fp);

  request->host = malloc(strlen(host) + 1);
  strcpy(request->host, host);

  return request;
}

void free_request(struct HTTPRequest *request) {
  free(request->method);
  free(request->path);
  free(request->host);
  free(request);
}
