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

// IAR's errno.h apparently doesn't define EAGAIN, EINVAL, EWOULDBLOCK, EINTR, EAFNOSUPPORT, ENOSPC.

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

#endif