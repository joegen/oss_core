// Library: OSS_CORE - Foundation API for SIP B2BUA
// Copyright (c) OSS Software Solutions
// Contributor: Joegen Baclor - mailto:joegen@ossapp.com
//
// Permission is hereby granted, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, execute, and to prepare
// derivative works of the Software, all subject to the
// "GNU Lesser General Public License (LGPL)".
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//

#include "OSS/JS/JSPlugin.h"
#include "OSS/UTL/CoreUtils.h"
#include <unistd.h>
#include <errno.h>
#include <poll.h>

static v8::Handle<v8::Value> __errno_get(v8::Local<v8::String> property, const v8::AccessorInfo& info) 
{
  return v8::Integer::New(errno);
}

static v8::Handle<v8::Value> init_exports(const v8::Arguments& args)
{
  v8::HandleScope scope; 
  v8::Persistent<v8::Object> exports = v8::Persistent<v8::Object>::New(v8::Object::New());
  
  //
  // Mutable Properties
  //
  exports->SetAccessor(v8::String::New("errno"), __errno_get, 0);
  
  //
  // Standard system errors
  //
  CONST_EXPORT(EPERM);          /* Operation not permitted */
  CONST_EXPORT(ENOENT);         /* No such file or directory */
  CONST_EXPORT(ESRCH);          /* No such process */
  CONST_EXPORT(EINTR);          /* Interrupted system call */
  CONST_EXPORT(EIO);            /* I/O error */
  CONST_EXPORT(ENXIO);          /* No such device or address */
  CONST_EXPORT(E2BIG);          /* Argument list too long */
  CONST_EXPORT(ENOEXEC);        /* Exec format error */
  CONST_EXPORT(EBADF);          /* Bad file number */
  CONST_EXPORT(ECHILD);         /* No child processes */
  CONST_EXPORT(EAGAIN);         /* Try again */
  CONST_EXPORT(ENOMEM);         /* Out of memory */
  CONST_EXPORT(EACCES);         /* Permission denied */
  CONST_EXPORT(EFAULT);         /* Bad address */
  CONST_EXPORT(ENOTBLK);        /* Block device required */
  CONST_EXPORT(EBUSY);          /* Device or resource busy */
  CONST_EXPORT(EEXIST);         /* File exists */
  CONST_EXPORT(EXDEV);          /* Cross-device link */
  CONST_EXPORT(ENODEV);         /* No such device */
  CONST_EXPORT(ENOTDIR);        /* Not a directory */
  CONST_EXPORT(EISDIR);         /* Is a directory */
  CONST_EXPORT(EINVAL);         /* Invalid argument */
  CONST_EXPORT(ENFILE);         /* File table overflow */
  CONST_EXPORT(EMFILE);         /* Too many open files */
  CONST_EXPORT(ENOTTY);         /* Not a typewriter */
  CONST_EXPORT(ETXTBSY);        /* Text file busy */
  CONST_EXPORT(EFBIG);          /* File too large */
  CONST_EXPORT(ENOSPC);         /* No space left on device */
  CONST_EXPORT(ESPIPE);         /* Illegal seek */
  CONST_EXPORT(EROFS);          /* Read-only file system */
  CONST_EXPORT(EMLINK);         /* Too many links */
  CONST_EXPORT(EPIPE);          /* Broken pipe */
  CONST_EXPORT(EDOM);           /* Math argument out of domain of func */
  CONST_EXPORT(ERANGE);         /* Math result not representable */
  CONST_EXPORT(EDEADLK);        /* Resource deadlock would occur */
  CONST_EXPORT(ENAMETOOLONG);   /* File name too long */
  CONST_EXPORT(ENOLCK);         /* No record locks available */
  CONST_EXPORT(ENOSYS);         /* Invalid system call number */
  CONST_EXPORT(ENOTEMPTY);      /* Directory not empty */
  CONST_EXPORT(ELOOP);          /* Too many symbolic links encountered */
  CONST_EXPORT(EWOULDBLOCK);    /* Operation would block */
  CONST_EXPORT(ENOMSG);         /* No message of desired type */
  CONST_EXPORT(EIDRM);          /* Identifier removed */
  CONST_EXPORT(ENOSTR);         /* Device not a stream */
  CONST_EXPORT(ENODATA);        /* No data available */
  CONST_EXPORT(ETIME);          /* Timer expired */
  CONST_EXPORT(ENOSR);          /* Out of streams resources */
  CONST_EXPORT(EREMOTE);        /* Object is remote */
  CONST_EXPORT(ENOLINK);        /* Link has been severed */
  CONST_EXPORT(EPROTO);         /* Protocol error */
  CONST_EXPORT(EMULTIHOP);      /* Multihop attempted */
  CONST_EXPORT(EBADMSG);        /* Not a data message */
  CONST_EXPORT(EOVERFLOW);      /* Value too large for defined data type */
  CONST_EXPORT(EILSEQ);         /* Illegal byte sequence */
  CONST_EXPORT(EUSERS);         /* Too many users */
  CONST_EXPORT(ENOTSOCK);       /* Socket operation on non-socket */
  CONST_EXPORT(EDESTADDRREQ);   /* Destination address required */
  CONST_EXPORT(EMSGSIZE);       /* Message too long */
  CONST_EXPORT(EPROTOTYPE);     /* Protocol wrong type for socket */
  CONST_EXPORT(ENOPROTOOPT);    /* Protocol not available */
  CONST_EXPORT(EPROTONOSUPPORT);/* Protocol not supported */
  CONST_EXPORT(ESOCKTNOSUPPORT);/* Socket type not supported */
  CONST_EXPORT(EOPNOTSUPP);     /* Operation not supported on transport endpoint */
  CONST_EXPORT(EPFNOSUPPORT);   /* Protocol family not supported */
  CONST_EXPORT(EAFNOSUPPORT);   /* Address family not supported by protocol */
  CONST_EXPORT(EADDRINUSE);     /* Address already in use */
  CONST_EXPORT(EADDRNOTAVAIL);  /* Cannot assign requested address */
  CONST_EXPORT(ENETDOWN);       /* Network is down */
  CONST_EXPORT(ENETUNREACH);    /* Network is unreachable */
  CONST_EXPORT(ENETRESET);      /* Network dropped connection because of reset */
  CONST_EXPORT(ECONNABORTED);   /* Software caused connection abort */
  CONST_EXPORT(ECONNRESET);     /* Connection reset by peer */
  CONST_EXPORT(ENOBUFS);        /* No buffer space available */
  CONST_EXPORT(EISCONN);        /* Transport endpoint is already connected */
  CONST_EXPORT(ENOTCONN);       /* Transport endpoint is not connected */
  CONST_EXPORT(ESHUTDOWN);      /* Cannot send after transport endpoint shutdown */
  CONST_EXPORT(ETOOMANYREFS);   /* Too many references: cannot splice */
  CONST_EXPORT(ETIMEDOUT);      /* Connection timed out */
  CONST_EXPORT(ECONNREFUSED);   /* Connection refused */
  CONST_EXPORT(EHOSTDOWN);      /* Host is down */
  CONST_EXPORT(EHOSTUNREACH);   /* No route to host */
  CONST_EXPORT(EALREADY);       /* Operation already in progress */
  CONST_EXPORT(EINPROGRESS);    /* Operation now in progress */
  CONST_EXPORT(ESTALE);         /* Stale file handle */
  CONST_EXPORT(EDQUOT);         /* Quota exceeded */
  CONST_EXPORT(ECANCELED);      /* Operation Canceled */
  CONST_EXPORT(EOWNERDEAD);     /* Owner died */
  CONST_EXPORT(ENOTRECOVERABLE);/* State not recoverable */
  
  
#if !OSS_PLATFORM_MAC_OS_X
  CONST_EXPORT(ERFKILL);        /* Operation not possible due to RF-kill */
  CONST_EXPORT(EHWPOISON);      /* Memory page has hardware error */
  CONST_EXPORT(ENOKEY);         /* Required key not available */
  CONST_EXPORT(EKEYEXPIRED);    /* Key has expired */
  CONST_EXPORT(EKEYREVOKED);    /* Key has been revoked */
  CONST_EXPORT(EKEYREJECTED);   /* Key was rejected by service */
  CONST_EXPORT(ENOMEDIUM);      /* No medium found */
  CONST_EXPORT(EMEDIUMTYPE);    /* Wrong medium type */
  CONST_EXPORT(ECHRNG);         /* Channel number out of range */
  CONST_EXPORT(EL2NSYNC);       /* Level 2 not synchronized */
  CONST_EXPORT(EL3HLT);         /* Level 3 halted */
  CONST_EXPORT(EL3RST);         /* Level 3 reset */
  CONST_EXPORT(ELNRNG);         /* Link number out of range */
  CONST_EXPORT(EUNATCH);        /* Protocol driver not attached */
  CONST_EXPORT(ENOCSI);         /* No CSI structure available */
  CONST_EXPORT(EL2HLT);         /* Level 2 halted */
  CONST_EXPORT(EBADE);          /* Invalid exchange */
  CONST_EXPORT(EBADR);          /* Invalid request descriptor */
  CONST_EXPORT(EXFULL);         /* Exchange full */
  CONST_EXPORT(ENOANO);         /* No anode */
  CONST_EXPORT(EBADRQC);        /* Invalid request code */
  CONST_EXPORT(EBADSLT);        /* Invalid slot */
  CONST_EXPORT(EDEADLOCK);      /* Resource deadlock would occur */
  CONST_EXPORT(EBFONT);         /* Bad font file format */
  CONST_EXPORT(ENONET);         /* Machine is not on the network */
  CONST_EXPORT(ENOPKG);         /* Package not installed */
  CONST_EXPORT(EADV);           /* Advertise error */
  CONST_EXPORT(ESRMNT);         /* Srmount error */
  CONST_EXPORT(ECOMM);          /* Communication error on send */
  CONST_EXPORT(EDOTDOT);        /* RFS specific error */
  CONST_EXPORT(ENOTUNIQ);       /* Name not unique on network */
  CONST_EXPORT(EBADFD);         /* File descriptor in bad state */
  CONST_EXPORT(EREMCHG);        /* Remote address changed */
  CONST_EXPORT(ELIBACC);        /* Can not access a needed shared library */
  CONST_EXPORT(ELIBBAD);        /* Accessing a corrupted shared library */
  CONST_EXPORT(ELIBSCN);        /* lib section in a.out corrupted */
  CONST_EXPORT(ELIBMAX);        /* Attempting to link in too many shared libraries */
  CONST_EXPORT(ELIBEXEC);       /* Cannot exec a shared library directly */
  CONST_EXPORT(ERESTART);       /* Interrupted system call should be restarted */
  CONST_EXPORT(ESTRPIPE);       /* Streams pipe error */
  CONST_EXPORT(EUCLEAN);        /* Structure needs cleaning */
  CONST_EXPORT(ENOTNAM);        /* Not a XENIX named type file */
  CONST_EXPORT(ENAVAIL);        /* No XENIX semaphores available */
  CONST_EXPORT(EISNAM);         /* Is a named type file */
  CONST_EXPORT(EREMOTEIO);      /* Remote I/O error */
#endif
  return exports;
}

JS_REGISTER_MODULE(Error);
