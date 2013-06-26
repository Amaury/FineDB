/**
 * @header	protocol.h
 * @abstract	Protocol constants.
 * @discussion	The FineDB protocols is based on bit combinations.
 *		Examples of command byte:
 *		- unsynchronized (answers before the command is processed)
 *		00000000	GET command
 *		00000010	unsynchronized PUT command without data
 *				(equals a DELETE command)
 *		00100010	unsynchronized PUT command, with data
 *				(create or update the given key)
 *		- synchronized (answers once the commande is processed)
 *		00000001	synchronized GET command (same as previous
 *				because GET are always synchronized)
 *		00000011	synchronized PUT command without data
 *				(DELETE command, answers once processed)
 *		00100011	synchronized PUT command, with data
 *				(create or update the given key, answers once processed)
 *		- unsychronized with compression
 *		10000000	GET command, compressed data is accepted
 *		10100010	unsychronized PUT command with compressed data
 *		- synchronized with compression
 *		10000001	GET command, compressed data is accepted
 *		10100011	synchronized PUT command with compressed data
 *
 *		Examples of response byte:
 *		00000000	OK
 *		00100000	OK with data (for a GET command)
 *		10100000	OK with compressed data (for a GET command with allowed compression)
 *		00000010	Protocol error.
 *		00000100	Server error.
 *		00000110	Unknown data.
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/**
 * @typedef	protocol_command_t
 *		List of protocol commands.
 * @constant	PROTO_GET	GET command.
 * @constant	PROTO_PUT	PUT command.
 */
typedef enum protocol_command_e {
	PROTO_GET	= 0,
	PROTO_PUT	= 2
} protocol_command_t;

/**
 * @typedef	protocol_option_t
 *		List of command options.
 * @constant	PROTO_OPT_SYNC		Synchronous command.
 * @constant	PROTO_OPT_COMPRESS	Compression activated.
 * @constant	PROTO_OPT_REPLICATION	Replication command.
 * @constant	PROTO_OPT_DATA		Data is embedded in the message.
 */
typedef enum protocol_option_e {
	PROTO_OPT_SYNC		= 1,	// 0b00000001
	PROTO_OPT_DATA		= 0x20,	// 0b00100000
	PROTO_OPT_REPLICATION	= 0x40,	// 0b01000000
	PROTO_OPT_COMPRESS	= 0x80,	// 0b10000000
} protocol_option_t;

/**
 * @typedef	protocol_response_t
 *		List of response codes.
 * @constant	RESP_OK		OK.
 * @constant	RESP_PROTO	Protocol error.
 * @constant	RESP_SERVER_ERR	Server error.
 * @constant	RESP_NO_DATA	Unknown key.
 */
typedef enum protocol_response_e {
	RESP_OK		= 0,
	RESP_PROTO	= 2,
	RESP_SERVER_ERR	= 4,
	RESP_NO_DATA	= 6
} protocol_response_t;

/** @define REQUEST_COMMAND Extract the command from other options. */
#define REQUEST_COMMAND(c)		(c & 0xe) // (c & 0b00001110)

/** @define REQUEST_HAS_SYNC Extract the sync option from a command. */
#define REQUEST_HAS_SYNC(c)		(c & 1)

/** @define REQUEST_HAS_DATA Extract the data option from a command. */
#define REQUEST_HAS_DATA(c)		(c & 0x20) // (c & 0b01000000)

/** @define REQUEST_HAS_REPLICATION Extract the replication option from a command. */
#define REQUEST_HAS_REPLICATION(c)	(c & 0x40) // (c & 0b01000000)

/** @define REQUEST_HAS_COMPRESS Extract the compression option from a command. */
#define REQUEST_HAS_COMPRESS(c)		(c & 0x80) // (c & 0b10000000)

/** @define RESPONSE_ADD_DATA Add the data option to a response code. */
#define RESPONSE_ADD_DATA(c)		(c | 0x40) // (c & 0b01000000)

#endif /* __PROTOCOL_H__ */
