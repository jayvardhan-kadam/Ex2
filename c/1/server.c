#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#define NUMSTR  3

char *strs[NUMSTR] = {
	"1\n",
	"2\n",
	"3\n"
};

int main(int argc, char* argv[])
{
	FILE *fp;
	int c, i, s, ns, port;
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
	
	if ((ns = accept(s, (struct sockaddr *)&fsin, &fromlen)) == -1) {
		perror("server accept()");	
		exit(1);
	}
	
	fp = fdopen(ns, "r");
	
	for (i = 0; i < NUMSTR; i++){
		send(ns, strs[i], strlen(strs[i]), 0);
	}
	
	for (i = 0; i < NUMSTR; i++) {
		while ((c = fgetc(fp)) != EOF) {
			putchar(c);				
			if(c == '\n')
				break;
		}
	}
	
	close(s);
	return 0;
}

