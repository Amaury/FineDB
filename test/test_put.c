#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buff[8196], *cmd;
	size_t bufsz;
	uint16_t name_len;
	uint32_t data_len;
	int i, size;

	if (argc == 1) {
		printf("Usage: test_put hostname key1 data1 key2 data2 ...\n");
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ((server = gethostbyname(argv[1])) == NULL) {
		fprintf(stderr, "Host error\n");
		exit(3);
	}
	bzero((char*) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(11138);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		fprintf(stderr, "Unable to connect\n");
		exit(4);
	}
	// loop on key/data pairs
	for (i = 2; i < argc; i += 2) {
		bufsz = 0;
		/* create message */
		// command
		buff[0] = 1;
		bufsz += 1;
		// name length
		size = strlen(argv[i]) + 1;
		name_len = htons(size);
		memcpy(&buff[1], &name_len, sizeof(uint16_t));
		bufsz += sizeof(uint16_t);
		// name
		memcpy(&buff[bufsz], argv[i], size);
		bufsz += size;
		// data length
		size = strlen(argv[i + 1]) + 1;
		data_len = htonl(size);
		memcpy(&buff[bufsz], &data_len, sizeof(uint32_t));
		bufsz += sizeof(uint32_t);
		// data
		memcpy(&buff[bufsz], argv[i + 1], size);
		bufsz += size;
		/* write message */
		write(sockfd, buff, bufsz);
		/* get response */
		size = read(sockfd, buff, 8196);
		cmd = buff;
		printf("Reply '%d' (size='%d')\n", (int)*cmd, size);
	}
}
