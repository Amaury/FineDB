#include "command_put.h"
#include "protocol.h"

/* Process a PUT command. */
yerr_t command_put(tcp_thread_t *thread, char *buff, size_t buffsz) {
	return (connection_send_response(thread->fd, RESP_OK, NULL, 0));
}
