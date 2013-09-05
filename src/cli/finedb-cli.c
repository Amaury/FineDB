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
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include "ydefs.h"
#include "libfinedb.h"
#include "linenoise.h"

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

/** @define HISTORY_FILE Name of the command-line history file. */
#define HISTORY_FILE	".finedb-cli.history"

/** @define LTRIM Move forward a pointer to skip spaces. */
#define LTRIM(pt)	while(*pt && IS_SPACE(*pt)) ++pt;

/* *** function declarations *** */
void printf_decorated(const char *type, char *s, ...);
void printf_color(const char *color, char *s, ...);
void print_response(char c);
void cli_completion(const char *buf, linenoiseCompletions *lc);
void command_help(void);
void command_use(cli_t *cli, char *pt);
void command_get(cli_t *cli, char *pt);
void command_del(cli_t *cli, char *pt);
void command_send_data(cli_t *cli, char *pt, ybool_t create_only, ybool_t update_only);
void command_inc(cli_t *cli, char *pt);
void command_dec(cli_t *cli, char *pt);
void command_start(cli_t *cli);
void command_stop(cli_t *cli);
#if 0
void command_list(cli_t *cli, char *pt);
void command_drop(cli_t *cli, char *pt);
#endif
void command_ping(cli_t *cli);
void command_sync(cli_t *cli);
void command_async(cli_t *cli);
void command_autocheck(cli_t *cli, char *pt);
int check_connection(cli_t *cli);

/* Array of commands. */
char *commands[] = {
	"help", "use", "get", "del", "put", "add", "update", "inc", "dec",
	"start", "commit", "rollback", "ping", "sync", "async", "autocheck",
	NULL
};

/* Main function. */
int main(int argc, char *argv[]) {
	cli_t cli;
	char *hostname = DEFAULT_HOST;
	char history_file[4096];
	ybool_t interactive_mode = YTRUE;

	bzero(&cli, sizeof(cli_t));
	cli.autocheck = YTRUE;
	if (argc == 2 && argv[1][0] != '-')
		hostname = argv[1];
	if (argc == 3 && !strcmp(argv[2], "-"))
		interactive_mode = YFALSE;
	// init database connection
	if ((cli.finedb = finedb_create(hostname, 11138)) == NULL) {
		printf_color("red", "Memory error.");
		printf("\n");
		exit(1);
	}
	if (finedb_connect(cli.finedb) != FINEDB_OK) {
		printf_color("red", "Unable to connect to server '%s' on port '%d'.", argv[1], 11138);
		printf("\n");
		exit(2);
	}
	// interactive mode init
	if (interactive_mode) {
		char *home = NULL;

		if ((home = getenv("HOME")) != NULL) {
			FILE *hist;

			snprintf(history_file, sizeof(history_file), "%s/%s", home, HISTORY_FILE);
			if ((hist = fopen(history_file, "w+")) != NULL) {
				fclose(hist);
				linenoiseHistoryLoad(HISTORY_FILE);
			}
			linenoiseSetCompletionCallback(cli_completion);
		}
	}
	// main loop
	for (; ; ) {
		char buff[4096], *line = NULL, *pt, *cmd;

		if (!interactive_mode) {
			ssize_t bufsz, linesz = 0;

			while ((bufsz = read(0, buff, sizeof(buff))) > 0) {
				pt = (char*)malloc(linesz + bufsz + 1);
				memcpy(pt, line, linesz);
				memcpy((void*)((size_t)pt + linesz), buff, bufsz);
				linesz += bufsz;
				pt[linesz] = '\0';
				if (line)
					free(line);
				line = pt;
			}
		} else {
			snprintf(buff, sizeof(buff), "%s > ", (cli.dbname ? cli.dbname : "default"));
			if ((line = linenoise(buff)) == NULL)
				break;
		}
		pt = line;
		LTRIM(pt);
		cmd = pt;
		// add command line to history
		linenoiseHistoryAdd(cmd);
		linenoiseHistorySave(history_file);
		// extract the command
		while (*pt && !IS_SPACE(*pt))
			++pt;
		*pt++ = '\0';
		LTRIM(pt);
		/* command management */
		if (cmd[0] == '\0')
			goto reloop;
			//continue;
		// local commands, no need for a running connection
		if (!strcasecmp(cmd, "exit") || !strcasecmp(cmd, "quit"))
			exit(0);
		if (!strcasecmp(cmd, "help") || cmd[0] == '?') {
			command_help();
			goto reloop;
			//continue;
		} else if (!strcasecmp(cmd, "sync")) {
			command_sync(&cli);
			goto reloop;
			//continue;
		} else if (!strcasecmp(cmd, "async")) {
			command_async(&cli);
			goto reloop;
			//continue;
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
		else if (!strcasecmp(cmd, "stop"))
			command_stop(&cli);
#if 0
		else if (!strcasecmp(cmd, "list"))
			command_list(&cli, pt);
#endif
		else if (!strcasecmp(cmd, "ping"))
			command_ping(&cli);
		else if (!strcasecmp(cmd, "autocheck"))
			command_autocheck(&cli, pt);
		else {
			printf_color("red", "Bad command.");
			printf("\n");
		}
reloop:
		free(line);
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
	printf("\n");
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
		if ((rc = finedb_setdb(cli->finedb, NULL)) != 0) {
			printf_color("red", "Unable to use the default database (%d).", rc);
			printf("\n");
		} else {
			YFREE(cli->dbname);
			printf_decorated("faint", "Use the default database");
			printf("\n");
		}
	} else {
		// set a new database
		if ((rc = finedb_setdb(cli->finedb, pt)) != 0) {
			printf_color("red", "Unable to use the '%s' database (%d).", rc);
			printf("\n");
		} else {
			YFREE(cli->dbname);
			cli->dbname = strdup(pt);
			printf_decorated("faint", "Use the '%s' database", pt);
			printf("\n");
		}
	}
}

/* Set synchronous mode. */
void command_sync(cli_t *cli) {
	finedb_sync(cli->finedb);
	printf_decorated("faint", "Set synchronous mode.");
	printf("\n");
}
/* Set asynchronous mode. */
void command_async(cli_t *cli) {
	finedb_async(cli->finedb);
	printf_decorated("faint", "Set asynchronous mode.");
	printf("\n");
}

/* Fetch a value from its key. */
void command_get(cli_t *cli, char *pt) {
	char *pt2, *key;
	ybin_t bkey, bdata;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)");
		printf("\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key");
		printf("\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)");
		printf("\n");
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
		printf_color("red", "Unable to get key '%s' (%d).", key, rc);
		printf("\n");
		return;
	}
	if (bdata.data == NULL || bdata.len == 0) {
		printf_decorated("faint", "No data.");
		printf("\n");
	}
	printf("%s\n", (char*)bdata.data);
}

