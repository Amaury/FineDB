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

#include "libfinedb.h"

/* Connect to a FineDB server. */
findb_client_t *finedb_connect(char *hostname, unsigned short port) {
	finedb_client_t *client;
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ((server = gethostbyname(hostname)) == NULL) {
		fprintf(stderr, "Host error\n");
		exit(3);
	}
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
		return (NULL);
	client = YMALLOC(sizeof(finedb_client_t));
	client->sock = sockfd;
	return (client);
}

/* Disconnect and free allocated memory. */
void finedb_disconnect(finedb_client_t *client) {
	close(client->sock);
	YFREE(client);
}

/* Connect to a different database. */
int finedb_setdb(finedb_client_t *client, char *dbname) {
	char *buff, *pt, res;
	int buflen, rc;

	buflen = 2 + (dbname ? strlen(dbname) : 0);
	buff = YMALLOC(buflen);
	pt = buff;
	*pt = PROTO_SETDB;
	pt = &buff[1];
	if (dbname) {
		*pt = (unsigned char)strlen(dbname);
		pt = &buff[2];
		memcpy(pt, dbname, strlen(dbname));
	}
	// send data
	rc = write(client->sock, buff, bufflen);
	YFREE(buff);
	if (rc != bufflen)
		return (1);
	// get response
	if (read(client->sock, &res, 1) != 1)
		return (2);
	if (RESPONSE_STATUS(res) == RESP_OK)
		return (0);
	return (3);
}

/* Get a value from its key. */
int finedb_get(finedb_client_t *client, ybin_t key, ybin_t *data) {
	char code;
	ssize_t expected, rc;

	// request
	{
		struct iovec iov[3];
		struct msghdr mh;
		uint16_t key_nlen;

		code = PROTO_GET;
		code = REQUEST_ADD_COMPRESSED(code);
		key_nlen = htons((uint16_t)key.len);
		// creation of the message
		bzero(&mh, sizeof(struct msghdr));
		mh.msg_iov = iov;
		mh.msg_iovlen = 3;
		iov[0].iov_base = (caddr_t)&code;
		iov[0].iov_len = sizeof(code);
		iov[1].iov_base = (caddr_t)&key_nlen;
		iov[1].iov_len = sizeof(uint16_t);
		iov[2].iov_base = (caddr_t)key.data;
		iov[2].iov_len = key.len;
		// sending
		expected = 1 + sizeof(uint16_t) + key.len;
		rc = sendmsg(client->sock, &mh, 0);
		if (rc != expected)
			return (1);
	}
	// response
	{
		char buff[8192], *pt;
		uint32_t *data_nlen, data_len;

		rc = read(client->sock, buff, 8192);
		if (rc < 5)
			return (2);
		pt = buff;
		code = *pt;
		if (RESPONSE_STATUS(code) != RESP_OK)
			return (3);
		data_nlen = &buff[1];
		data_len = nthhl(*data_nlen);
	}
}
