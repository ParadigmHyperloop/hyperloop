/*****************************************************************************
 * Copyright (c) Paradigm Hyperloop, 2017
 *
 * This material is proprietary intellectual property of Paradigm Hyperloop.
 * All rights reserved.
 *
 * The methods and techniques described herein are considered proprietary
 * information. Reproduction or distribution, in whole or in part, is
 * forbidden without the express written permission of Paradigm Hyperloop.
 *
 * Please send requests and inquiries to:
 *
 *  Software Engineering Lead - Eddie Hurtig <hurtige@ccs.neu.edu>
 *
 * Source that is published publicly is for demonstration purposes only and
 * shall not be utilized to any extent without express written permission of
 * Paradigm Hyperloop.
 *
 * Please see http://www.paradigm.team for additional information.
 *
 * THIS SOFTWARE IS PROVIDED BY PARADIGM HYPERLOOP ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PARADIGM HYPERLOOP BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#include "logging.h"

extern ring_buf_t logbuf;

static int log_open(char *filename) {
  return open(filename, O_CREAT | O_WRONLY | O_APPEND,
              S_IRWXU | S_IRWXG | S_IROTH);
}

static int log_connect() {
  pod_t *pod = get_pod();
  info("Connecting to logging server: %d", get_value(&pod->logging_port));
  int fd, portno = get_value(&pod->logging_port);
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname = LOG_SVR_NAME;

  // Create the socket
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (fd < 0) {
    error("ERROR opening socket\n");
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
  info("Resolved %s => %s", hostname, inet_ntoa(serveraddr.sin_addr));
  serveraddr.sin_port = htons(portno);

  // "connect" to the UDP server (Really just sets the default sendto address)
  if (connect(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    error("UDP Connection Error\n");
    return -1;
  }

  note("Connected to " LOG_SVR_NAME ": %d fd %d", get_value(&pod->logging_port),
       fd);

  return fd;
}

// Use of any output macro in this function could lead to stack overflow...
// Do not use output macros in this function
static int log_send(log_t *l) {
  pod_t *pod = get_pod();

  if (pod->logging_socket < 0) {
    fprintf(stderr, "Attempt to send before socket open: %d",
            pod->logging_socket);
    return -1;
  }

  char buf[MAX_PACKET_SIZE];
  size_t len = 0;
  switch (l->type) {
  case Packet:
    assert(sizeof(l->data) <= MAX_PACKET_SIZE);
    len = l->sz;
    memcpy(&buf[0], &(l->data), len);
    break;
  default:
    fprintf(stderr, "Unknown Log Type: %d", l->type);
    return -1;
  }

  /* send the message line to the server */
  ssize_t n = write(pod->logging_socket, buf, len);
  if (n <= 0) {
    fprintf(stderr, "ERROR writing to socket: %s\n", strerror(errno));
    return -1;
  }

  if (pod->logging_fd > -1) {
    n = write(pod->logging_fd, buf, len);
    if (n <= 0) {
      fprintf(stderr, "ERROR writing to binary log file: %s\n",
              strerror(errno));
      return -1;
    }
  }
  /* print the server's reply */
  bzero(buf, MAX_PACKET_SIZE);

  return 0;
}

void *logging_main(__unused void *arg) {
  debug("[logging_main] Thread Start");

  pod_t *pod = get_pod();
  debug("[logging_main] Got a pod");

  while (pod->logging_socket < 0 && get_pod_mode() != Shutdown) {
    debug("[logging_main] Opening loggin socket");
    pod->logging_socket = log_connect();
    if (pod->logging_socket < 0) {
      error("Logging Socket failed to connect: %s", strerror(errno));
      sleep(1);
    } else {
      break;
    }
  }

  while (pod->logging_fd < 0) {
    debug("[logging_main] Opening Log File");

    pod->logging_fd = log_open(pod->logging_filename);
    if (pod->logging_fd < 0) {
      error("Logging File failed to open: %s", strerror(errno));
      sleep(1);
    } else {
      break;
    }
  }

  // Post to the boot sem to tell the main thread that we have initialized
  // Main thread will assert that pod mode is still Boot

  info("Logging has connected, continuing with boot process");
  sem_post(pod->boot_sem);

  // Start the log forwarding loop until shutdown
  log_t l;
  bool running = true;
  while (get_pod_mode() != Shutdown && running == true) {
    // Try to pop off a log_t that has been queued
    int r = ring_buf_pop(&l, sizeof(log_t), &logbuf);

    if (r == 0) {
      // Send the log, attempt 3 additional tries if it failed
      int result = log_send(&l), attempts = 0;
      while (result < 0 && attempts < MAX_ATTEMPTS_PER_LOG) {
        attempts++;
        usleep(LOGGING_THREAD_SLEEP * attempts * attempts);
        fprintf(stderr, "Log Retry %d of %d. rc: %d\n", attempts,
                MAX_ATTEMPTS_PER_LOG, result);
        result = log_send(&l);
        if (result == 0) {
          fprintf(stderr, "Log Retry %d of %d. Success!\n", attempts,
                  MAX_ATTEMPTS_PER_LOG);
        }
      }

      // Failed to send logs.
      // Regardless of the reason, we need to stop
      if (result < 0) {
        running = false;
        fprintf(stderr, "Alert: Log Send Failed %d\n", result);
        set_pod_mode(Emergency, "Log Send Failed, Shutting Down Logger");
        break;
      }
    } else {
      // No logs in the queue, lets sit for a while
      usleep(LOGGING_THREAD_SLEEP);
    }
  }

  warn("=== Logging system is going down ===");
  return NULL;
}