/* Delete a key. */
void command_del(cli_t *cli, char *pt) {
	char *pt2, *key;
	ybin_t bkey;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)");
		printf("\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key");
		printf("\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)");
		printf("\n");
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
		printf_color("red", "Unable to delete key '%s'.", key);
		printf("\n");
	printf_decorated("faint", "OK");
	printf("\n");
}

/* Put, add or update a key/value in database. */
void command_send_data(cli_t *cli, char *pt, ybool_t create_only, ybool_t update_only) {
	char *pt2, *key, *data;
	ybin_t bkey, bdata;
	int rc;

	LTRIM(pt);
	if (*pt != '"') {
		printf_decorated("faint", "Bad key format (no quote)");
		printf("\n");
		return;
	}
	pt++;
	if (!*pt) {
		printf_decorated("faint", "Bad key");
		printf("\n");
		return;
	}
	key = pt2 = pt;
	pt2 = pt;
	if ((pt2 = strchr(pt2, '"')) == NULL) {
		printf_decorated("faint", "Bad key format (no trailing quote)");
		printf("\n");
		return;
	}
	*pt2 = '\0';
	pt2++;
	if (!*pt2) {
		printf_decorated("faint", "Missing data");
		printf("\n");
		return;
	}
	*pt2 = '\0';
	data = pt2 + 1;
	LTRIM(data);
	if (*data != '"') {
		printf_decorated("faint", "Bad data format (no quote)");
		printf("\n");
		return;
	}
	data++;
	if (!*data) {
		printf_decorated("faint", "Missing data");
		printf("\n");
		return;
	}
	pt2 = data + strlen(data) - 1;
	if (*pt2 != '"') {
		printf_decorated("faint", "Bad data format (no trailing quote)");
		printf("\n");
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
		printf_color("red", "Unable to %s key '%s'.",
		             (create_only ? "add" : (update_only ? "update" : "put")), key);
	else
		printf_decorated("faint", "OK");
	printf("\n");
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
		printf_color("red", "A transaction is already open. It will be rollbacked.");
		printf("\n");
	}
	// request
	rc = finedb_start(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		printf("\n");
		return;
	}
	printf_decorated("faint", "Transaction started.");
	printf("\n");
	cli->in_transaction = YTRUE;
}

/* Stop a transaction. */
void command_stop(cli_t *cli) {
	int rc;

	// check connection if needed
	if (!check_connection(cli))
		return;
	// check opened transaction
	if (!cli->in_transaction) {
		printf_color("red", "No opened transaction.");
		printf("\n");
		return;
	}
	// request
	rc = finedb_stop(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		printf("\n");
		return;
	}
	printf_decorated("faint", "Transaction stopped.");
	printf("\n");
	cli->in_transaction = YFALSE;
}

/* Test a connection. */
void command_ping(cli_t *cli) {
	int rc;

	// request
	rc = finedb_ping(cli->finedb);
	if (rc) {
		printf_color("red", "Server error.");
		printf("\n");
		return;
	}
	printf_decorated("faint", "OK");
	printf("\n");
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
	printf_decorated("faint", "autocheck option is %s",
	                 cli->autocheck ? "activated" : "deactivated");
	printf("\n");
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
		printf_color("red", "Lost connection to server.");
		printf("\n");
		return (0);
	}
	if (cli->in_transaction) {
		cli->in_transaction = YFALSE;
		printf_decorated("faint", "Reconnected to server. Transaction is lost.");
	} else
		printf_decorated("faint", "Reconnected to server.");
	printf("\n");
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

/* ******************** UTILITIES FUNCTIONS ********************* */

/**
 * Command line completion.
 * @param	buf	Text to complete.
 * @param	lc	Readline object.
 */
void cli_completion(const char *buf, linenoiseCompletions *lc) {
	unsigned int i;
	size_t bufsz = strlen(buf);

	for (i = 0; commands[i]; ++i) {
		char *cmd = commands[i];

		if (strlen(cmd) < bufsz)
			continue;
		if (!strncasecmp(buf, cmd, bufsz))
			linenoiseAddCompletion(lc, cmd);
	}
}

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

