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

#include "yerror.h"
#include "ydynabin.h"

#include "snappy.h"

#include "protocol.h"

#include "libfinedb.h"

/* *** Private functions *** */
static yerr_t _read_data(int fd, ydynabin_t *container, size_t size);
static int _send_key_data(finedb_client_t *client, ybool_t create_only,
                          ybool_t update_only, ybin_t key, ybin_t data);
//static int _send_incdec(finedb_client_t *client, ybool_t dec, ybin_t key, int val, int *new_value);
static int _send_simple_request(finedb_client_t *client, const char code, char *response);

/* Create a FineDB connection client. */
finedb_client_t *finedb_create(const char *hostname, unsigned short port) {
	finedb_client_t *client;

	if ((client = YMALLOC(sizeof(finedb_client_t))) == NULL)
		return (NULL);
	if ((client->hostname = strdup(hostname)) == NULL) {
		YFREE(client);
		return (NULL);
	}
	client->port = port;
	client->sock = -1;
	return (client);
}

/* Destroy a FineDB connection client. */
void finedb_delete(finedb_client_t *client) {
	finedb_disconnect(client);
	YFREE(client->hostname);
	YFREE(client);
}

/* Connect to a FineDB server. */
int finedb_connect(finedb_client_t *client) {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	// if a connection is open, close it
	finedb_disconnect(client);
	// open a new connection
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ((server = gethostbyname(client->hostname)) == NULL) {
		client->sock = -1;
		return (FINEDB_ERR_NETWORK);
	}
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy(server->h_addr, &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(client->port);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		client->sock = -1;
		return (FINEDB_ERR_NETWORK);
	}
	client->sock = sockfd;
	return (FINEDB_OK);
}

/* Disconnect and free allocated memory. */
void finedb_disconnect(finedb_client_t *client) {
	if (client->sock > -1)
		close(client->sock);
}

/* Set synchronous mode. */
void finedb_sync(finedb_client_t *client) {
	client->sync = YTRUE;
}

