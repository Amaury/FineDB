/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	ylog.h
 * @abstract	All definitions to use log files.
 * @discussion	The yLogs are a usefull mean to write logs.
 *		<ul>
 *		<li>You initialize the logs. There is four initialization 
 *		types, to specify the logs destination: the standard error 
 *		output, the system logger (syslog), to a given file, or to a 
 *		callback function (to send the logs through network, for 
 *		example). Here are some examples:
 *			<ul>
 *			<li>YLOG_INIT_STDERR();</li>
 *			<li>YLOG_INIT_SYSLOG();</li>
 *			<li>YLOG_INIT_FILE("/var/mylog");</li>
 *			<li>YLOG_INIT_HANDLER(pointer_to_function);</li>
 *			</ul><br />
 *		The init must be done in the main() function, which must have 
 *		its second parameter called "argv". The default output (without
 *		init) is set to the standard error output.<br /><br />
 *		</li>
 *		<li>It is possible to set a default priority level. This is the
 *		minimal level needed for a log to be written in log file (or 
 *		written on stderr). There is five levels (in growing order):
 *			<ul>
 *			<li>DEBUG: debug information for source code that 
 *			should be debugged</li>
 *			<li>INFO: information, messages during debug time</li>
 *			<li>NOTE: notice, normal but significant</li>
 *			<li>WARN: warning, non-blocking problem</li>
 *			<li>ERR: error, doesn't work as it should</li>
 *			<li>CRIT: critical, the program can't continue</li>
 *			</ul>
 *		Some macros helps you to set the default priority level:
 *			<ul>
 *			<li>YLOG_SET_DEBUG();</li>
 *			<li>YLOG_SET_INFO();</li>
 *			<li>YLOG_SET_NOTE();</li>
 *			<li>YLOG_SET_WARN();</li>
 *			<li>YLOG_SET_ERR();</li>
 *			<li>YLOG_SET_CRIT();</li>
 *			</ul>
 *		A normal running program will be set to WARN or ERR, thus at 
 *		debug time you will set to INFO (or DEBUG if you want to trace
 *		all the program). The default level is set to WARN.<br /><br />
 *		</li>
 *		<li>It is also possible, when the logs are written on stderr,
 *		to set the maximum log files size:
 *			<ul>
 *			<li>YLOG_SIZE_MINI();</li>
 *			<li>YLOG_SIZE_NORM();</li>
 *			<li>YLOG_SIZE_BIG();</li>
 *			<li>YLOG_SIZE_HUGE();</li>
 *			</ul>
 *		The size are 100 KO, 512 KO (default size), 2 MO and 10 MO. 
 *		When a file is full, it is moved with the date concatenate to 
 *		its name, and a new log file is created.
 *		<br /><br />
 *		</li>
 *		<li>You can add a log entry with this command:
 *			<ul>
 *			<li>YLOG_ADD(YLOG_ERR, "problem %d", 5);</li>
 *			</ul>
 *		As you can see, this macro takes a priority level, a string 
 *		that describes the log entry itself, and as many additional 
 *		parameters as needed (like in printf()). If the log's priority
 *		level is lower than the current default one, the log entry 
 *		isn't added.<br />
 *		There is another command, formerly created for temporary logs;
 *		for example, when you want to add some log entries for debug,
 *		you can put a simple:
 *			<ul>
 *			<li>YLOG("step 1");</li>
 *			<li>YLOG("step 2: '%s'", str);</li>
 *			</ul>
 *		The logs will be written with the minimal needed priority 
 *		level.<br /><br />
 *		</li>
 *		<li>At the end of the program, you close the opened log with 
 *		the command:
 *			<ul>
 *			<li>YLOG_END();</li>
 *			</ul>
 *		If some logs are added after this command, they will be 
 *		redirected to the standard error output.<br /><br />
 *		</li>
 *		<li><u>Additionnal feature: treatment by module</u><br />
 *		You can have some parts of a program that should not, usually,
 *		write logs, but sometimes they should. The priority level is
 *		not enough all the time to separate the differents parts, so
 *		there is a module treatment system.
 *			<ul>
 *			<li>First of all, you create an environment variable
 *			named 'YLOG_MODULES', that contains a list of module
 *			names (separate with colons) which are allowed to
 *			write logs.</li>
 *			<li>In the modules that should sometimes write some
 *			logs, use this macro to do it:
 *	<pre>YLOG_MOD("module_name", YLOG_PRIO_ERR, "problem %d", i);</pre>
 *			If the module name is not found in the YLOG_MODULES
 *			variable, the log is not written. If it is found, or if
 *			YLOG_MODULES is not defined, it is like a normal call
 *			to YLOG_ADD() (ie: the priority level is checked).
 *			</li>
 *			</ul>
 *		</li>
 *		</ul>
 * @version	1.0.0 Jun 26 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YLOG_H__
