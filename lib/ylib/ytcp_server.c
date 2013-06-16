#include <linux/limits.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ydefs.h"
#include "yvect.h"
#include "ylog.h"
#include "ytcp_server.h"

/* ******************** Private prototypes ****************** */
static void *_ytcp_server_thread_handle(void *param);
static yerr_t _ytcp_server_thread_launch(ytcp_server_t *server, int fd);

/*
** ytcp_server_init()
** Initialize a yTCP server.
*/
ytcp_server_t *ytcp_server_init(unsigned int nbr_threads,
				void *(*f)(void*), void *data)
{
  ytcp_thread_t *thread;
  yvect_t threads;
  ytcp_server_t *server;
  unsigned int i;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  nbr_threads = (nbr_threads == 0) ? _YTCP_NBR_THREADS : nbr_threads;
  threads = yv_new();
  server = YMALLOC(sizeof(ytcp_server_t));
  server->threads_func = f;
  server->nbr_threads = nbr_threads;
  server->first_waiting = 0;
  server->vect_threads = threads;
  server->sd = -1;
  server->run_loop = 1;
  server->purge_cnt = _YTCP_PURGE_COUNTER;
  for (i = 0; i < nbr_threads && i < 1024; ++i)
    {
      thread = YMALLOC(sizeof(ytcp_thread_t));
      pthread_mutex_init(&(thread->mut_do), NULL);
      pthread_mutex_lock(&(thread->mut_do));
      if (pthread_create(&(thread->tid), 0, _ytcp_server_thread_handle,
			 thread))
	{
	  YLOG_ADD(YLOG_WARN, "Problem during thread creation");
	  thread->state = YTCP_CLOSE;
	}
      thread->fd = -1;
      thread->state = YTCP_WAIT;
      thread->data = data;
      thread->server = server;
      yv_add(&threads, thread);
    }
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
  return (server);
}

/*
** ytcp_server_start()
** Start a yTCP server.
*/
yerr_t ytcp_server_start(ytcp_server_t *server, int port)
{
  struct sockaddr_in addr;
  unsigned int addr_size;
  int fd;
  const int on = 1;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  if ((server->sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      YLOG_ADD(YLOG_ERR, "Socket error");
      return (YEIO);
    }
  if (setsockopt(server->sd, SOL_SOCKET, SO_REUSEADDR, (void*)&on, sizeof(on)) < 0)
    YLOG_ADD(YLOG_WARN, "setsockopt(SO_REUSEADDR) failed");
  if (setsockopt(server->sd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on, sizeof(on)) < 0)
    YLOG_ADD(YLOG_WARN, "setsockopt(SO_KEEPALIVE) failed");
  addr_size = sizeof(addr);
  memset(&addr, 0, addr_size);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (bind(server->sd, (struct sockaddr*)&addr, addr_size) < 0)
    {
      YLOG_ADD(YLOG_CRIT, "Bind error");
      return (YEBADF);
    }
  if (listen(server->sd, SOMAXCONN))
    {
      YLOG_ADD(YLOG_CRIT, "Listen error");
      return (YEBADF);
    }
  while (server->run_loop)
    {
      if ((fd = accept(server->sd, (struct sockaddr*)&addr, &addr_size)) < 0)
	continue ;
      if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void*)&on, sizeof(on)) < 0)
	YLOG_ADD(YLOG_WARN, "setsockopt(KEEPALIVE) failed");
      if (_ytcp_server_thread_launch(server, fd) != YENOERR)
	close(fd);
    }
  close(server->sd);
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
  return (YENOERR);
}

