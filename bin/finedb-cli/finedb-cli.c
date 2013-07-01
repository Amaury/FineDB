/**
 * finedb-cli
 * Command-line client interface to a FineDB server.
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ydefs.h"

#define LTRIM(pt)	while(*pt && IS_SPACE(*pt)) ++pt;

// function declarations
void command_help(void);
void command_get(int fd, char *pt);
void command_put(int fd, char *pt);
void command_del(int fd, char *pt);

int main(int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buff[8196], *cmd;
	size_t bufsz;
	//uint16_t name_len;
	//uint32_t *pdata_len, data_len;
	//int i, size;

	if (argc == 1) {
		printf("Usage: finedb-cli hostname\n");
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
	// main loop
	for (; ; ) {
		char *pt, *cmd;

		printf("%c[1m>%c[0m ", 27, 27);
		fflush(stdout);
		bufsz = read(0, buff, 8196);
		buff[bufsz - 1] = '\0';

		pt = buff;
		LTRIM(pt);
		cmd = pt;
		while (*pt && !IS_SPACE(*pt))
			++pt;
		*pt++ = '\0';
		LTRIM(pt);
		// command management
		printf("cmd = '%s'\n", cmd);
		if (!strcasecmp(cmd, "exit") || !strcasecmp(cmd, "quit"))
			exit(0);
		if (!strcasecmp(cmd, "help") || cmd[0] == '?')
			command_help();
		else if (!strcasecmp(cmd, "put"))
			command_put(fd, pt);
		else if (!strcasecmp(cmd, "get"))
			command_get(fd, pt);
		else if (!strcasecmp(cmd, "del"))
			command_del(fd, pt);
	}
	return (0);
}

void command_help() {
	printf("%c[2m"
		"Usage:    finedb-cli hostname\n"
		"Commands:\n"
		"    put \"key\" \"data\"\n"
		"    get \"key1\"\n"
		"    del \"key1\"\n"
		"    quit\n"
		"%c[0m", 27, 27);
	exit(0);
}

void command_put(int fd, char *pt) {
	char *pt2, *key, *data;

	LTRIM(pt);
	if (*pt != '"') {
		printf("Bad key format (no quote)\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf("Bad key\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf("Bad key format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';
	pt2++;
	if (!*pt2) {
		printf("Missing data\n");
		return;
	}
	*pt2 = '\0';
	data = pt2 + 1;
	LTRIM(data);
	if (*data != '"') {
		printf("Bad data format (no quote)\n");
		return;
	}
	data++;
	if (!*data) {
		printf("Missing data\n");
		return;
	}
	pt2 = data + strlen(data) - 1;
	if (*pt2 != '"') {
		printf("Bad data format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';
	printf("PUT '%s' => '%s'\n", key, data);
}

void command_get(int fd, char *pt) {

}

void command_del(int fd, char *pt) {

}

#if 0

	// loop on key/data pairs
	for (i = 2; i < argc; i += 1) {
		bufsz = 0;
		/* create message */
		// command
		buff[0] = 2;
		bufsz += 1;
		// name length
		size = strlen(argv[i]) + 1;
		name_len = htons(size);
		memcpy(&buff[1], &name_len, sizeof(uint16_t));
		bufsz += sizeof(uint16_t);
		// name
		memcpy(&buff[bufsz], argv[i], size);
		bufsz += size;
		/* write message */
		write(sockfd, buff, bufsz);
		/* get response */
		size = read(sockfd, buff, 8196);
		cmd = buff;
		if (*cmd != 64) {
			size_t offset;
			char *c;
			
			printf("ERROR (%d) for key '%s'\n", (int)*cmd, argv[i]);
			printf("\t'");
			for (offset = 0; offset < size; ++offset) {
				c = &buff[offset];
				if (*c >= ' ' && *c <= '~')
					printf("%c", *c);
				else
					printf("%x", (int)*c);
				if (offset == 0 || offset == 4)
					printf(" ");
			}
			printf("'\n");
			continue;
		}
		pdata_len = (uint32_t*)&buff[1];
		data_len = ntohl(*pdata_len);
		buff[data_len + 6] = '\0';
		printf("OK key '%s' (%d) => '%s'\n", argv[i], data_len, &buff[5]);
	}
}
#endif