#define __YLOG_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

#include <stdio.h>
#include "ydefs.h"

/*! @define KB100 Value of 100 KB. */
#define KB100			102400
/*! @define KB512 Value of 512 KB. */
#define KB512			524288
/*! @define MB2 Value of 2 MB. */
#define MB2			2097152
/*! @define MB10 Value of 10 MB. */
#define MB10			10485760

/*! @define YLOG_INIT_STDERR Initialize to use standard error output. */
#define YLOG_INIT_STDERR()	ylog_init(YLOG_STDERR, NULL, argv[0], KB512)
/*! @define YLOG_INIT_FILE Initialize to use an output file. */
#define YLOG_INIT_FILE(f)	ylog_init(YLOG_FILE, f, argv[0], KB512)
/*! @define YLOG_INIT_SYSLOG Initialize to use the system logger. */
#define YLOG_INIT_SYSLOG()	ylog_init(YLOG_SYSLOG, NULL, argv[0], KB512)
/*! @define YLOG_INIT_HANDLER Initialize to use a log handler. */
#define YLOG_INIT_HANDLER(f)	(ylog_init(YLOG_HANDLER, NULL, argv[0], \
					   KB512), ylog_set_handler(f))
/*! @define YLOG_INIT_STDERR_FILE Initialize to use standard error and
 an output file. */
#define	YLOG_INIT_STDERR_FILE(f)	ylog_init(YLOG_STDERR | YLOG_FILE, \
						  f, argv[0], KB512)
/*! @define YLOG_INIT_STDERR_SYSLOG Initialize to use standard error and
 the system logger ouputs. */
#define	YLOG_INIT_STDERR_SYSLOG()	ylog_init(YLOG_STDERR | YLOG_SYSLOG, \
						  NULL, argv[0], KB512)
/*! @define YLOG_INIT_FILE_SYSLOG Initialize to use an output file and
 the system logger. */
#define	YLOG_INIT_FILE_SYSLOG(f)	ylog_init(YLOG_FILE | YLOG_SYSLOG, \
						  f, argv[0], KB512)
/*! @define YLOG_INIT_STDERR_HANDLER Initialize to use standard error ouput
 and a log handler. */
#define	YLOG_INIT_STDERR_HANDLER(f)	(ylog_init(YLOG_STDERR | YLOG_HANDLER, \
						   NULL, argv[0], KB512), \
					 ylog_set_handler(f))
/*! @define YLOG_INIT_FILE_HANDLER Initialize to use an output file and
 a log handler. */
#define	YLOG_INIT_FILE_HANDLER(fi, fu)	(ylog_init(YLOG_FILE | YLOG_HANDLER, \
						   fi, argv[0], KB512), \
					 ylog_set_handler(fu))
/*! @define YLOG_INIT_SYSLOG_HANDLER Initialize to use the system logger and
 a log handler. */
#define	YLOG_INIT_SYSLOG_HANDLER(f)	(ylog_init(YLOG_SYSLOG | YLOG_HANDLER, \
						   NULL, argv[0], KB512), \
					 ylog_set_handler(f))
/*! @define YLOG_INIT_STDERR_FILE_SYSLOG Initialize to use standard error
 output, an output file and the system logger. */
#define	YLOG_INIT_STDERR_FILE_SYSLOG(f)	ylog_init(YLOG_STDERR | YLOG_FILE | \
						  YLOG_SYSLOG, f, argv[0], \
						  KB512)
/*! @define YLOG_INIT_STDERR_FILE_HANDLER Initialize to use standard error
 output, an output file and a log handler. */
#define	YLOG_INIT_STDERR_FILE_HANDLER(fi, fu)	(ylog_init(YLOG_STDERR | YLOG_FILE | \
							   YLOG_HANDLER, fi, argv[0], \
							   KB512), ylog_set_handler(fu))
/*! @define YLOG_INIT_STDERR_SYSLOG_HANDLER Initialize to use standard error
 output, the system logger and a log handler. */
#define	YLOG_INIT_STDERR_SYSLOG_HANDLER(f)	(ylog_init(YLOG_STDERR | YLOG_SYSLOG | \
							   YLOG_HANDLER, NULL, argv[0], \
							   KB512), ylog_set_andler(f))
/*! @define YLOG_INIT_FILE_SYSLOG_HANDLER Initialize to use an output file,
 the system logger and a log handler. */
#define	YLOG_INIT_FILE_SYSLOG_HANDLER(fi, fu)	(ylog_init(YLOG_FILE | YLOG_SYSLOG | \
							   YLOG_HANDLER, fi, argv[0], \
							   KB512), ylog_set_handler(fu))
