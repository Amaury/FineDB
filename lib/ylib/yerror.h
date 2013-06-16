/* Process this file with the HeaderBrowser tool (http://www.headerbrowser.org)
   to create documentation. */
/*!
 * @header	yerror.h
 * @abstract	Definitions for error handling.
 * @discussion	The type yerr_t is a convenient way to transmit error values.
 *		When a yerr_t value is positive, there is no error. If it is
 *		negative or equal to zero, there is an error. When a function
 *		have to return an error code, it must return YENOERR in case
 *		of success, or a code that depend of the occuring error.
 * @see		The file $INCLUDE/asm/error.h
 * @version	1.0.0 Aug 12 2002
 * @author	Amaury Bouchard <amaury@amaury.net>
 */
#ifndef __YERROR_H__
#define __YERROR_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif /* __cplusplus || c_plusplus */

/*! @typedef yerr_t Type for error handling. Negative values are errors. */
typedef signed int yerr_t;

/*! @define YENOERR No error. */
#define YENOERR			1
/*! @define YEUNDEF Error not defined. */
#define YEUNDEF			0
/*! @define YEPERM Operation not permitted. */
#define	YEPERM			(-1)
/*! @define YENOENT No such file or directory. */
#define	YENOENT			(-2)
/*! @define YESRCH No such process. */
#define	YESRCH			(-3)
/*! @define YEINTR Interrupted system call. */
#define	YEINTR			(-4)
/*! @define YEIO I/O error. */
#define	YEIO			(-5)
/*! @define YENXIO No such device or address. */
#define	YENXIO			(-6)
/*! @define YE2BIG Arg list too long. */
#define	YE2BIG			(-7)
/*! @define YENOEXEC Exec format error. */
#define	YENOEXEC		(-8)
/*! @define YEBADF Bad file number. */
#define	YEBADF			(-9)
/*! @define YECHILD No child processes. */
#define	YECHILD			(-10)
/*! @define YEAGAIN Try again. */
#define	YEAGAIN			(-11)
/*! @define YENOMEM Out of memory. */
#define	YENOMEM			(-12)
/*! @define YEACCESS Permission denied. */
#define	YEACCESS		(-13)
/*! @define YEFAULT Bad address. */
#define	YEFAULT			(-14)
/*! @define YENOTBLK Block device required. */
#define	YENOTBLK		(-15)
/*! @define YEBUSY Device or resource busy. */
#define	YEBUSY			(-16)
/*! @define YEEXIST File exists. */
#define	YEEXIST			(-17)
/*! @define YEXDEV Cross-device link. */
#define	YEXDEV			(-18)
/*! @define YENODEV No such device. */
#define	YENODEV			(-19)
/*! @define YENOTDIR Not a directory. */
#define	YENOTDIR		(-20)
/*! @define YEISDIR Is a directory. */
#define	YEISDIR			(-21)
/*! @define YEINVAL Invalid argument. */
#define	YEINVAL			(-22)
/*! @define YENFILE File table overflow. */
#define	YENFILE			(-23)
/*! @define YEMFILE Too many open files. */
#define	YEMFILE			(-24)
/*! @define YENOTTY Not a typewriter. */
#define	YENOTTY			(-25)
/*! @define YETXTBSY Text file busy. */
#define	YETXTBSY		(-26)
/*! @define YEFBIG File too large. */
#define	YEFBIG			(-27)
/*! @define YENOSPC No space left on device. */
#define	YENOSPC			(-28)
/*! @define YESPIPE Illegal seek. */
#define	YESPIPE			(-29)
/*! @define YEROFS Read-only file system. */
#define	YEROFS			(-30)
/*! @define YEMLINK Too many links. */
#define	YEMLINK			(-31)
/*! @define YEPIPE Broken pipe. */
#define	YEPIPE			(-32)
/*! @define YEDOM Math argument out of domain of func. */
#define	YEDOM			(-33)
/*! @define YERANGE Math result not representable. */
#define	YERANGE			(-34)
/*! @define YEDEADLK Resource deadlock would occur. */
#define	YEDEADLK		(-35)
/*! @define YENAMETOOLONG File name too long. */
#define	YENAMETOOLONG		(-36)
/*! @define YENOLCK No record locks available. */
#define	YENOLCK			(-37)
/*! @define YENOSYS Function not implemented. */
#define	YENOSYS			(-38)
/*! @define YENOTEMPTY Directory not empty. */
#define	YENOTEMPTY		(-39)
/*! @define YELOOP Too many symbolic links encountered. */
#define	YELOOP			(-40)
/*! @define YEWOULDBLOCK Operation would block. */
#define	YEWOULDBLOCK		(-EAGAIN)
/*! @define YENOMSG No message of desired type. */
#define	YENOMSG			(-42)
/*! @define YEIDRM Identifier removed. */
#define	YEIDRM			(-43)
/*! @define YECHRNG Channel number out of range. */
#define	YECHRNG			(-44)
/*! @define YEL2NSYNC Level 2 not synchronized. */
#define	YEL2NSYNC		(-45)
/*! @define YEL3HLT Level 3 halted. */
#define	YEL3HLT			(-46)
/*! @define YEL3RST Level 3 reset. */
#define	YEL3RST			(-47)
/*! @define YELNRNG Link number out of range. */
#define	YELNRNG			(-48)
/*! @define YEUNATCH Protocol driver not attached. */
#define	YEUNATCH		(-49)
/*! @define YENOCSI No CSI structure available. */
#define	YENOCSI			(-50)
/*! @define YEL2HLT Level 2 halted. */
#define	YEL2HLT			(-51)
/*! @define YEBADE Invalid exchange. */
#define	YEBADE			(-52)
/*! @define YEBADR Invalid request descriptor. */
#define	YEBADR			(-53)
/*! @define YEXFULL Exchange full. */
#define	YEXFULL			(-54)
/*! @define YENOANO No anode. */
#define	YENOANO			(-55)
/*! @define YEBADRQC Invalid request code. */
#define	YEBADRQC		(-56)
/*! @define YEBADSLT Invalid slot. */
#define	YEBADSLT		(-57)
/*! @define YEDEADLOCK Dead lock. */
#define	YEDEADLOCK		(-EDEADLK)
/*! @define YEBFONT Bad font file format. */
#define	YEBFONT			(-59)
/*! @define YENOSTR Device not a stream. */
#define	YENOSTR			(-60)
/*! @define YENODATA No data available. */
#define	YENODATA		(-61)
/*! @define YETIME Timer expired. */
#define	YETIME			(-62)
/*! @define YENOSR Out of streams resources. */
#define	YENOSR			(-63)
/*! @define YENONET Machine is not on the network. */
#define	YENONET			(-64)
/*! @define YENOPKG Package not installed. */
#define	YENOPKG			(-65)
/*! @define YEREMOTE Object is remote. */
#define	YEREMOTE		(-66)
/*! @define YENOLINK Link has been severed. */
#define	YENOLINK		(-67)
/*! @define YEADV Advertise error. */
#define	YEADV			(-68)
/*! @define YESRMNT Srmount error. */
#define	YESRMNT			(-69)
/*! @define YECOMM Communication error on send. */
#define	YECOMM			(-70)
/*! @define YEPROTO Protocol error. */
#define	YEPROTO			(-71)
/*! @define YEMULTIHOP Multihop attempted. */
#define	YEMULTIHOP		(-72)
/*! @define YEDOTDOT RFS specific error. */
#define	YEDOTDOT		(-73)
/*! @define YEBADMSG Not a data message. */
#define	YEBADMSG		(-74)
/*! @define YEOVERFLOW Value too large for defined data type. */
#define	YEOVERFLOW		(-75)
/*! @define YENOTUNIQ Name not unique on network. */
#define	YENOTUNIQ		(-76)
/*! @define YEBADFD File descriptor in bad state. */
#define	YEBADFD			(-77)
/*! @define YEREMCHG Remote address changed. */
#define	YEREMCHG		(-78)
/*! @define YELIBACC Can not access a needed shared library. */
#define	YELIBACC		(-79)
/*! @define YELIBBAD Accessing a corrupted shared library. */
#define	YELIBBAD		(-80)
/*! @define YELIBSCN .lib section in a.out corrupted. */
#define	YELIBSCN		(-81)
/*! @define YELIBMAX Attempting to link in too many shared libraries. */
#define	YELIBMAX		(-82)
/*! @define YELIBEXEC Cannot exec a shared library directly. */
#define	YELIBEXEC		(-83)
/*! @define YEILSEQ Illegal byte sequence. */
#define	YEILSEQ			(-84)
/*! @define YERESTART Interrupted system call should be restarted. */
#define	YERESTART		(-85)
/*! @define YESTRPIPE Streams pipe error. */
#define	YESTRPIPE		(-86)
/*! @define YEUSERS Too many users. */
#define	YEUSERS			(-87)
/*! @define YENOTSOCK Socket operation on non-socket. */
#define	YENOTSOCK		(-88)
/*! @define YEDESTADDRREQ Destination address required. */
#define	YEDESTADDRREQ		(-89)
/*! @define YEMSGSIZE Message too long. */
#define	YEMSGSIZE		(-90)
/*! @define YEPROTOTYPE Protocol wrong type for socket. */
#define	YEPROTOTYPE		(-91)
/*! @define YENOPROTOOPT Protocol not available. */
#define	YENOPROTOOPT		(-92)
/*! @define YEPROTONOSUPPORT Protocol not supported. */
#define	YEPROTONOSUPPORT	(-93)
/*! @define YESOCKTNOSUPPORT Socket type not supported. */
#define	YESOCKTNOSUPPORT	(-94)
/*! @define YEOPNOTSUPP Operation not supported on transport endpoint. */
#define	YEOPNOTSUPP		(-95)
/*! @define YEPFNOSUPPORT Protocol family not supported. */
#define	YEPFNOSUPPORT		(-96)
/*! @define YEAFNOSUPPORT Address family not supported by protocol. */
#define	YEAFNOSUPPORT		(-97)
/*! @define YEADDRINUSE Address already in use. */
#define	YEADDRINUSE		(-98)
/*! @define YEADDRNOTAVAIL Cannot assign requested address. */
#define	YEADDRNOTAVAIL		(-99)
/*! @define YENETDOWN Network is down. */
#define	YENETDOWN		(-100)
/*! @define YENETUNREACH Network is unreachable. */
#define	YENETUNREACH		(-101)
/*! @define YENETRESET Network dropped connection because of reset. */
#define	YENETRESET		(-102)
/*! @define YECONNABORTED Software caused connection abort. */
#define	YECONNABORTED		(-103)
/*! @define YECONNRESET Connection reset by peer. */
#define	YECONNRESET		(-104)
/*! @define YENOBUFS No buffer space available. */
#define	YENOBUFS		(-105)
/*! @define YEISCONN Transport endpoint is already connected. */
#define	YEISCONN		(-106)
/*! @define YENOTCONN Transport endpoint is not connected. */
#define	YENOTCONN		(-107)
/*! @define YESHUTDOWN Cannot send after transport endpoint shutdown. */
#define	YESHUTDOWN		(-108)
/*! @define YETOOMANYREFS Too many references: cannot splice. */
#define	YETOOMANYREFS		(-109)
/*! @define YETIMEDOUT Connection timed out. */
#define	YETIMEDOUT		(-110)
/*! @define YECONNREFUSED Connection refused. */
#define	YECONNREFUSED		(-111)
/*! @define YEHOSTDOWN Host is down. */
#define	YEHOSTDOWN		(-112)
/*! @define YEHOSTUNREACH No route to host. */
#define	YEHOSTUNREACH		(-113)
/*! @define YEALREADY Operation already in progress. */
#define	YEALREADY		(-114)
/*! @define YEINPROGRESS Operation now in progress. */
#define	YEINPROGRESS		(-115)
/*! @define YESTALE Stale NFS file handle. */
#define	YESTALE			(-116)
/*! @define YEUCLEAN Structure needs cleaning. */
#define	YEUCLEAN		(-117)
/*! @define YENOTNAM Not a XENIX named type file. */
#define	YENOTNAM		(-118)
/*! @define YENAVAIL No XENIX semaphores available. */
#define	YENAVAIL		(-119)
/*! @define YEISNAM Is a named type file. */
#define	YEISNAM			(-120)
/*! @define YEREMOTEIO Remote I/O error. */
#define	YEREMOTEIO		(-121)
/*! @define YEDQUOT Quota exceeded. */
#define	YEDQUOT			(-122)
/*! @define YENOMEDIUM No medium found. */
#define	YENOMEDIUM		(-123)
/*! @define YEMEDIUMTYPE Wrong medium type. */
#define	YEMEDIUMTYPE		(-124)
/*! @define YESYNTAX Syntax error. */
#define YESYNTAX		(-125)

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif /* __cplusplus || c_plusplus */

#endif /* __YERROR_H__ */
