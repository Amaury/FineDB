#include <arpa/inet.h>
#include <string.h>
#include "nanomsg/nn.h"
#include "snappy.h"
#include "ylog.h"
#include "ybin.h"
#include "command.h"
#include "protocol.h"
#include "database.h"

/* Process a PING command. */
yerr_t command_ping(tcp_thread_t *thread, ybool_t sync, ybool_t compress, ybool_t serialized, ydynabin_t *buff) {
	YLOG_ADD(YLOG_DEBUG, "PING command");
	return (CONNECTION_SEND_OK(thread));
}