/*! @define YLOG_INIT_ALL Initialize to use all output methods. */
#define	YLOG_INIT_ALL(fi, fu)		(ylog_init(YLOG_STDERR | YLOG_FILE | \
						   YLOG_SYSLOG | YLOG_HANDLER, \
						   f, argv[0], KB512), \
					 ylog_set_handler(fu))

/*! @define YLOG_SET_DEBUG Set the default priority to debug level. */
#define YLOG_SET_DEBUG()	ylog_set_prio(YLOG_DEBUG);
/*! @define YLOG_SET_INFO Set the default priority to informationnal level. */
#define YLOG_SET_INFO()		ylog_set_prio(YLOG_INFO)
/*! @define YLOG_SET_NOTE Set the default priority to notice level. */
#define YLOG_SET_NOTE()		ylog_set_prio(YLOG_NOTE)
/*! @define YLOG_SET_WARN Set the default priority to warning level. */
#define YLOG_SET_WARN()		ylog_set_prio(YLOG_WARN)
/*! @define YLOG_SET_ERR Set the default priority to error level. */
#define YLOG_SET_ERR()		ylog_set_prio(YLOG_ERR)
/*! @define YLOG_SET_CRIT Set the default priority to critical level. */
#define YLOG_SET_CRIT()		ylog_set_prio(YLOG_CRIT)

/*! @define YLOG_SIZE_MINI Set the max log size to a minimal value (100 KB). */
#define YLOG_SIZE_MINI()	ylog_set_logsize(KB100)
/*! @define YLOG_SIZE_NORM Set the max log size to a normal value (512 KB). */
#define YLOG_SIZE_NORM()	ylog_set_logsize(KB512)
/*! @define YLOG_SIZE_BIG Set the max log size to a big value (2 MB). */
#define YLOG_SIZE_BIG()		ylog_set_logsize(MB2)
/*! @define YLOG_SIZE_HUGE Set the max log size to a huge value (10 MB). */
#define YLOG_SIZE_HUGE()	ylog_set_logsize(MB10)

/*! @define YLOG Add a simple log at the lowest sufficient level. The parameter
 * could be a simple character string, or a string with several arguments (like
 * in printf()). Return TRUE if the log entry was written, FALSE otherwise. */
# define YLOG(...)		ylog_write(_ylog_gl.prio, __FILE__, __LINE__, \
					   __FUNCTION__, __VA_ARGS__)
/*! @define YLOG_ADD Add a log with a specified priority. The last parameter 
 * could be a simple character string, or a string with several arguments (like
 * in printf()). Return TRUE if the log entry was written, FALSE otherwise. */
#define YLOG_ADD(prio, ...)	ylog_write(prio, __FILE__, __LINE__, \
					   __FUNCTION__, __VA_ARGS__)
/*! @define YLOG_MOD Add a log with a specified priority, only if the given
 * module name is specified in the YLOG_MODULES environment variable. The
 * last parameter could be a simple character string, or a string with several
 * arguments (like in printf()). Return value like YLOG_ADD(). */
#define YLOG_MOD(mod, prio, ...)	(ylog_check_module(mod) ? \
					 ylog_write(prio, __FILE__, __LINE__, \
						    __FUNCTION__, __VA_ARGS__) : 0)

/*! @define YLOG_END Stop the logs. Futur logs will be write on standard error
 * output, with minimal default priority level. */
#define YLOG_END()		ylog_close(YLOG_INFO)

/*!
 * @enum	ylog_type_e
 *		All different destinations possible for logs.
 * @constant	YLOG_STDERR	Logs are written on the standard error output.
 * @constant	YLOG_FILE	Logs are written on a given file.
 * @constant	YLOG_SYSLOG	Logs are written on the system logger.
 * @constant	YLOG_HANDLER	Logs are sent to a given handler.
 */
enum ylog_type_e
{
  YLOG_STDERR	= 1,
  YLOG_FILE	= 2,
  YLOG_SYSLOG	= 4,
  YLOG_HANDLER	= 8
};

/*! @typedef ylog_type_t Log destination. See ylog_type_e enumeration. */
typedef enum ylog_type_e ylog_type_t;

/*!
 * @enum	ylog_priority_e
 *		Several priority levels are defined in yLogs. They are similar
 *		to the syslog's ones (see 'man 3 syslog')
 * @constant	YLOG_DEBUG	Debug-level message (lower priority level).
 * @constant	YLOG_INFO	Informationnal message (default level).
 * @constant	YLOG_NOTE	Notice ; normal but significant.
 * @constant	YLOG_WARN	Warning.
 * @constant	YLOG_ERR	Error.
 * @constant	YLOG_CRIT	Critical message (higher priority level).
 */
