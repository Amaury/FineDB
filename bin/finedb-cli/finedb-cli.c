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
#include "protocol.h"

/** @define LTRIM Move forward a pointer to skip spaces. */
#define LTRIM(pt)	while(*pt && IS_SPACE(*pt)) ++pt;

/**
 * Options structure.
 * @field	fd		Socket descriptor.
 * @field	dbname		Database's name.
 * @field	sync		Synchronized option.
 */
typedef struct cli_s {
	int	fd;
	char	*dbname;
	ybool_t	sync;
} cli_t;

// function declarations
void command_help(void);
void command_get(cli_t *cli, char *pt);
void command_put(cli_t *cli, char *pt);
void command_del(cli_t *cli, char *pt);
void command_use(cli_t *cli, char *pt);
void command_sync(cli_t *cli);
void command_async(cli_t *cli);

int main(int argc, char *argv[]) {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	cli_t *cli;
	char buff[8196], *cmd;
	size_t bufsz;
	
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
	cli = YMALLOC(sizeof(cli_t));
	cli->fd = sockfd;
	// main loop
	for (; ; ) {
		char *pt, *cmd;

		bzero(buff, sizeof(buff));
		printf("%c[1m%s >%c[0m ", 27, (cli->dbname ? cli->dbname : "default"), 27);
		fflush(stdout);
		bufsz = read(0, buff, 8192);
		buff[bufsz - 1] = '\0';

		pt = buff;
		LTRIM(pt);
		cmd = pt;
		while (*pt && !IS_SPACE(*pt))
			++pt;
		*pt++ = '\0';
		LTRIM(pt);
		// command management
		if (!strcasecmp(cmd, "exit") || !strcasecmp(cmd, "quit"))
			exit(0);
		if (!strcasecmp(cmd, "help") || cmd[0] == '?')
			command_help();
		else if (!strcasecmp(cmd, "put"))
			command_put(cli, pt);
		else if (!strcasecmp(cmd, "get"))
			command_get(cli, pt);
		else if (!strcasecmp(cmd, "del"))
			command_del(cli, pt);
		else if (!strcasecmp(cmd, "use"))
			command_use(cli, pt);
		else if (!strcasecmp(cmd, "sync"))
			command_sync(cli);
		else if (!strcasecmp(cmd, "async"))
			command_async(cli);
	}
	return (0);
}

/* Defines the used database. */
void command_use(cli_t *cli, char *pt) {
	YFREE(cli->dbname);
	if (!strlen(pt) || !strcasecmp(pt, "default")) {
		// use default database
		printf("%c[2mUse default database%c[0m\n", 27, 27);
	} else {
		// set a new database
		cli->dbname = strdup(pt);
		printf("%c[2mUse '%s' database%c[0m\n", 27, pt, 27);
	}
}

/* Set synchronous mode. */
void command_sync(cli_t *cli) {
	cli->sync = YTRUE;
	printf("%c[2mSet synchronous mode.%c[0m\n", 27, 27);
}
/* Set asynchronous mode. */
void command_async(cli_t *cli) {
	cli->sync = YFALSE;
	printf("%c[2mSet asynchronous mode.%c[0m\n", 27, 27);
}

/* Show usage. */
void command_help() {
	printf("%c[2m"
		"Usage:    finedb-cli hostname\n"
		"Commands:\n"
		"    put \"key\" \"data\"\n"
		"    get \"key1\"\n"
		"    del \"key1\"\n"
		"    sync\n"
		"    async\n"
		"    quit\n"
		"%c[0m", 27, 27);
}

