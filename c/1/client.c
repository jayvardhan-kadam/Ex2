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
	"First\n",
	"Second\n",
	"Third\n"
};

int main(int argc, char* argv[])
{
	FILE *fp;
	char hostname[128];
	int c, i, s, port;
	struct hostent *hp;
	struct sockaddr_in sin;

	if(argc <= 2) {
	  perror("host and port required");
	  exit(1);		
	}

	if((port = atoi(argv[2])) == 0) {
	  perror("no port");
	  exit(1);
	}

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
	
	for (i = 0; i < NUMSTR; i++) {
		while ((c = fgetc(fp)) != EOF) {
			putchar(c);				
			if(c == '\n')
				break;
		}
	}

	for (i = 0; i < NUMSTR; i++){
		send(s, strs[i], strlen(strs[i]), 0);
	}
	
	close(s);
	return 0;
}

 
