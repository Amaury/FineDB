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

/** @define PROTOCOL_COMMAND Extract the command from other options. */
#define PROTOCOL_COMMAND(c)	(c & 0x3f) // (c & 0b00111111)

/** @define PROTOCOL_COMPRESS Extract the compression option from a command. */
#define PROTOCOL_COMPRESS(c)	(c & 0x80) // (c & 0b10000000)

/** @define PROTOCOL_REPLICATION Extract the replication option from a command. */
#define PROTOCOL_REPLICATION(c)	(c & 0x40) // (c & 0b01000000)

#endif /* __PROTOCOL_H__ */