/*
** ytcp_server_purge_threads()
** Do a garbage collecting of threads in YTCP_CLOSE state.
*/
void ytcp_server_purge_threads(ytcp_server_t *server)
{
  ytcp_thread_t *thread;
  int i;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  for (i = server->nbr_threads; i; --i)
    {
      thread = (ytcp_thread_t*)(server->vect_threads[i - 1]);
      if (thread->state == YTCP_CLOSE || thread->state == YTCP_NONE)
	{
	  thread = yv_ext(server->vect_threads, i);
	  thread->state = YTCP_CLOSE;
	  pthread_mutex_unlock(&(thread->mut_do));
	  pthread_join(thread->tid, NULL);
	  pthread_mutex_destroy(&(thread->mut_do));
	  YFREE(thread);
	  server->nbr_threads--;
	}
    }
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
}

/*
** ytcp_server_set_nbr_threads()
** Set the number of threads to a given number. If there is more running
** threads than wanted number, only the waiting threads are ended. If there
** is less threads than desired, new ones are created.
*/
yerr_t ytcp_server_set_nbr_threads(ytcp_server_t *server, int nbr)
{
  ytcp_thread_t *thread;
  int i;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  if (nbr == server->nbr_threads)
    return (YENOERR);
  if (server->nbr_threads > nbr)
    {
      for (i = server->nbr_threads; i; --i)
	{
	  thread = (ytcp_thread_t*)(server->vect_threads[i - 1]);
	  if (thread->state != YTCP_RUN)
	    {
	      thread = yv_ext(server->vect_threads, i);
	      thread->state = YTCP_CLOSE;
	      pthread_mutex_unlock(&(thread->mut_do));
	      pthread_join(thread->tid, NULL);
	      pthread_mutex_destroy(&(thread->mut_do));
	      YFREE(thread);
	      server->nbr_threads--;
	    }
	}
    }
  while (server->nbr_threads < nbr)
    {
      thread = YMALLOC(sizeof(ytcp_thread_t));
      pthread_mutex_init(&(thread->mut_do), NULL);
      pthread_mutex_lock(&(thread->mut_do));
      thread->server = server;
      thread->data = ((ytcp_thread_t*)(server->vect_threads[0]))->data;
      if (pthread_create(&(thread->tid), 0, _ytcp_server_thread_handle,
			 thread))
	{
	  YLOG_ADD(YLOG_ERR, "Problem during thread creation");
	  thread->state = YTCP_CLOSE;
	  thread->fd = -1;
	  return (YEAGAIN);
	}
      yv_add(&(server->vect_threads), thread);
      server->nbr_threads++;
      thread->fd = -1;
      thread->state = YTCP_WAIT;
    }
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
  return (YENOERR);
}

/*
** ytcp_server_stop()
** Stop a yTCP server. The server stop to loop and close its listening socket.
*/
yerr_t ytcp_server_stop(ytcp_server_t *server)
{
  server->run_loop = 0;
  return (YENOERR);
}

/*
** ytcp_server_delete()
** Delete a yTCP server and all its threads.
*/
ytcp_server_t *ytcp_server_delete(ytcp_server_t *server,
				  unsigned short wait_threads)
{
  ytcp_thread_t *thread;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  if (!server || !server->vect_threads)
    {
      YLOG_ADD(YLOG_NOTE, "Server doesn't exist.");
      return (NULL);
    }
  while ((thread = (ytcp_thread_t*)yv_get(server->vect_threads)))
    {
      if (wait_threads && thread->state == YTCP_RUN)
	{
	  while (thread->state == YTCP_RUN)
	    usleep(500);
	}
      thread->state = YTCP_CLOSE;
      pthread_mutex_unlock(&(thread->mut_do));
      pthread_join(thread->tid, NULL);
      pthread_mutex_destroy(&(thread->mut_do));
      YFREE(thread);
      server->nbr_threads--;
    }
  yv_del(&(server->vect_threads), NULL, NULL);
  YFREE(server);
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
  return (NULL);
}