/* Put a key/value in database. */
void command_put(cli_t *cli, char *pt) {
	char *pt2, *key, *data;
	char *buffer, c;
	size_t sz, offset, length;
	uint16_t length16;
	uint32_t length32;

	LTRIM(pt);
	if (*pt != '"') {
		printf("%c[2mBad key format (no quote)\n%c[0m", 27, 27);
		return;
	}
	pt++;
	if (!*pt) {
		printf("%c[2mBad key\n%c[0m", 27, 27);
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf("%c[2mBad key format (no trailing quote)\n%c[0m", 27, 27);
		return;
	}
	*pt2 = '\0';
	pt2++;
	if (!*pt2) {
		printf("%c[2mMissing data\n%c[0m", 27, 27);
		return;
	}
	*pt2 = '\0';
	data = pt2 + 1;
	LTRIM(data);
	if (*data != '"') {
		printf("%c[2mBad data format (no quote)\n%c[0m", 27, 27);
		return;
	}
	data++;
	if (!*data) {
		printf("%c[2mMissing data\n%c[0m", 27, 27);
		return;
	}
	pt2 = data + strlen(data) - 1;
	if (*pt2 != '"') {
		printf("%c[2mBad data format (no trailing quote)\n%c[0m", 27, 27);
		return;
	}
	*pt2 = '\0';

	// create sending buffer
	sz = 1;
	if (cli->dbname)
		sz += 1 + strlen(cli->dbname);
	sz += sizeof(uint16_t) + strlen(key);
	sz += sizeof(uint32_t) + strlen(data);
	buffer = YMALLOC(sz);
	// set the code byte
	buffer[0] = PROTO_PUT;
	if (cli->sync)
		buffer[0] = REQUEST_ADD_SYNC(buffer[0]);
	if (cli->dbname)
		buffer[0] = REQUEST_ADD_DBNAME(buffer[0]);
	buffer[0] = REQUEST_ADD_DATA(buffer[0]);
	// dbname
	offset = 1;
	if (cli->dbname) {
		length = strlen(cli->dbname);
		buffer[offset] = (char)length;
		offset++;
		memcpy(&buffer[offset], cli->dbname, length);
		offset += length;
	}
	// key
	length = strlen(key);
	length16 = htons((uint16_t)length);
	memcpy(&buffer[offset], &length16, sizeof(length16));
	offset += sizeof(length16);
	memcpy(&buffer[offset], key, length);
	offset += length;
	// data
	length = strlen(data);
	length32 = htonl((uint32_t)length);
	memcpy(&buffer[offset], &length32, sizeof(length32));
	offset += sizeof(length32);
	memcpy(&buffer[offset], data, length);

	{
		size_t n;
		for (n = 0; n < sz; n++)
			printf("%02x ", buffer[n]);
		printf("\n");
	}

	// send data
	write(cli->fd, buffer, sz);
	YFREE(buffer);

	// get response
	read(cli->fd, &c, 1);
	if (c == RESP_OK)
		printf("%c[2mOK%c[0m\n", 27, 27);
	else
		printf("%c[2mERROR: %s%c[0m\n", 27,
		       (c == RESP_PROTO ? "protocol" :
		        (c == RESP_SERVER_ERR ? "server" :
		         (c == RESP_NO_DATA ? "no data" : "unknown"))), 27);
}

void command_get(cli_t *cli, char *pt) {
	char *pt2, *key, *data;
	char *buffer, c, buff[5];
	size_t sz, offset, length;
	uint16_t length16;
	uint32_t length32;

	LTRIM(pt);
	if (*pt != '"') {
		printf("%c[2mBad key format (no quote)\n%c[0m", 27, 27);
		return;
	}
	pt++;
	if (!*pt) {
		printf("%c[2mBad key\n%c[0m", 27, 27);
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf("%c[2mBad key format (no trailing quote)\n%c[0m", 27, 27);
		return;
	}
	*pt2 = '\0';
	printf("GET '%s'\n", key);

	// create sending buffer
	sz = 1;
	if (cli->dbname)
		sz += 1 + strlen(cli->dbname);
	sz += sizeof(uint16_t) + strlen(key);
	buffer = YMALLOC(sz);
	// set the code byte
	buffer[0] = PROTO_GET;
	if (cli->sync)
		buffer[0] = REQUEST_ADD_SYNC(buffer[0]);
	if (cli->dbname)
		buffer[0] = REQUEST_ADD_DBNAME(buffer[0]);
	// dbname
	offset = 1;
	if (cli->dbname) {
		length = strlen(cli->dbname);
		buffer[offset] = (char)length;
		offset++;
		memcpy(&buffer[offset], cli->dbname, length);
		offset += length;
	}
	// key
	length = strlen(key);
	length16 = htons((uint16_t)length);
	memcpy(&buffer[offset], &length16, sizeof(length16));
	offset += sizeof(length16);
	memcpy(&buffer[offset], key, length);

	{
		size_t n;
		for (n = 0; n < sz; n++)
			printf("%02x ", buffer[n]);
		printf("\n");
	}

	// send data
	write(cli->fd, buffer, sz);
	YFREE(buffer);

	// get response
	read(cli->fd, &buff, 5);
	if (RESPONSE_CODE(buff[0]) != RESP_OK) {
		printf("%c[2mERROR: %s%c[0m\n", 27,
		       (RESPONSE_CODE(buff[0]) == RESP_PROTO ? "protocol" :
		        (RESPONSE_CODE(buff[0]) == RESP_SERVER_ERR ? "server" :
		         (RESPONSE_CODE(buff[0]) == RESP_NO_DATA ? "no data" : "unknown"))), 27);
		return;
	}
	printf("%c[2mOK%c[0m\n", 27, 27);
	memcpy(&length32, &buff[1], sizeof(length32));
	length = (size_t)ntohl(length32);
	buffer = YMALLOC(length + 1);
	read(cli->fd, buffer, length);
	printf("%c[2m%s%c[0m\n", 27, buffer, 27);
	YFREE(buffer);
}

void command_del(cli_t *cli, char *pt) {

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
