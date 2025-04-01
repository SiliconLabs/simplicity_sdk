/***************************************************************************//**
 * @file sl_wisun_udp_client.c
 * @brief Wi-SUN UDP client
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket/socket.h"
#include "sl_wisun_trace_util.h"
#include "sl_wisun_udp_client.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static uint16_t _socket_buff_length = 0U;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* create udp client */
int32_t sl_wisun_udp_client_create(void)
{
  int32_t sockid = 0;

  // create client socket
  sockid = socket(AF_INET6, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  if (sockid == SOCKET_INVALID_ID) {
    printf("[Failed to create socket: %ld]\n", sockid);
    return SOCKET_INVALID_ID;
  }

  printf("[Socket created: %ld]\n", sockid);

  return sockid;
}

/* close udp client socket */
void sl_wisun_udp_client_close(const int32_t sockid)
{
  if (close(sockid) == SOCKET_RETVAL_ERROR) {
    printf("[Failed to close socket: %ld]\n", sockid);
  } else {
    printf("[Socket closed: %ld]\n", sockid);
  }
}

/* write to udp client socket */
void sl_wisun_udp_client_write(const int32_t sockid,
                               const char *remote_ip_address,
                               const uint16_t remote_port,
                               const char *str)
{
  int32_t res = 0;
  static sockaddr_in6_t server_addr = { 0 };

  if (remote_ip_address == NULL) {
    printf("[Failed: IP address is NULL ptr]\n");
    return;
  }

  if (str == NULL) {
    printf("[Failed: Data to write is NULL ptr]\n");
    return;
  }

  // set the server address
  server_addr.sin6_family = AF_INET6;
  server_addr.sin6_port = htons(remote_port);

  if (inet_pton(AF_INET6, remote_ip_address, &server_addr.sin6_addr) != 1) {
    printf("[Invalid IP address: %s]\n", remote_ip_address);
    return;
  }

  // send data
  res = sendto(sockid, str, strlen(str), 0, (const struct sockaddr *) &server_addr, sizeof(server_addr));
  if (res == SOCKET_RETVAL_ERROR) {
    printf("[Failed to send on socket: %ld]\n", sockid);
  }

  // Save socket buffer length for read
  _socket_buff_length += strlen(str);

}

/* read on udp client socket */
void sl_wisun_udp_client_read(const int32_t sockid)
{
  char *socket_buff = NULL;
  int32_t res = 0;
  static sockaddr_in6_t server_addr = { 0 };
  socklen_t len = sizeof(server_addr);

  // Socket is empty
  if (_socket_buff_length == 0U) {
    return;
  }

  // Allocate memory for socket buffer
  socket_buff = (char *)app_wisun_malloc(_socket_buff_length + 1);
  memset(socket_buff, 0U, _socket_buff_length + 1);

  // Read from socket
  res = recvfrom(sockid, socket_buff, _socket_buff_length, 0, (struct sockaddr *)&server_addr, &len);
  if (res <= 0L) {
    app_wisun_free(socket_buff);
    // Reset socket buffer length
    _socket_buff_length = 0U;
    return;
  }

  // Print received data
  printf("%s\n", socket_buff);
  app_wisun_free(socket_buff);

  // Update socket buffer length
  _socket_buff_length -= res;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
