#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <syslog.h>
#include "ydefs.h"
#include "ystr.h"
#define YLOG_IS_YLOG
#include "ylog.h"

/* ****** global log variable ******* */
ylog_main_t _ylog_gl = {NULL, YLOG_STDERR, NULL, NULL, NULL, NULL,
			YLOG_WARN, 524288, NULL, LOG_DAEMON};

/*
** ylog_init()
** Initialize the global yLog structure. Can be used many times in
** the same process.
*/
void ylog_init(int setup, char *filename, char *progname,
	       unsigned int max_log_size)
{
  char *pt = NULL;
  int i = 0;

  if (_ylog_gl.setup & YLOG_FILE && _ylog_gl.file)
    fclose(_ylog_gl.file);
  YFREE(_ylog_gl.progname);
  YFREE(_ylog_gl.modules);
  YFREE(_ylog_gl.filename);
  YFREE(_ylog_gl.identname);
  _ylog_gl.prio = YLOG_WARN;
  _ylog_gl.setup = setup;
  _ylog_gl.file = NULL;
  _ylog_gl.facility = LOG_DAEMON;
  _ylog_gl.max_log_size = (max_log_size && max_log_size < KB100) ?
    KB100 : max_log_size;
  if (progname && strlen(progname))
    {
      if ((pt = strrchr(progname, SLASH)) && strlen(pt + 1))
	progname = pt + 1;
      if ((_ylog_gl.progname = YMALLOC(strlen(progname) + 1)))
	strcpy(_ylog_gl.progname, progname);
    }
  if (setup & YLOG_FILE)
    {
      if (filename && (_ylog_gl.filename = YMALLOC(strlen(filename) + 1)) &&
	  (_ylog_gl.file = fopen(filename, "a")))
	strcpy(_ylog_gl.filename, filename);
      else
	{
	  YFREE(_ylog_gl.filename);
	  _ylog_gl.setup ^= YLOG_FILE;
	  _ylog_gl.setup |= YLOG_STDERR;
	  _ylog_gl.file = NULL;
	  YLOG_ADD(YLOG_ERR, "Problem with file '%s'", filename);
	}
    }
  if ((pt = getenv("YLOG_MODULES")) && (i = strlen(pt)) &&
      (_ylog_gl.modules = YMALLOC(i + 1)))
    strcpy(_ylog_gl.modules, pt);
  else
    _ylog_gl.modules = NULL;
}

/*
** ylog_set_prio()
** Setup the priority level of the current initialized yLog.
*/
void ylog_set_prio(ylog_priority_t prio)
{
  _ylog_gl.prio = prio;
}

/*
** ylog_set_logsize()
** Set the maximum size of log files. Inifinite size if set to 0.
*/
void ylog_set_logsize(unsigned int max_log_size)
{
  _ylog_gl.max_log_size = (max_log_size && max_log_size < KB100) ?
    KB100 : max_log_size;
}

/*
** ylog_set_handler()
** Set the function pointer to the log handler. This function will be
** called each time a log is added.
*/
void ylog_set_handler(void (*f)(const char*))
{
  _ylog_gl.handler = f;
}

/*
** ylog_set_identname()
** Set the identity name used for syslog.
*/
void ylog_set_identname(const char *identname)
{
  YFREE(_ylog_gl.identname);
  if (identname)
    _ylog_gl.identname = strdup(identname);
}

/*
** ylog_set_facility()
** Set the syslog facility.
*/
void ylog_set_facility(int facility)
{
  _ylog_gl.facility = facility;
}