enum ylog_priority_e
{
  YLOG_DEBUG = 0,
  YLOG_INFO,
  YLOG_NOTE,
  YLOG_WARN,
  YLOG_ERR,
  YLOG_CRIT
};

/*! @typedef ylog_priority_t Log priority levels. See ylog_priority_e. */
typedef enum ylog_priority_e ylog_priority_t;

/*!
 * @struct	ylog_main_s
 *		Main structure for yLogs. Must have one of it in global space.
 * @field	handler		Function pointer to the log handler (set to 
 *				NULL for logs on stderr, syslog or file).
 * @field	setup		Log type (stderr, syslog, file).
 * @field	filename	Path to the output file.
 * @field	file		Pointer to the output file (for stderr and file
 *				output).
 * @field	progname	Name of the current program.
 * @field	identname	Identity name used for syslog.
 * @field	prio		Current minimum priority level of written logs.
 * @field	max_log_size	Maximum size of log files (in bytes). Infinite
 *				size if set to 0.
 * @field	modules		Copy of the YLOG_MODULES environment variable.
 * @field	facility	Syslog facility.
 */
struct ylog_main_s
{
  void (*handler)(const char*);
  int setup;
  char *filename;
  FILE *file;
  char *progname;
  char *identname;
  ylog_priority_t prio;
  unsigned int max_log_size;
  char *modules;
  int facility;
};

/*! @typedef ylog_main_t Main yLogs structure. See ylog_main_s structure. */
typedef struct ylog_main_s ylog_main_t;

#ifndef YLOG_IS_YLOG
/*! @var _ylog_gl Global variable that contains all informations about process'
 * logs. DON'T USE IT ! */
extern ylog_main_t _ylog_gl;
#endif /* YLOG_IS_YLOG */

/*!
 * @function	ylog_init
 *		Initialize the global yLog structure. Can be used many times in
 *		the same process.
 * @param	setup		Log type (stderr, file, syslog, function).
 *				Could be a boolean combination like
 *				YLOG_STDERR | YLOG_SYSLOG.
 * @param	filename	Name of the output file (for file output only).
 * @param	progname	Name of the current process.
 * @param	max_log_size	Maximum size of log files. Inifinite size if
 *				set to 0.
 */
void ylog_init(int setup, char *filename, char *progname,
	       unsigned int max_log_size);

/*!
 * @function	ylog_set_prio
 *		Change the minimum priority level. Only messages with a greater
 *		or equal level will be written into logs.
 * @param	prio	New default priority level.
 */
void ylog_set_prio(ylog_priority_t prio);

/*!
 * @function	ylog_set_logsize
 *		Set the maximum size of log files.
 * @param	max_log_size	Maximum size of log files. Inifinite size if
 *				set to 0.
 */
void ylog_set_logsize(unsigned int max_log_size);

/*!
 * @function	ylog_set_handler
 *		Set the function pointer to the log handler. This function will
 *		be called each time a log is added.
 * @param	f	A pointer to the callback function.
 */
void ylog_set_handler(void (*f)(const char*));

/*!
 * @function	ylog_set_identname
 *		Set the identity name used for syslog.
 * @param	identname	The new identity name. Replace the old name.
 *				Could be set to NULL (just erase the old one).
 */
void ylog_set_identname(const char *identname);

/*!
 * @function	ylog_set_facility
 *		Set the syslog facility.
 * @param	facility	The new syslog facility.
 */
void ylog_set_facility(int facility);

/*!
 * @function	ylog_write
 *		Write a new log entry in logs.
 * @param	prio	Priority level of this log entry.
 * @param	file	Name of the file where this log entry is added.
 * @param	line	Number of the line where this log entry is added.
 * @param	funcname	Caller function's name.
 * @param	str	Main character string of the message.
 * @param	...	Variable arguments.
 * @return	TRUE if the log entry was written, FALSE otherwise.
 */
ybool_t ylog_write(ylog_priority_t prio, const char *file, int line,
		   const char *funcname, const char *str, ...);

/*!
 * @function	ylog_check_module
 *		Check if a module can write logs or not. If the YLOG_MODULES
 *		environment variable is set, and the module name is not inside,
 *		the log entry should not be written.
 * @param	module	Name of the module which wants to add the log.
 * @return	TRUE if the log can be added, FALSE otherwise.
 */
ybool_t ylog_check_module(char *module);

/*!
 * @function	ylog_close
 *		Close a log session. If some ylog_write() are called after,
 *		they will be redirected to standard output.
 * @param	prio	New default priority level.
 */
void ylog_close(ylog_priority_t prio);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YLOG_H__ */
