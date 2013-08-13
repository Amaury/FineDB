#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command_ping.h"
#include "protocol.h"
#include "database.h"

/* Process a PING command. */
yerr_t command_ping(tcp_thread_t *thread) {
	YLOG_ADD(YLOG_DEBUG, "PING command");
	return (CONNECTION_SEND_OK(thread));
}
