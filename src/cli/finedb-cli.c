/**
 * finedb-cli
 * Command-line client interface to a FineDB server.
 *
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include "libfinedb.h"

/**
 * Options structure.
 * @field	finedb	Pointer to the client structure.
 * @field	dbname	Name of the current database;
 */
typedef struct cli_s {
	finedb_client_t *finedb;
	char *dbname;
} cli_t;

/** @define LTRIM Move forward a pointer to skip spaces. */
#define LTRIM(pt)	while(*pt && IS_SPACE(*pt)) ++pt;

// function declarations
void printf_decorated(const char *type, char *s, ...);
void printf_color(const char *color, char *s, ...);
void print_response(char c);
void command_help(void);
void command_use(cli_t *cli, char *pt);
void command_get(cli_t *cli, char *pt);
void command_del(cli_t *cli, char *pt);
void command_send_data(cli_t *cli, char *pt, ybool_t create_only, ybool_t update_only);
void command_inc(cli_t *cli, char *pt);
void command_dec(cli_t *cli, char *pt);
void command_start(cli_t *cli);
void command_commit(cli_t *cli);
void command_rollback(cli_t *cli);
#if 0
void command_list(cli_t *cli, char *pt);
void command_drop(cli_t *cli, char *pt);
#endif
void command_sync(cli_t *cli);
void command_async(cli_t *cli);

int main(int argc, char *argv[]) {
	cli_t cli;
	char buff[8196];
	size_t bufsz;

	if (argc == 1) {
		printf_color("red", "Usage: finedb-cli hostname\n");
		exit(1);
	}
	bzero(&cli, sizeof(cli_t));
	if ((cli.finedb = finedb_connect(argv[1], 11138)) == NULL) {
		printf_color("red", "Unable to connect to server '%s' on port '%d'.\n", argv[1], 11138);
		exit(2);
	}
	// main loop
	for (; ; ) {
		char *pt, *cmd;

		bzero(buff, sizeof(buff));
		printf_decorated("bold", (cli.dbname ? cli.dbname : "default"));
		printf(" > ");
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
		else if (!strcasecmp(cmd, "use"))
			command_use(&cli, pt);
		else if (!strcasecmp(cmd, "get"))
			command_get(&cli, pt);
		else if (!strcasecmp(cmd, "del"))
			command_del(&cli, pt);
		else if (!strcasecmp(cmd, "put"))
			command_send_data(&cli, pt, YFALSE, YFALSE);
		else if (!strcasecmp(cmd, "add"))
			command_send_data(&cli, pt, YTRUE, YFALSE);
		else if (!strcasecmp(cmd, "update"))
			command_send_data(&cli, pt, YTRUE, YFALSE);
		else if (!strcasecmp(cmd, "inc"))
			command_inc(&cli, pt);
		else if (!strcasecmp(cmd, "dec"))
			command_dec(&cli, pt);
		else if (!strcasecmp(cmd, "start"))
			command_start(&cli);
		else if (!strcasecmp(cmd, "commit"))
			command_commit(&cli);
		else if (!strcasecmp(cmd, "rollback"))
			command_rollback(&cli);
#if 0
		else if (!strcasecmp(cmd, "list"))
			command_list(&cli, pt);
#endif
		else if (!strcasecmp(cmd, "sync"))
			command_sync(&cli);
		else if (!strcasecmp(cmd, "async"))
			command_async(&cli);
		else
			printf_color("red", "Bad command.\n");
	}
	return (0);
}

/* Show usage. */
void command_help() {
	printf_decorated("faint", "Usage:    finedb-cli hostname\n"
	                          "Commands:\n"
	                          "    put \"key\" \"data\"\n"
	                          "    get \"key1\"\n"
	                          "    del \"key1\"\n"
	                          "    sync\n"
	                          "    async\n"
	                          "    quit\n");
}

