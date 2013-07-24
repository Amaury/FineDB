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
#define REQUEST_COMMAND(c)		(c & 0xe)	// 00001110

/** @define REQUEST_HAS_OPT Extract the multi-purpose option from a request. */
#define REQUEST_HAS_MIXED_OPT(c)	(c & PROTO_OPT_MIXED)

/** @define REQUEST_HAS_SERIALIZED Extract the serialized option from a request. */
#define REQUEST_HAS_SERIALIZED(c)	(c & PROTO_OPT_SERIALIZED)

/** @define REQUEST_HAS_COMPRESSED Extract the compression option from a request. */
#define REQUEST_HAS_COMPRESSED(c)	(c & PROTO_OPT_COMPRESSED)

/** @define REQUEST_HAS_SERVTOSERV Extract the server-to-server option from a request. */
#define REQUEST_HAS_SERVTOSERV(c)	(c & PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE WRITING *************** */

/** @define RESPONSE_ADD_DATA Add the data option to a response code. */
#define RESPONSE_ADD_DATA(c)		(c | PROTO_OPT_DATA)

/** @define RESPONSE_ADD_COMPRESSED Add the compression option to a response code. */
#define	RESPONSE_ADD_COMPRESSED		(c | PROTO_OPT_COMPRESSED)

/* **************** REQUEST WRITING **************** */

/** @define REQUEST_ADD_SYNC Add the sync option to a request code. */
#define REQUEST_ADD_SYNC(c)		(c | PROTO_OPT_SYNC)

/** @define REQUEST_ADD_DBNAME Add the dbname option to a request code. */
#define REQUEST_ADD_DBNAME(c)		(c | PROTO_OPT_DBNAME)

/** @define REQUEST_ADD_DATA Add the data option to a request code. */
#define REQUEST_ADD_DATA(c)		(c | PROTO_OPT_DATA)

/** @define REQUEST_ADD_COMPRESSeD Add the compression option to a request code. */
#define REQUEST_ADD_COMPRESSED(c)	(c | PROTO_OPT_COMPRESSED)

/** @define REQUEST_ADD_SERVTOSERV Add the server-to-server option to a request code. */
#define REQUEST_ADD_SERVTOSERV(c)	(c | PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE READING *************** */

/** @define RESPONSE_CODE Extract the code from a response. */
#define RESPONSE_CODE(c)		(c & 0x7) // 0b00000111

/**
 * @typedef	protocol_command_t
 *		List of protocol commands.
 * @constant	PROTO_SETDB	SETDB command.
 * @constant	PROTO_GET	GET command.
 * @constant	PROTO_DEL	DEL command.
 * @constant	PROTO_PUT	PUT command.
 * @constant	PROTO_LIST	LIST command.
 * @constant	PROTO_DROP	DROP command.
 */
typedef enum protocol_command_e {
	PROTO_SETDB	= 0,
	PROTO_GET	= 1,
	PROTO_DEL	= 2,
	PROTO_PUT	= 3,
	PROTO_LIST	= 4,
	PROTO_DROP	= 5
} protocol_command_t;

/**
 * @typedef	protocol_option_t
 *		List of command options.
 * @constant	PROTO_OPT_MIXED		Mixed option.
 * @constant	PROTO_OPT_SERIALIZED	Serialized data.
 * @constant	PROTO_OPT_COMPRESSED	Compression activated.
 * @constant	PROTO_OPT_SERVTOSERV	Server-to-server command.
 */
typedef enum protocol_option_e {
	PROTO_OPT_MIXED		= 0x10,	// 0b00010000
	PROTO_OPT_SERIALIZED	= 0x20,	// 0b00100000
	PROTO_OPT_COMPRESSED	= 0x40,	// 0b01000000
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
