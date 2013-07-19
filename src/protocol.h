/**
 * @header	protocol.h
 * @abstract	Protocol constants.
 * @discussion	The FineDB protocols is based on bit combinations.
 * @author	Amaury Bouchard <amaury@amaury.net>
 * @copyright	© 2013, Amaury Bouchard
 */
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/* **************** REQUEST READING ************* */

/** @define REQUEST_COMMAND Extract the command from other options. */
#define REQUEST_COMMAND(c)		(c & 3)

/** @define REQUEST_HAS_SYNC Extract the sync option from a request. */
#define REQUEST_HAS_SYNC(c)		(c & PROTO_OPT_SYNC)

/** @define REQUEST_HAS_DBNAME Extract the dbname option from a request. */
#define REQUEST_HAS_DBNAME(c)		(c & PROTO_OPT_DBNAME)

/** @define REQUEST_HAS_DATA Extract the data option from a request. */
#define REQUEST_HAS_DATA(c)		(c & PROTO_OPT_DATA)

/** @define REQUEST_HAS_COMPRESS Extract the compression option from a request. */
#define REQUEST_HAS_COMPRESS(c)		(c & PROTO_OPT_COMPRESS)

/** @define REQUEST_HAS_SERVTOSERV Extract the server-to-server option from a request. */
#define REQUEST_HAS_SERVTOSERV(c)	(c & PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE WRITING *************** */

/** @define RESPONSE_ADD_DATA Add the data option to a response code. */
#define RESPONSE_ADD_DATA(c)		(c | PROTO_OPT_DATA)

/** @define RESPONSE_ADD_COMPRESS Add the compression option to a response code. */
#define	RESPONSE_ADD_COMPRESS		(c | PROTO_OPT_COMPRESS)

/* **************** REQUEST WRITING **************** */

/** @define REQUEST_ADD_SYNC Add the sync option to a request code. */
#define REQUEST_ADD_SYNC(c)		(c | PROTO_OPT_SYNC)

/** @define REQUEST_ADD_DBNAME Add the dbname option to a request code. */
#define REQUEST_ADD_DBNAME(c)		(c | PROTO_OPT_DBNAME)

/** @define REQUEST_ADD_DATA Add the data option to a request code. */
#define REQUEST_ADD_DATA(c)		(c | PROTO_OPT_DATA)

/** @define REQUEST_ADD_COMPRESS Add the compression option to a request code. */
#define REQUEST_ADD_COMPRESS(c)		(c | PROTO_OPT_COMPRESS)

/** @define REQUEST_ADD_SERVTOSERV Add the server-to-server option to a request code. */
#define REQUEST_ADD_SERVTOSERV(c)	(c | PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE READING *************** */

/** @define RESPONSE_CODE Extract the code from a response. */
#define RESPONSE_CODE(c)		(c & 0x7) // 0b00000111

/**
 * @typedef	protocol_command_t
 *		List of protocol commands.
 * @constant	PROTO_GET	GET command.
 * @constant	PROTO_PUT	PUT command.
 * @constant	PROTO_LIST	LIST command.
 */
typedef enum protocol_command_e {
	PROTO_GET	= 0,
	PROTO_PUT	= 1,
	PROTO_LIST	= 2
} protocol_command_t;

/**
 * @typedef	protocol_option_t
 *		List of command options.
 * @constant	PROTO_OPT_SYNC		Synchronous command.
 * @constant	PROTO_OPT_DBNAME	Database's name.
 * @constant	PROTO_OPT_DATA		Data is embedded in the message.
 * @constant	PROTO_OPT_COMPRESS	Compression activated.
 * @constant	PROTO_OPT_SERVTOSERV	Server-to-server command.
 */
typedef enum protocol_option_e {
	PROTO_OPT_SYNC		= 0x08,	// 0b00001000
	PROTO_OPT_DBNAME	= 0x10, // 0b00010000
	PROTO_OPT_DATA		= 0x20,	// 0b00100000
	PROTO_OPT_COMPRESS	= 0x40,	// 0b01000000
	PROTO_OPT_SERVTOSERV	= 0x80,	// 0b10000000
} protocol_option_t;

/**
 * @typedef	protocol_response_t
 *		List of response codes.
 * @constant	RESP_OK		OK.
 * @constant	RESP_PROTO	Protocol error.
 * @constant	RESP_SERVER_ERR	Server error.
 * @constant	RESP_NO_DATA	Unknown key.
 * @constant	RESP_UNDEFINED	Undefined error.
 */
typedef enum protocol_response_e {
	RESP_OK		= 0,
	RESP_PROTO	= 1,
	RESP_SERVER_ERR	= 2,
	RESP_NO_DATA	= 3,
	RESP_UNDEFINED	= 4
} protocol_response_t;

#endif /* __PROTOCOL_H__ */