/* Defines the used database. */
void command_use(cli_t *cli, char *pt) {
	int rc;

	cli->finedb->debug = YTRUE;
	if (!strlen(pt) || !strcasecmp(pt, "default")) {
		// use default database
		if ((rc = finedb_setdb(cli->finedb, NULL)) != 0)
			printf_color("red", "Unable to use the default database (%d).\n", rc);
		else {
			YFREE(cli->dbname);
			printf_decorated("faint", "Use the default database\n");
		}
	} else {
		// set a new database
		if ((rc = finedb_setdb(cli->finedb, pt)) != 0)
			printf_color("red", "Unable to use the '%s' database (%d).\n", rc);
		else {
			YFREE(cli->dbname);
			cli->dbname = strdup(pt);
			printf_decorated("faint", "Use the '%s' database\n", pt);
		}
	}
}

/* Set synchronous mode. */
void command_sync(cli_t *cli) {
	finedb_sync(cli->finedb);
	printf_decorated("faint", "Set synchronous mode.\n");
}
/* Set asynchronous mode. */
void command_async(cli_t *cli) {
	finedb_async(cli->finedb);
	printf_decorated("faint", "Set asynchronous mode.\n");
}

/* Fetch a value from its key. */
void command_get(cli_t *cli, char *pt) {
	char *pt2, *key;
	ybin_t bkey, bdata;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';

	// request
	bzero(&bdata, sizeof(bdata));
	ybin_set(&bkey, key, strlen(key));
	rc = finedb_get(cli->finedb, bkey, &bdata);
	if (rc) {
		printf_color("red", "Unable to get key '%s' (%d).\n", key, rc);
		return;
	}
	if (bdata.data == NULL || bdata.len == 0)
		printf_decorated("faint", "No data.\n");
	printf("%s\n", (char*)bdata.data);
}

/* Delete a key. */
void command_del(cli_t *cli, char *pt) {
	char *pt2, *key;
	ybin_t bkey;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';

	// request
	ybin_set(&bkey, key, strlen(key));
	rc = finedb_del(cli->finedb, bkey);
	if (rc)
		printf_color("red", "Unable to delete key '%s'.\n", key);
}

/* Put, add or update a key/value in database. */
void command_send_data(cli_t *cli, char *pt, ybool_t create_only, ybool_t update_only) {
	char *pt2, *key, *data;
	ybin_t bkey, bdata;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';
	pt2++;
	if (!*pt2) {
		printf_decorated("faint", "Missing data\n");
		return;
	}
	*pt2 = '\0';
	data = pt2 + 1;
	LTRIM(data);
	if (*data != '"') {
		printf_decorated("faint", "Bad data format (no quote)\n");
		return;
	}
	data++;
	if (!*data) {
		printf_decorated("faint", "Missing data\n");
		return;
	}
	pt2 = data + strlen(data) - 1;
	if (*pt2 != '"') {
		printf_decorated("faint", "Bad data format (no trailing quote)\n");
		return;
	}
	*pt2 = '\0';

	// request
	ybin_set(&bkey, key, strlen(key));
	ybin_set(&bdata, data, strlen(data));
	if (create_only)
		rc = finedb_add(cli->finedb, bkey, bdata);
	else if (update_only)
		rc = finedb_update(cli->finedb, bkey, bdata);
	else
		rc = finedb_put(cli->finedb, bkey, bdata);
	if (rc)
		printf_color("red", "Unable to %s key '%s'.\n",
		             (create_only ? "add" : (update_only ? "update" : "put")), key);
}

/* Increment a value. */
void command_inc(cli_t *cli, char *pt) {
}

/* Decrement a value. */
void command_dec(cli_t *cli, char *pt) {
}

/* Start a transaction. */
void command_start(cli_t *cli) {
	int rc;

	rc = finedb_start(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		return;
	}
	printf_decorated("faint", "Transaction started.\n");
}

