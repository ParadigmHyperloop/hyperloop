/*****************************************************************************
 * Copyright (c) OpenLoop, 2016
 *
 * This material is proprietary of The OpenLoop Alliance and its members.
 * All rights reserved.
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden except by express written permission of OpenLoop.
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * OpenLoop.
 *
 * Please see http://www.opnlp.co for contact information
 ****************************************************************************/

#include "pod.h"

extern ring_buf_t logbuf;

// Much of this code is based on this CMU TCP client example
// http://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpclient.c

int log_connect() {
  info("Connecting to logging server: " LOG_SVR_NAME);

  int fd, portno = LOG_SVR_PORT;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname = LOG_SVR_NAME;

  // Create the socket
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    error("ERROR opening socket\n");
    return -1;
  }

  struct timeval t;
  t.tv_sec = 1;
  t.tv_usec = 0;

  // Set send and recieve timeouts to reasonable numbers
  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t)) < 0) {
    error("setsockopt failed\n");
    return -1;
  }

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&t, sizeof(t)) < 0) {
    error("setsockopt failed\n");
    return -1;
  }

  // Get the server's IP Address from DNS
  server = gethostbyname(hostname);
  if (server == NULL) {
    error("ERROR, no such host as %s\n", hostname);
    return -1;
  }

  // Build the server's IP Address
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr,
        server->h_length);
  serveraddr.sin_port = htons(portno);

  // Start TCP connection
  if (connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    error("Connection Refused\n");
    return -1;
  }

  note("Connected to " LOG_SVR_NAME ":" __XSTR__(LOG_SVR_PORT) " fd %d", fd);

  return fd;
}

// Use of any output macro in this function could lead to stack overflow...
// Do not use output macros within this function
int log_send(log_t *l) {
  pod_t *pod = get_pod();

  if (pod->logging_socket < 0) {
    fprintf(stderr, "Attempt to send before socket open: %d",
            pod->logging_socket);
    return -1;
  }

  char buf[MAX_PACKET_SIZE];

  switch (l->type) {
  case Message:
    snprintf(&buf[0], MAX_PACKET_SIZE, "POD1%s", l->v.message);
    break;
  case Telemetry_float:
    snprintf(&buf[0], MAX_PACKET_SIZE, "POD2%s %f\n", l->v.float_data.name,
             l->v.float_data.value);
    break;
  case Telemetry_int32:
    snprintf(&buf[0], MAX_PACKET_SIZE, "POD2%s %d\n", l->v.int32_data.name,
             l->v.int32_data.value);
    break;
  case Packet:
    assert(sizeof(l->v.packet) < MAX_PACKET_SIZE);
    memcpy(&buf[0], &(l->v.packet), sizeof(l->v.packet));
    break;
  default:
    fprintf(stderr, "Unknown Log Type: %d", l->type);
    return -1;
  }

  /* send the message line to the server */
  int n = write(pod->logging_socket, buf, strlen(buf));
  if (n <= 0) {
    fprintf(stderr, "ERROR writing to socket\n");
    return -1;
  }
  /* print the server's reply */
  bzero(buf, MAX_PACKET_SIZE);

  return 0;
}

void *logging_main(void *arg) {
  debug("[logging_main] Thread Start");

  pod_t *pod = get_pod();

  while (pod->logging_socket < 0) {
    pod->logging_socket = log_connect();
    if (pod->logging_socket < 0) {
      error("Logging Socket failed to connect: %s", strerror(errno));
      sleep(1);
    } else {
      break;
    }
  }

  // Post to the boot sem to tell the main thread that we have initialized
  // Main thread will assert that pod mode is still Boot

  info("punching boot_sem to proceed");
  sem_post(pod->boot_sem);

  // Start the log forwarding loop until shutdown
  log_t l;
  bool running = true;
  while (get_pod_mode() != Shutdown && running == true) {
    // Try to pop off a log_t that has been queued
    int r = ring_buf_pop(&l, &logbuf);

    if (r == 0) {
      // Send the log, attempt 3 additional tries if it failed
      int result = log_send(&l), attempts = 0;
      while (result < 0 && attempts < 3) {
        usleep(LOGGING_THREAD_SLEEP);
        result = log_send(&l);
        attempts++;
        fprintf(stderr, "Log Retry #%d result %d\n", attempts, result);
      }

      // Failed to send logs.
      // Regardless of the reason, we need to stop
      if (result < 0) {
        running = false;
        fprintf(stderr, "Alert: Log Send Failed %d\n", result);
        set_pod_mode(Emergency, "Log Send Failed");
      }
    } else {
      // No logs in the queue, lets sit for a while
      usleep(LOGGING_THREAD_SLEEP);
    }
  }

  error("=== Logging system is going down ===");
  exit(1);
  return NULL;
}