/* Set asynchronous mode. */
void finedb_async(finedb_client_t *client) {
	client->sync = YFALSE;
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
	rc = write(client->sock, buff, buflen);
	YFREE(buff);
	if (rc != buflen)
		return (FINEDB_ERR_NETWORK);
	// get response
	rc = read(client->sock, &res, 1);
	if (rc != 1)
		return (FINEDB_ERR_NETWORK);
	if (RESPONSE_STATUS(res) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/* Get a value from its key. */
int finedb_get(finedb_client_t *client, ybin_t key, ybin_t *value) {
	char code;
	ssize_t expected, rc;
	int retval = FINEDB_OK;

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
			return (FINEDB_ERR_NETWORK);
	}
	// response
	{
		char *pt;
		uint32_t *pdata_len, data_len;
		ydynabin_t *buff;
		void *ptr, *data = NULL;

		buff = ydynabin_new(NULL, 0, YFALSE);
		// read the response code
		if (_read_data(client->sock, buff, 1) != YENOERR) {
			retval = FINEDB_ERR_NETWORK;
			goto end_of_process;
		}
		pt = ydynabin_forward(buff, sizeof(unsigned char));
		code = *pt;
		if (RESPONSE_STATUS(code) != RESP_OK) {
			retval = FINEDB_ERR_SERVER;
			goto end_of_process;
		}
		// read the size of data
		if (_read_data(client->sock, buff, sizeof(data_len)) != YENOERR) {
			retval = FINEDB_ERR_NETWORK;
			goto end_of_process;
		}
		pdata_len = ydynabin_forward(buff, sizeof(data_len));
		data_len = ntohl(*pdata_len);
		// read data
		if (data_len > 0) {
			if (_read_data(client->sock, buff, (size_t)data_len) != YENOERR) {
				retval = FINEDB_ERR_NETWORK;
				goto end_of_process;
			}
			ptr = ydynabin_forward(buff, (size_t)data_len);
			if ((data = YMALLOC((size_t)data_len)) == NULL) {
				retval = FINEDB_ERR_MEMORY;
				goto end_of_process;
			}
			memcpy(data, ptr, (size_t)data_len);
			// compressed data?
			if (REQUEST_HAS_COMPRESSED(code)) {
				size_t unzip_len;
				char *unzip_data;

				snappy_uncompressed_length(data, data_len, &unzip_len);
				if ((unzip_data = YMALLOC(unzip_len)) == NULL) {
					YFREE(data);
					retval = FINEDB_ERR_MEMORY;
					goto end_of_process;
				}
				if (snappy_uncompress(data, data_len, unzip_data)) {
					YFREE(data);
					YFREE(unzip_data);
					retval = FINEDB_ERR_ZIP;
					goto end_of_process;
				}
				YFREE(data);
				data = unzip_data;
				data_len = unzip_len;
			}
		}
		// result
		value->len = data_len;
		value->data = data;
end_of_process:
		ydynabin_delete(buff);
	}
	return (retval);
}

/* Delete a velue from database. */
int finedb_del(finedb_client_t *client, ybin_t key) {
	char code;
	ssize_t expected, rc;

	// request
	{
		struct iovec iov[3];
		struct msghdr mh;
		uint16_t key_nlen;

		code = PROTO_DEL;
		if (client->sync)
			code = REQUEST_ADD_SYNC(code);
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
			return (FINEDB_ERR_NETWORK);
	}
	// response
	rc = read(client->sock, &code, 1);
	if (rc != 1)
		return (FINEDB_ERR_NETWORK);
	if (RESPONSE_STATUS(code) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/* Put a key/value in the database. */
int finedb_put(finedb_client_t *client, ybin_t key, ybin_t data) {
	return (_send_key_data(client, YFALSE, YFALSE, key, data));
}

/* Add a new key in the database. */
int finedb_add(finedb_client_t *client, ybin_t key, ybin_t data) {
	return (_send_key_data(client, YTRUE, YFALSE, key, data));
}

/* Update a key in the database. */
int finedb_update(finedb_client_t *client, ybin_t key, ybin_t data) {
	return (_send_key_data(client, YFALSE, YTRUE, key, data));
}

#if 0
/* Increment a value. */
int finedb_inc(finedb_client_t *client, ybin_t key, int increment, int *new_value) {
	return (_send_incdec(client, YFALSE, key, increment, new_value));
}

/* Decrement a value. */
int finedb_dec(finedb_client_t *client, ybin_t key, int increment, int *new_value) {
	return (_send_incdec(client, YTRUE, key, increment, new_value));
}
#endif /* 0 */

/* Start a transaction. */
int finedb_start(finedb_client_t *client) {
	char code;
	int rc;

	rc = _send_simple_request(client, PROTO_START, &code);
	if (rc)
		return (rc);
	if (RESPONSE_STATUS(code) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/* Stop a transaction. */
int finedb_stop(finedb_client_t *client) {
	char code;
	int rc;

	rc = _send_simple_request(client, PROTO_STOP, &code);
	if (rc)
		return (rc);
	if (RESPONSE_STATUS(code) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/* Test a running connection. */
int finedb_ping(finedb_client_t *client) {
	char code;
	int rc;

	rc = _send_simple_request(client, PROTO_PING, &code);
	if (rc)
		return (rc);
	if (RESPONSE_STATUS(code) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/* ********************* PRIVATE FUNCTIONS **************** */
/**
 * @function	_send_simple_request
 * Send a simple request and get a simple response.
 * @param	client		Pointer to the client structure.
 * @param	code		Request code.
 * @param	response	Pointer to an interger where the response code will be copied.
 * @return	FINEDB_OK if OK.
 */
static int _send_simple_request(finedb_client_t *client, const char code, char *response) {
	char res;
	int rc;

	// send data
	rc = write(client->sock, &code, 1);
	if (rc != 1)
		return (FINEDB_ERR_NETWORK);
	// get response
	rc = read(client->sock, &res, 1);
	if (rc != 1)
		return (FINEDB_ERR_NETWORK);
	if (response)
		*response = res;
	return (FINEDB_OK);
}

#if 0
/**
 * @function	_send_incdec
 * Send an INC/DEC request to the server.
 * @param	client		Pointer to the client structure.
 * @param	dec		YFALSE for INC, YTRUE for DEC.
 * @param	key		Pointer to the key content.
 * @param	val		Increment/decrement value.
 * @param	new_value	Pointer to the integer where the new value will be copied.
 * @return 	FINEDB_OK if OK.
 */
static int _send_incdec(finedb_client_t *client, ybool_t dec, ybin_t key, int val, int *new_value) {
	char code;
	ssize_t expected, rc;
	int retval = FINEDB_OK;

	// request
	{
		struct iovec iov[4];
		struct msghdr mh;
		uint16_t key_nlen;
		uint32_t nincrement;

		code = dec ? PROTO_DEC : PROTO_INC;
		key_nlen = htons((uint16_t)key.len);
		nincrement = htonl((uint32_t)val);
		// creation of the message
		bzero(&mh, sizeof(struct msghdr));
		mh.msg_iov = iov;
		mh.msg_iovlen = 4;
		iov[0].iov_base = (caddr_t)&code;
		iov[0].iov_len = sizeof(code);
		iov[1].iov_base = (caddr_t)&key_nlen;
		iov[1].iov_len = sizeof(uint16_t);
		iov[2].iov_base = (caddr_t)key.data;
		iov[2].iov_len = key.len;
		iov[3].iov_base = (caddr_t)&nincrement;
		iov[3].iov_len = sizeof(uint32_t);
		// sending
		expected = 1 + sizeof(uint16_t) + key.len + sizeof(uint32_t);
		rc = sendmsg(client->sock, &mh, 0);
		if (rc != expected)
			return (FINEDB_ERR_NETWORK);
	}
	// response
	{
		char *pt;
		uint32_t *pnew_value;
		ydynabin_t *buff;

		buff = ydynabin_new(NULL, 0, YFALSE);
		// read the response code
		if (_read_data(client->sock, buff, 1) != YENOERR) {
			retval = FINEDB_ERR_NETWORK;
			goto end_of_process;
		}
		pt = ydynabin_forward(buff, sizeof(unsigned char));
		code = *pt;
		if (RESPONSE_STATUS(code) != RESP_OK) {
			retval = FINEDB_ERR_SERVER;
			goto end_of_process;
		}
		// read the size of data
		if (_read_data(client->sock, buff, sizeof(uint32_t)) != YENOERR) {
			retval = FINEDB_ERR_NETWORK;
			goto end_of_process;
		}
		pnew_value = ydynabin_forward(buff, sizeof(uint32_t));
		if (new_value)
			*new_value = (int)ntohl(*pnew_value);
end_of_process:
		ydynabin_delete(buff);
	}
	return (retval);
}
#endif /* 0 */

/**
 * @function	_send_key_data
 * Send a PUT/ADD/UPDATE request to the server.
 * @param	client		Pointer to the client structure.
 * @param	create_only	YTRUE for an ADD request.
 * @param	update_only	YTRUE for an UPDATE request.
 * @param	key		Pointer to the key content.
 * @param	data		Pointer to the data content.
 * @return 	FINEDB_OK if OK.
 */
static int _send_key_data(finedb_client_t *client, ybool_t create_only,
                          ybool_t update_only, ybin_t key, ybin_t data) {
	char code;
	ssize_t expected, rc;

	// request
	{
		struct iovec iov[5];
		struct msghdr mh;
		uint16_t key_nlen;
		uint32_t data_nlen;
		struct snappy_env zip_env;
		size_t zip_len;
		char *zip_data = NULL;

		// data compression
		memset(&zip_env, 0, sizeof(struct snappy_env));
		if (snappy_init_env(&zip_env)) {
			return (FINEDB_ERR_NETWORK);
		}
		if ((zip_data = YMALLOC(snappy_max_compressed_length(data.len))) == NULL) {
			return (FINEDB_ERR_MEMORY);
		}
		if (snappy_compress(&zip_env, data.data, data.len, zip_data, &zip_len)) {
			YFREE(zip_data);
			return (FINEDB_ERR_ZIP);
		}
		zip_data[zip_len] = '\0';
		snappy_free_env(&zip_env);
		// preparation
#if 0
		if (create_only)
			code = PROTO_ADD;
		else if (update_only)
			code = PROTO_UPDATE;
		else
#endif /* 0 */
			code = PROTO_PUT;
		code = REQUEST_ADD_COMPRESSED(code);
		if (client->sync)
			code = REQUEST_ADD_SYNC(code);
		key_nlen = htons((uint16_t)key.len);
		data_nlen = htonl((uint32_t)zip_len);
		// creation of the message
		bzero(&mh, sizeof(struct msghdr));
		mh.msg_iov = iov;
		mh.msg_iovlen = 5;
		iov[0].iov_base = (caddr_t)&code;
		iov[0].iov_len = sizeof(code);
		iov[1].iov_base = (caddr_t)&key_nlen;
		iov[1].iov_len = sizeof(uint16_t);
		iov[2].iov_base = (caddr_t)key.data;
		iov[2].iov_len = key.len;
		iov[3].iov_base = (caddr_t)&data_nlen;
		iov[3].iov_len = sizeof(uint32_t);
		iov[4].iov_base = (caddr_t)zip_data;
		iov[4].iov_len = zip_len;
		// sending
		expected = 1 + sizeof(uint16_t) + key.len + sizeof(uint32_t) + zip_len;
		rc = sendmsg(client->sock, &mh, 0);
		YFREE(zip_data);
		if (rc != expected)
			return (FINEDB_ERR_NETWORK);
	}
	// response
	rc = read(client->sock, &code, 1);
	if (rc != 1)
		return (FINEDB_ERR_NETWORK);
	if (RESPONSE_STATUS(code) != RESP_OK)
		return (FINEDB_ERR_SERVER);
	return (FINEDB_OK);
}

/**
 * Read data from a socket.
 * @param	fd		Socket descriptor.
 * @param	container	Dynamic buffer.
 * @param	size		Expected size of data.
 * @return	YENOERR if OK.
 */
static yerr_t _read_data(int fd, ydynabin_t *container, size_t size) {
	char buff[8196];
	ssize_t bufsz;
	yerr_t dynaerr;

	if (container->len >= size)
		return (YENOERR);
	while (container->len < size) {
		if ((bufsz = read(fd, buff, 8196)) < 0)
			return (YEACCESS);
		if (bufsz == 0) {
			if (container->len < size)
				return (YECONNRESET);
			break;
		}
		if ((dynaerr = ydynabin_expand(container, buff, (size_t)bufsz)) != YENOERR)
			return (dynaerr);
	}
	return (YENOERR);
}