/* Commit a transaction. */
void command_commit(cli_t *cli) {
	int rc;

	rc = finedb_commit(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		return;
	}
	printf_decorated("faint", "Transaction committed.\n");
}

/* Rollback a transaction. */
void command_rollback(cli_t *cli) {
	int rc;

	rc = finedb_rollback(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		return;
	}
	printf_decorated("faint", "Transaction aborted.\n");
}

#if 0
/* List the keys stored in database. */
void command_list(cli_t *cli, char *pt) {
	char *buffer, c;
	size_t sz, offset, length, rc;

	LTRIM(pt);
	// create sending buffer
	sz = 1;
	if (cli->dbname)
		sz += 1 + strlen(cli->dbname);
	buffer = YMALLOC(sz);
	// set the code byte
	buffer[0] = PROTO_LIST;
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
	{
		size_t n;
		for (n = 0; n < sz; n++)
			printf("%02x ", buffer[n]);
		printf("\n");
	}

	// send data
	rc = write(cli->fd, buffer, sz);
	YFREE(buffer);
	if (rc != sz) {
		printf("%c[Connection error%c[0m\n", 27, 27);
		return;
	}

	// get response
	if (read(cli->fd, &c, 1) != 1) {
		printf("%c[Connection error%c[0m\n", 27, 27);
		return;
	}
	if (RESPONSE_CODE(c) != RESP_OK) {
		printf("%c[2mERROR: %s%c[0m\n", 27,
		       (RESPONSE_CODE(c) == RESP_PROTO ? "protocol" :
		        (RESPONSE_CODE(c) == RESP_SERVER_ERR ? "server" :
		         (RESPONSE_CODE(c) == RESP_NO_DATA ? "no data" : "unknown"))), 27);
		return;
	}
	printf("%c[2mOK%c[0m\n", 27, 27);
	for (; ; ) {
		uint16_t ln = 0, lh;

		if (read(cli->fd, &ln, 2) < 2)
			break;
		lh = ntohs(ln);
		buffer = YMALLOC(lh + 1);
		if (read(cli->fd, buffer, lh) != lh) {
			printf("%c[Connection error%c[0m\n", 27, 27);
			YFREE(buffer);
			return;
		}
		printf("%c[2m%s%c[0m\n", 27, buffer, 27);
		YFREE(buffer);
	}
}
#endif

/**
 * Write a decorated string.
 * @param	type	"bold", "faint", "underline" or "negative".
 * @param	s	The string to write.
 */
void printf_decorated(const char *type, char *s, ...) {
	va_list ap;
	int i;

	va_start(ap, s);
	if (!strcasecmp(type, "bold"))
		i = 1;
	else if (!strcasecmp(type, "faint"))
		i = 2;
	else if (!strcasecmp(type, "underline"))
		i = 4;
	else if (!strcasecmp(type, "negative"))
		i = 7;
	printf("%c[%dm", 27, i);
	vprintf(s, ap);
	printf("%c[0m", 27);
	va_end(ap);
}

/**
 * Write a coloured string.
 * @param	color	"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white".
 * @param	s	The string to write.
 */
void printf_color(const char *color, char *s, ...) {
	va_list ap;
	int i;

	va_start(ap, s);
	if (!strcasecmp(color, "black"))
		i = 0;
	else if (!strcasecmp(color, "red"))
		i = 1;
	else if (!strcasecmp(color, "green"))
		i = 2;
	else if (!strcasecmp(color, "yellow"))
		i = 3;
	else if (!strcasecmp(color, "blue"))
		i = 4;
	else if (!strcasecmp(color, "magenta"))
		i = 5;
	else if (!strcasecmp(color, "cyan"))
		i = 6;
	else if (!strcasecmp(color, "white"))
		i = 7;
	printf("%c[9%dm", 27, i);
	vprintf(s, ap);
	printf("%c[0m", 27);
	va_end(ap);
}

