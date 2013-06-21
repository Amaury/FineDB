#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/**
 * @typedef	protocol_command_t
 *		List of protocol commands.
 * @constant	PROTO_PUT	PUT command.
 * @constant	PROTO_GET	GET command.
 * @constant	PROTO_DEL	DEL command.
 */
typedef enum protocol_command_e {
	PROTO_PUT = 1,
	PROTO_GET = 2,
	PROTO_DEL = 3
} protocol_command_t;

/**
 * @typedef	protocol_response_t
 *		List of response codes.
 * @constant	RESP_OK		OK.
 * @constant	RESP_BAD_CMD	Bad command.
 * @constant	RESP_PROTO	Protocol error.
 */
typedef enum protocol_response_e {
	RESP_OK = 0,
	RESP_BAD_CMD = 1,
	RESP_PROTO = 2
} protocol_response_t;

/** @define REQUEST_COMMAND Extract the command from other options. */
#define REQUEST_COMMAND(c)	(c & 0x3f) // (c & 0b00111111)

/** @define REQUEST_HAS_COMPRESS Extract the compression option from a command. */
#define REQUEST_HAS_COMPRESS(c)	(c & 0x80) // (c & 0b10000000)

/** @define REQUEST_HAS_REPLICATION Extract the replication option from a command. */
#define REQUEST_HAS_REPLICATION(c)	(c & 0x40) // (c & 0b01000000)

/** @define RESPONSE_ADD_DATA Add the data option to a response code. */
#define RESPONSE_ADD_DATA(c)	(c | 0x40) // (c & 0b01000000)

#endif /* __PROTOCOL_H__ */
