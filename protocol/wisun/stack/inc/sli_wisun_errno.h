/*
 * Copyright (c) 2023 Silicon Laboratories Inc. (www.silabs.com)
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of the Silicon Labs Master Software License
 * Agreement (MSLA) available at [1].  This software is distributed to you in
 * Object Code format and/or Source Code format and is governed by the sections
 * of the MSLA applicable to Object Code, Source Code and Modified Open Source
 * Code. By using this software, you agree to the terms of the MSLA.
 *
 * [1]: https://www.silabs.com/about-us/legal/master-software-license-agreement
 */
#ifndef SLI_WISUN_ERRNO_H
#define SLI_WISUN_ERRNO_H
#include "errno.h"

#define sli_wisun_set_errno(err)     \
  do {                               \
    errno = (err);                   \
  } while (0)

// IAR's errno.h apparently doesn't define errno code values.

#ifndef EINTR
#define EINTR 4
#endif

#ifndef EBADF
#define EBADF 9
#endif

#ifndef EINVAL
#define EINVAL 22
#endif

#ifndef EAGAIN
#define EAGAIN 11
#endif

#ifndef EWOULDBLOCK
#define EWOULDBLOCK 11
#endif

#ifndef ENOTCONN
#define ENOTCONN 107
#endif

#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT 97
#endif

#ifndef ENOSPC
#define ENOSPC 28
#endif

#ifndef EMFILE
#define EMFILE 24 /* Too many open file descriptors */
#endif

#ifndef EOVERFLOW
#define EOVERFLOW 75
#endif

#ifndef EIO
#define EIO 5
#endif

#ifndef EMSGSIZE
#define EMSGSIZE 90
#endif

#ifndef ENETDOWN
#define ENETDOWN 100
#endif

#ifndef ENOBUFS
#define ENOBUFS 105
#endif

#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL 99
#endif

#ifndef EDESTADDRREQ
#define EDESTADDRREQ 89
#endif

#ifndef EOPNOTSUPP
#define EOPNOTSUPP 95
#endif

#ifndef EADDRINUSE
#define EADDRINUSE 98
#endif

#ifndef EPROTOTYPE
#define EPROTOTYPE 91
#endif

#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT 93
#endif

#ifndef ENFILE
#define ENFILE 23
#endif

#ifndef ENOMEM
#define ENOMEM 12
#endif

#ifndef ENOPROTOOPT
#define ENOPROTOOPT 92
#endif

#ifndef EISCONN
#define EISCONN 106
#endif

#ifndef EALREADY
#define EALREADY 114
#endif

#endif