/*
** ylog_write()
** Write a message to log.
*/
ybool_t ylog_write(ylog_priority_t prio, const char *file, int line,
		   const char *funcname, const char *str, ...)
{
  time_t current_time;
  struct tm *tm;
  char *msg[] = {"DEBUG", "INFO", "NOTE", "WARN", "ERR", "CRIT"};
  va_list plist;
  char *tmpstr, *tmp2;
  FILE *tmp_file;
  ybool_t res = YTRUE;
  int i;

  if (prio < _ylog_gl.prio)
    return (YFALSE);
  va_start(plist, str);
  current_time = time(NULL);
  tm = localtime(&current_time);
  tmpstr = ys_new("");
  tmp2 = ys_new("");
  /* update log structure for consistency */
  if (_ylog_gl.setup & YLOG_FILE && !_ylog_gl.file)
    {
      _ylog_gl.setup |= YLOG_STDERR;
      _ylog_gl.setup ^= YLOG_FILE;
    }
  if (_ylog_gl.setup & YLOG_HANDLER && !_ylog_gl.handler)
    {
      _ylog_gl.setup |= YLOG_STDERR;
      _ylog_gl.setup ^= YLOG_HANDLER;
    }
  /* create log string */
  ys_printf(&tmpstr, "(%s|%d)[%s] %s: %s", file ? file : "", line,
	    msg[(int)prio], funcname ? funcname : "", str ? str : "");
  ys_vprintf(&tmp2, tmpstr, plist);
  va_end(plist);
  ys_trunc(tmpstr);
  /* process output to syslog */
  if (_ylog_gl.setup & YLOG_SYSLOG)
    {
      openlog(_ylog_gl.identname ? _ylog_gl.identname :
	      _ylog_gl.progname ? _ylog_gl.progname : "", 0,
	      _ylog_gl.facility);
      syslog(prio == YLOG_DEBUG ? LOG_DEBUG :
	     prio == YLOG_INFO ? LOG_INFO :
	     prio == YLOG_NOTE ? LOG_NOTICE :
	     prio == YLOG_WARN ? LOG_WARNING :
	     prio == YLOG_ERR ? LOG_ERR :
	     prio == YLOG_CRIT ? LOG_CRIT : LOG_DEBUG,
	     "%s", tmp2);
      closelog();
      if (_ylog_gl.setup == YLOG_SYSLOG)
	{
	  ys_del(&tmp2);
	  ys_del(&tmpstr);
	  return (YTRUE);
	}
    }
  /* create extended log string */
  ys_printf(&tmpstr, "%04d-%02d-%02d %02d:%02d:%02d %s%s\n",
	    tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
	    tm->tm_hour, tm->tm_min, tm->tm_sec,
	    _ylog_gl.progname ? _ylog_gl.progname : "", tmp2);
  ys_del(&tmp2);
  /* process output to handler */
  if (_ylog_gl.setup & YLOG_HANDLER)
    _ylog_gl.handler(tmpstr);
  /* process output to stderr */
  if (_ylog_gl.setup & YLOG_STDERR)
    fputs(tmpstr, stderr);
  /* process output to file */
  if (_ylog_gl.setup & YLOG_FILE)
    {
      if (fputs(tmpstr, _ylog_gl.file) < 0)
	{
	  fclose(_ylog_gl.file);
	  _ylog_gl.file = NULL;
	  _ylog_gl.setup |= YLOG_STDERR;
	  _ylog_gl.setup ^= YLOG_FILE;
	  YLOG_ADD(YLOG_ERR, "Problem to write log to file '%s'",
		   _ylog_gl.filename);
	  res = YFALSE;
	}
      fflush(_ylog_gl.file);
      if (_ylog_gl.max_log_size)
	{
	  /* cut the logs in multiple files */
	  struct stat st;
	  if (!fstat(fileno(_ylog_gl.file), &st) &&
	      st.st_size >= _ylog_gl.max_log_size)
	    {
	      tmp_file = _ylog_gl.file;
	      _ylog_gl.file = 0;
	      if (fclose(tmp_file))
		YLOG_ADD(YLOG_WARN, "Unable to close file");
	      tmpstr = ys_new("");
	      /* search an usable file name */
	      for (i = 0; ; ++i)
		{
		  ys_printf(&tmpstr, "%s-%04d%02d%02d-%02d%02d%02d-%d",
			    _ylog_gl.filename, tm->tm_year + 1900,
			    tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
			    tm->tm_min, tm->tm_sec, i);
		  if (stat(tmpstr, &st))
		    break ;
		}
	      /* move the current log file */
	      rename(_ylog_gl.filename, tmpstr);
	      if (!(_ylog_gl.file = fopen(_ylog_gl.filename, "w")))
		{
		  _ylog_gl.setup |= YLOG_STDERR;
		  _ylog_gl.setup ^= YLOG_FILE;
		  YLOG_ADD(YLOG_ERR, "Unable to open file '%s'",
			   _ylog_gl.filename);
		}
	    }
	}
    }
  ys_del(&tmpstr);
  return (res);
}

/*
** ylog_check_module()
** Write a message to log, only the declared subsystems.
*/
ybool_t ylog_check_module(char *module)
{
  char *pt, c;

  if (!module || !_ylog_gl.modules)
    return (YTRUE);
  if (!(pt = strstr(_ylog_gl.modules, module)))
    return (YFALSE);
  c = *(pt + strlen(module));
  if (!IS_SPACE(c) && c != '\0' && c != ',' && c != ';' && c != ':')
    return (YFALSE);
  return (YTRUE);
}

/*
** ylog_close()
** Close a log session. If some ylog_write() are called after, they will be
** redirected to standard output.
*/
void ylog_close(ylog_priority_t prio)
{
  YFREE(_ylog_gl.filename);
  YFREE(_ylog_gl.modules);
  YFREE(_ylog_gl.progname);
  YFREE(_ylog_gl.identname);
  if (_ylog_gl.setup & YLOG_FILE && _ylog_gl.file)
    fclose(_ylog_gl.file);
  _ylog_gl.facility = LOG_DAEMON;
  _ylog_gl.handler = NULL;
  _ylog_gl.setup = YLOG_STDERR;
  _ylog_gl.file = stderr;
  _ylog_gl.prio = prio;
}
