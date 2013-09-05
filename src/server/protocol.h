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
#define REQUEST_COMMAND(c)		(c & 0xf)	// 00001111

/** @define REQUEST_HAS_SYNC Extract the multi-purpose option from a request. */
#define REQUEST_HAS_SYNC(c)		(c & PROTO_OPT_SYNC)

/** @define REQUEST_HAS_SERIALIZED Extract the serialized option from a request. */
#define REQUEST_HAS_SERIALIZED(c)	(c & PROTO_OPT_SERIALIZED)

/** @define REQUEST_HAS_COMPRESSED Extract the compression option from a request. */
#define REQUEST_HAS_COMPRESSED(c)	(c & PROTO_OPT_COMPRESSED)

/** @define REQUEST_HAS_SERVTOSERV Extract the server-to-server option from a request. */
#define REQUEST_HAS_SERVTOSERV(c)	(c & PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE WRITING *************** */

/** @define RESPONSE_ADD_SERIALIZED Add the serialized option to a response code. */
#define RESPONSE_ADD_SERIALIZED(c)	(c | PROTO_OPT_SERIALIZED)

/** @define RESPONSE_ADD_COMPRESSED Add the compression option to a response code. */
#define	RESPONSE_ADD_COMPRESSED(c)	(c | PROTO_OPT_COMPRESSED)

/* **************** REQUEST WRITING **************** */

/** @define REQUEST_ADD_SYNC Add the sync option to a request code. */
#define REQUEST_ADD_SYNC(c)		(c | PROTO_OPT_SYNC)

/** @define REQUEST_ADD_COMPRESSeD Add the compression option to a request code. */
#define REQUEST_ADD_COMPRESSED(c)	(c | PROTO_OPT_COMPRESSED)

/** @define REQUEST_ADD_SERVTOSERV Add the server-to-server option to a request code. */
#define REQUEST_ADD_SERVTOSERV(c)	(c | PROTO_OPT_SERVTOSERV)

/* **************** RESPONSE READING *************** */

/** @define RESPONSE_STATUS Extract the status of a response. */
#define RESPONSE_STATUS(c)		(c & 1)

/** @define RESPONSE_ERROR Extract the error code from a response. */
#define RESPONSE_ERROR(c)		(c & 0x1e)	// 0b00011110

/**
 * @typedef	protocol_command_t
 *		List of protocol commands.
 * @constant	PROTO_PING	PING command.
 * @constant	PROTO_GET	GET command.
 * @constant	PROTO_DEL	DEL command.
 * @constant	PROTO_PUT	PUT command.
 * @constant	PROTO_SETDB	SETDB command.
 * @constant	PROTO_START	START command.
 * @constant	PROTO_STOP	STOP command.
 * @constant	PROTO_ADMIN	ADMIN command.
 * @constant	PROTO_EXTRA	EXTRA command.
 */
typedef enum protocol_command_e {
	PROTO_PING	= 0x0,
	PROTO_GET	= 0x1,
	PROTO_DEL	= 0x2,
	PROTO_PUT	= 0x3,
	PROTO_SETDB	= 0x4,
	PROTO_START	= 0x5,
	PROTO_STOP	= 0x6,
	PROTO_ADMIN	= 0xe,
	PROTO_EXTRA	= 0xf,
} protocol_command_t;

/**
 * @typedef	protocol_option_t
 *		List of command options.
 * @constant	PROTO_OPT_SYNC		Synchronous request..
 * @constant	PROTO_OPT_SERIALIZED	Serialized data.
 * @constant	PROTO_OPT_COMPRESSED	Compression activated.
 * @constant	PROTO_OPT_SERVTOSERV	Server-to-server command.
 */
typedef enum protocol_option_e {
	PROTO_OPT_SYNC		= 0x10,	// 0b00010000
	PROTO_OPT_SERIALIZED	= 0x20,	// 0b00100000
	PROTO_OPT_COMPRESSED	= 0x40,	// 0b01000000
	PROTO_OPT_SERVTOSERV	= 0x80,	// 0b10000000
} protocol_option_t;

/**
 * @typedef	protocol_response_t
 *		List of response codes.
 * @constant	RESP_ERR_UNDEFINED	Undefined error.
 * @constant	RESP_OK			OK.
 * @constant	RESP_ERR_PROTOCOL	Protocol error.
 * @constant	RESP_ERR_SERVER		Server error.
 * @constant	RESP_ERR_FULL_DB	Database is full.
 * @constant	RESP_ERR_TOO_MANY_DB	Too many opened databases.
 * @constant	RESP_ERR_BAD_NAME	Bad name (dbname for SETDB request,
 *					key for GET or DEL request).
 * @constant	RESP_ERR_TRANSACTION	Transaction error.
 */
typedef enum protocol_response_e {
	RESP_ERR_UNDEFINED	= 0,
	RESP_OK			= 1,
	RESP_ERR_PROTOCOL	= 2,
	RESP_ERR_SERVER		= 3,
	RESP_ERR_FULL_DB	= 4,
	RESP_ERR_TOO_MANY_DB	= 5,
	RESP_ERR_BAD_NAME	= 6,
	RESP_ERR_TRANSACTION	= 7
} protocol_response_t;

#endif /* __PROTOCOL_H__ */