/*
** _ytcp_server_thread_handle() -- PRIVATE FUNCTION
** Callback function executed by all server's threads. Loop to check if the
** thread must handle a new connection, or if it must die.
*/
void *_ytcp_server_thread_handle(void *param)
{
  ytcp_thread_t *thread = param;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  for (; ; )
    {
      if (!pthread_mutex_lock(&(thread->mut_do)))
	{
	  if (thread->state == YTCP_CLOSE)
	    {
	      YLOG_MOD("ytcp", YLOG_DEBUG, "End of thread");
	      pthread_exit(NULL);
	    }
	  else
	    {
	      YLOG_MOD("ytcp", YLOG_DEBUG, "Start of connection");
	      thread->server->threads_func(thread);
	      if (thread->stream)
		fclose(thread->stream);
	      else if (thread->fd > -1)
		close(thread->fd);
	      thread->stream = NULL;
	      thread->fd = -1;
	      thread->state = YTCP_WAIT;
	      YLOG_MOD("ytcp", YLOG_DEBUG, "End of connection");
	    }
	}
    }
  YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
  return (NULL);
}

/*
** _ytcp_server_thread_launch() -- PRIVATE FUNCTION
** Use a waiting thread to handle an incoming connection.
*/
static yerr_t _ytcp_server_thread_launch(ytcp_server_t *server, int fd)
{
  ytcp_thread_t *thread;
  int i;

  YLOG_MOD("ytcp", YLOG_DEBUG, "Entering");
  if (!server->purge_cnt)
    {
      ytcp_server_purge_threads(server);
      server->purge_cnt = _YTCP_PURGE_COUNTER;
    }
  /* search a waiting thread */
  if (server->first_waiting >= 0)
    {
      thread = (ytcp_thread_t*)(server->vect_threads[server->first_waiting]);
      thread->fd = fd;
      thread->stream = fdopen(fd, "rb+");
      setvbuf(thread->stream, NULL, _IONBF, 0);
      thread->state = YTCP_RUN;
      pthread_mutex_unlock(&(thread->mut_do));
      for (i = 0; i < server->nbr_threads; ++i)
	{
	  thread = (ytcp_thread_t*)(server->vect_threads[i]);
	  if (thread->state == YTCP_WAIT)
	    {
	      server->first_waiting = i;
	      break;
	    }
	}
      if (i == server->nbr_threads)
	server->first_waiting = -1;
      YLOG_MOD("ytcp", YLOG_DEBUG, "Exiting");
      return (YENOERR);
    }
  /* update 'first_waiting' */
  for (i = 0; i < server->nbr_threads; ++i)
    {
      thread = (ytcp_thread_t*)(server->vect_threads[i]);
      if (thread->state == YTCP_WAIT)
	{
	  server->first_waiting = i;
	  break;
	}
    }
  if (i != server->nbr_threads)
    {
      YLOG_MOD("ytcp", YLOG_DEBUG, "Recall the function");
      return (_ytcp_server_thread_launch(server, fd));
    }
  /* no one waiting thread - create a new one */
  thread = YMALLOC(sizeof(ytcp_thread_t));
  pthread_mutex_init(&(thread->mut_do), NULL);
  pthread_mutex_lock(&(thread->mut_do));
  thread->server = server;
  thread->data = ((ytcp_thread_t*)(server->vect_threads[0]))->data;
  if (pthread_create(&(thread->tid), 0, _ytcp_server_thread_handle, thread))
    {
      YLOG_ADD(YLOG_ERR, "Problem during thread creation");
      thread->state = YTCP_CLOSE;
      thread->fd = -1;
      pthread_mutex_unlock(&(thread->mut_do));
      pthread_mutex_destroy(&(thread->mut_do));
      return (YEAGAIN);
    }
  yv_add(&(server->vect_threads), thread);
  server->nbr_threads++;
  thread->fd = fd;
  thread->state = YTCP_RUN;
  pthread_mutex_unlock(&(thread->mut_do));
  YLOG_MOD("ytcp", YLOG_DEBUG, "New thread created - Exiting");
  return (YENOERR);
}
