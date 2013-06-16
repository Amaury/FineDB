/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ytcp_server.h
 * @abstract	All definitions needed to create a TCP/IP server.
 * @discussion	The yTCP layer is a convenient way to create mutli-threaded
 *		TCP/IP servers.
 *		<ul>
 *		<li>First, you initialize the server environment.
 *			<pre>ytcp_server_t *server;</pre>
 *			<pre>server = ytcp_server_init(5, tfunc, NULL);</pre>
 *			<ul>
 *			<li>The first parameter is the minimum number of 
 *			threads to create.</li>
 *			<li>The second parameter is a pointer to a callback 
 *			funtion. It will be called when a connection is 
 *			received.</li>
 *			<li>The third parameter is a pointer to a data that 
 *			will be given to the callback function when it will be
 *			called.</li>
 *			</ul>
 *		Each time a connection is received, the server wake up a 
 *		thread. The thread's callback function is executed, getting a
 *		pointer to a ytcp_thread_t structure as parameter. Once its 
 *		treatments are done, the callback function just have to return
 *		a NULL value.<br /><br />
 *		The thread callback function must have this prototype:
 *			<pre>void *function(void *parameter);</pre>
 *		The return value is not readed. Return NULL. The given 
 *		parameter is, in fact, a pointer to the thread structure. This
 *		structure is *private*. Use these macros to handle its content:
 *			<ul>
 *			<li>int socket = YTCP_THREAD_SOCK(parameter);</li>
 *			<li>FILE *stream = YTCP_THREAD_STREAM(parameter);</li>
 *			<li>void *data = YTCP_THREAD_DATA(parameter);</li>
 *			</ul>
 *		As you can see, there is 2 different way to use the TCP/IP
 *		connection: reading and writing to the socket directly (with
 *		read() and write() system calls), or using the stream (allowing
 *		use of fprintf(), fscanf(), fgets(), ...). The stream is 
 *		unbuffered, so you don't have to use fflush() on it after each
 *		writing.<br /><br />
 *		You don't have to close the thread's socket or stream. The
 *		connection is automatically closed when your thread handler
 *		returns.
 *		<br />
 *		</li>
 *		<li>After, you start the server loop.
 *			<pre>ytcp_server_start(server, port_number);</pre>
 *		The server will wait for new connections on the given port 
 *		number, and treat them.<br /><br />
 *		</li>
 *		<li>The server loop can be stopped (by any threads):
 *			<pre>ytcp_server_stop(server);</pre>
 *		</li>
 *		<li>Don't forget to destroy the threads and free the allocated
 *		memory.
 *			<pre>ytcp_server_delete(server, 0);</pre>
 *		</li>
 *		</ul>
 *		Important: the yTCP layer use threads, so programs using it 
 *		MUST be reentrant. Take care about globals modified by all
 *		threads in the same time (for example, don't trust the errno
 *		global).
 * @version	1.0.0 Jun 26 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YTCP_SERVER_H__
#define __YTCP_SERVER_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <pthread.h>
#include "yerror.h"
#include "yvect.h"

/*! @define YTCP_THREAD_SOCK Give the socket descriptor of a thread structure.
 * Take a pointer to the thread structure as parameter. */
#define YTCP_THREAD_SOCK(t)	(((ytcp_thread_t*)t)->fd)

/*! @define YTCP_THREAD_STREAM Give the stream (type: FILE*) that correspond to
 * the socket. Take a pointer to the thread structure as parameter. */
#define YTCP_THREAD_STREAM(t)	(((ytcp_thread_t*)t)->stream)

/*! @define YTCP_THREAD_DATA Give the internal data pointer of a thread 
 * structure. Take a pointer to the thread strycture as parameter. */
#define YTCP_THREAD_DATA(t)	(((ytcp_thread_t*)t)->data)

/*! @define _YTCP_NBR_THREADS Default number of threads launched at startup.
 * Internal macro. Don't use it. */
#define _YTCP_NBR_THREADS	15

/*! @define _YTCP_PURGE_COUNTER Set the purge counter. Internal macro. Don't
 * use it. */
#define _YTCP_PURGE_COUNTER	(server->nbr_threads * 300)

/*!
 * @enum	ytcp_state_e
 *		Definition of the state of a TCP connection.
 * @constant	YTCP_NONE	Connection not set yet.
 * @constant	YTCP_WAIT	The thread exists, waiting for an incoming
 *				connection.
 * @constant	YTCP_RUN	The connection is established and running.
 * @constant	YTCP_CLOSE	The connection and the socket are closed. An
 *				error may have occured.
 */
