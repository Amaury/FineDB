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
#include "ydefs.h"
#include "libfinedb.h"

/**
 * Options structure.
 * @field	finedb		Pointer to the client structure.
 * @field	dbname		Name of the current database.
 * @field	in_transaction	YTRUE if a transaction is opened.
 * @field	autocheck	YTRUE to check the connection before every request.
 */
typedef struct cli_s {
	finedb_client_t *finedb;
	char *dbname;
	ybool_t in_transaction;
	ybool_t autocheck;
} cli_t;

/** @define DEFAULT_HOST Default hostname of the server. */
#define DEFAULT_HOST	"localhost"

/** @define LTRIM Move forward a pointer to skip spaces. */
#define LTRIM(pt)	while(*pt && IS_SPACE(*pt)) ++pt;

/* *** function declarations *** */
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
void command_ping(cli_t *cli);
void command_sync(cli_t *cli);
void command_async(cli_t *cli);
void command_autocheck(cli_t *cli, char *pt);
int check_connection(cli_t *cli);

/* Main function. */
int main(int argc, char *argv[]) {
	cli_t cli;
	char buff[8196], *hostname = DEFAULT_HOST;
	size_t bufsz;

	bzero(&cli, sizeof(cli_t));
	cli.autocheck = YTRUE;
	if (argc == 2)
		hostname = argv[1];
	if ((cli.finedb = finedb_create(hostname, 11138)) == NULL) {
		printf_color("red", "Memory error.");
		exit(1);
	}
	if (finedb_connect(cli.finedb) != FINEDB_OK) {
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
		/* command management */
		if (cmd[0] == '\0')
			continue;
		// local commands, no need for a running connection
		if (!strcasecmp(cmd, "exit") || !strcasecmp(cmd, "quit"))
			exit(0);
		if (!strcasecmp(cmd, "help") || cmd[0] == '?') {
			command_help();
			continue;
		} else if (!strcasecmp(cmd, "sync")) {
			command_sync(&cli);
			continue;
		} else if (!strcasecmp(cmd, "async")) {
			command_async(&cli);
			continue;
		}
		// commands that need a running connection
		if (!strcasecmp(cmd, "use"))
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
			command_send_data(&cli, pt, YFALSE, YTRUE);
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
		else if (!strcasecmp(cmd, "ping"))
			command_ping(&cli);
		else if (!strcasecmp(cmd, "autocheck"))
			command_autocheck(&cli, pt);
		else
			printf_color("red", "Bad command.\n");
	}
	return (0);
}

/* Show usage. */
void command_help() {
	printf_decorated("faint", "Usage:    finedb-cli [hostname]\n"
	                          "Commands:\n"
	                          "    get \"key1\"\n"
	                          "    put \"key\" \"data\"\n"
	                          "    add \"key\" \"data\"\n"
	                          "    update \"key\" \"data\"\n"
	                          "    del \"key\"\n"
	                          "    use \"dbname\"\n"
	                          "    start\n"
	                          "    commit\n"
	                          "    rollback\n"
	                          "    ping\n"
	                          "    sync\n"
	                          "    async\n"
	                          "    autocheck [on|off]\n"
	                          "    quit\n");
}

/* Defines the used database. */
void command_use(cli_t *cli, char *pt) {
	int rc;

	// check connection if needed
	if (!check_connection(cli))
		return;
	// request
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

	// check connection if needed
	if (!check_connection(cli))
		return;
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

	// check connection if needed
	if (!check_connection(cli))
		return;
	// request
	ybin_set(&bkey, key, strlen(key));
	rc = finedb_del(cli->finedb, bkey);
	if (rc)
		printf_color("red", "Unable to delete key '%s'.\n", key);
	printf_decorated("faint", "OK\n");
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

	// check connection if needed
	if (!check_connection(cli))
		return;
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
	printf_decorated("faint", "OK\n");
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

	// check connection if needed
	if (!check_connection(cli))
		return;
	// check opened transaction
	if (cli->in_transaction) {
		printf_color("red", "A transaction is already open. It will be rollbacked.\n");
	}
	// request
	rc = finedb_start(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.\n");
		return;
	}
	printf_decorated("faint", "Transaction started.\n");
	cli->in_transaction = YTRUE;
}

/* Commit a transaction. */
void command_commit(cli_t *cli) {
	int rc;

	// check connection if needed
	if (!check_connection(cli))
		return;
	// check opened transaction
	if (!cli->in_transaction) {
		printf_color("red", "No opened transaction.\n");
		return;
	}
	// request
	rc = finedb_commit(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.\n");
		return;
	}
	printf_decorated("faint", "Transaction committed.\n");
	cli->in_transaction = YFALSE;
}

/* Rollback a transaction. */
void command_rollback(cli_t *cli) {
	int rc;

	// check connection if needed
	if (!check_connection(cli))
		return;
	// check opened transaction
	if (!cli->in_transaction) {
		printf_color("red", "No opened transaction.\n");
		return;
	}
	// request
	rc = finedb_rollback(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.\n");
		return;
	}
	printf_decorated("faint", "Transaction aborted.\n");
	cli->in_transaction = YFALSE;
}

/* Test a connection. */
void command_ping(cli_t *cli) {
	int rc;

	// check connection if needed
	if (!check_connection(cli))
		return;
	// request
	rc = finedb_ping(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.\n");
		return;
	}
	printf_decorated("faint", "OK\n");
}

/* Set the autocheck option. */
void command_autocheck(cli_t *cli, char *pt) {
	if (strlen(pt)) {
		if (!strcasecmp(pt, "on") || !strcasecmp(pt, "yes") ||
		    !strcasecmp(pt, "true") || !strcmp(pt, "1"))
			cli->autocheck = YTRUE;
		else
			cli->autocheck = YFALSE;
	}
	printf_decorated("faint", "autocheck option is %s\n",
	                 cli->autocheck ? "activated" : "deactivated");
}

/**
 * @function	check_connection
 * Check the connection to the server.
 * @param	cli	Pointer to the client structure.
 * @return	1 if OK, 0 if no connection.
 */
int check_connection(cli_t *cli) {
	if (!cli->autocheck)
		return (1);
	// ping the server
	if (!finedb_ping(cli->finedb))
		return (1);
	// no connection, try to reconnect
	if (finedb_connect(cli->finedb) != FINEDB_OK) {
		printf_color("red", "Lost connection to server.\n");
		return (0);
	}
	if (cli->in_transaction) {
		printf_decorated("faint", "Reconnected to server. Transaction is lost.\n");
		cli->in_transaction = YFALSE;
	} else
		printf_decorated("faint", "Reconnected to server.\n");
	return (1);
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