enum ytcp_state_e
{
  YTCP_NONE = 0,
  YTCP_WAIT,
  YTCP_RUN,
  YTCP_CLOSE
};

/*! @typedef ytcp_state_t TCP states. See ytcp_state_e enumeration. */
typedef enum ytcp_state_e ytcp_state_t;

/*!
 * @struct	ytcp_thread_s
 *		All data needed by a server thread that handle a connection.
 *		The code executed by a thread can read these data, but not
 *		modify them (except the 'data' field).
 * @field	tid		The thread identificator.
 * @field	fd		The socket file descriptor.
 * @field	stream		Stream corresponding to the socket descriptor.
 * @field	mut_do		The mutex which stops the thread until it can
 *				do something (run a connection or die).
 * @field	state		The current state of the thread and its
 *				connection.
 * @field	data		Pointer to data (used by the thread's
 *				callback).
 * @field	server		Pointer to the server structure.
 */
struct ytcp_thread_s
{
  pthread_t tid;
  int fd;
  FILE *stream;
  pthread_mutex_t mut_do;
  ytcp_state_t state;
  void *data;
  struct ytcp_server_s *server;
};

/*! @typedef ytcp_thread_t Server thread. See ytcp_thread_s struct. */
typedef struct ytcp_thread_s ytcp_thread_t;

/*!
 * @struct	ytcp_server_s
 *		The TCP/IP server object.
 * @field	threads_func	Pointer to the function called when a thread 
 *				have to process some incoming data.
 * @field	nbr_threads	The total number of threads.
 * @field	first_waiting	Number of the first known free thread that can
 *				handle new connection.
 * @field	vect_threads	A vector of threads.
 * @field	sd		Descriptor of the connection socket.
 * @field	run_loop	Set to 1 when the serveur must be running (and
 *				loop on incoming connections), and set to 0
 *				when the server must stop.
 * @field	purge_cnt	Counter decremented at each connection receive.
 *				When equal to zero, the threads are purged to
 *				eliminate ones with YTCP_CLOSE state.
 */
struct ytcp_server_s
{
  void *(*threads_func)(void*);
  int nbr_threads;
  int first_waiting;
  yvect_t vect_threads;
  int sd;
  short run_loop;
  int purge_cnt;
};

/*! @typedef ytcp_server_t Server object. See ytcp_server_s struct. */
typedef struct ytcp_server_s ytcp_server_t;

/*!
 * @function	ytcp_server_init
 *		Initialize a yTCP server.
 * @param	nbr_threads	Initial number of running threads. If set to 0,
 *				takes the YTCP_NBR_THREADS value.
 * @param	f		Function executed by the server's threads when
 *				an incoming connection is established.
 * @param	data		Pointer given in parameter to the handler.
 * @return	A pointer to the yTCP server structure.
 */
ytcp_server_t *ytcp_server_init(unsigned int nbr_threads, void *(*f)(void*),
				void *data);

/*!
 * @function	ytcp_server_start
 *		Start a yTCP server.
 * @param	server	A pointer to the server structure.
 * @param	port	The port number to bind with.
 * @return	YENOERR if OK.
 */
yerr_t ytcp_server_start(ytcp_server_t *server, int port);

/*!
 * @function	ytcp_server_purge_threads
 *		Do a garbage collecting of threads in YTCP_CLOSE state.
 * @param	server	A pointer to the server structure.
 */
void ytcp_server_purge_threads(ytcp_server_t *server);

/*!
 * @function	ytcp_server_set_nbr_threads
 *		Set the number of threads to a given number. If there is more
 *		running threads than wanted number, only the waiting threads
 *		are ended. If there is less threads than desired, new ones are
 *		created.
 * @param	server	A pointer to the server structure.
 * @param	nbr	The wanted number of threads.
 * @return	YENOERR if OK.
 */
yerr_t ytcp_server_set_nbr_threads(ytcp_server_t *server, int nbr);

/*!
 * @function	ytcp_server_stop
 *		Stop a yTCP server. The server stop to loop and close its
 *		listening socket.
 * @param	server	A pointer to the server structure.
 * @return	YENOERR if OK.
 */
yerr_t ytcp_server_stop(ytcp_server_t *server);

/*!
 * @function	ytcp_server_delete
 *		Delete a yTCP server and all its threads.
 * @param	server		A pointer to the server structure.
 * @param	wait_threads	If set to 1, the server will wait the normal
 *				end of each running threads ; if set to 0, it
 *				will end immediately.
 * @return	NULL.
 */
ytcp_server_t *ytcp_server_delete(ytcp_server_t *server,
				  unsigned short wait_threads);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YTCP_SERVER_H__ */
