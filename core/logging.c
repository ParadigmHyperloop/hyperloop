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

extern char *pod_mode_names[N_POD_STATES];

static int logging_socket = -1;

typedef struct ring_buf {
  void *start;
  void *end;
  void *head;
  void *tail;
  int sz;
  bool initialized;
  pthread_mutex_t mutex;
} ring_buf_t;

static log_t logbuf_data[LOG_BUF_SIZE];
static ring_buf_t logbuf;

/**
 * Push a value onto the queue
 */
#define p_push(p, d) d + ((p - d) % sizeof(d));
int bufAdd(log_t l, struct ring_buf *buf) {
  pthread_mutex_lock(&(buf->mutex));
  // Copy the new element into the buffer

  memcpy(buf->head, &l, buf->sz);
  // fprintf(stderr, "\n >>> Pushing %p (%s)\n", buf->head, ((log_t
  // *)buf->head)->content.message);

  buf->head += buf->sz;
  if (buf->head >= buf->end) {
    buf->head = buf->start;
  }

  // &(buf->data[0]) + (((buf->head + sizeof(*buf->head) - buf->data) %
  // sizeof(buf->data)) / sizeof(*buf->head))
  //  buf->data + ((buf->head + sizeof(*buf->head) - buf->data) %
  //  sizeof(buf->data))
  if (buf->head == buf->tail) {
    buf->tail += buf->sz;
    if (buf->tail == buf->end) {
      buf->tail = buf->start;
    }
    pthread_mutex_unlock(&(buf->mutex));

    return -1;
  }
  pthread_mutex_unlock(&(buf->mutex));

  return 0;
}

/**
 * Pop the oldest value off the buffer
 */
int bufPop(log_t *l, struct ring_buf *buf) {
  pthread_mutex_lock(&(buf->mutex));

  if (buf->tail == buf->head) {
    pthread_mutex_unlock(&(buf->mutex));
    return -1;
  }
  // fprintf(stderr, "\n <<< Popping %p (%s)\n", buf->tail, ((log_t
  // *)buf->tail)->content.message);
  // Copy the new element into the buffer
  memcpy(l, buf->tail, buf->sz);

  buf->tail += buf->sz;
  if (buf->tail >= buf->end) {
    buf->tail = buf->start;
  }

  pthread_mutex_unlock(&(buf->mutex));
  return 0;
}
#undef p_push

/**
 * Initialize the ring buffer
 */
void bufInit(struct ring_buf *buf, void *block, int size, int item_sz) {
  buf->head = buf->start = buf->tail = block;
  buf->end = block + size * item_sz;
  buf->sz = item_sz;
  buf->initialized = true;
  pthread_mutex_init(&(buf->mutex), NULL);
}

// Much of this code is based on this CMU TCP client example
// http://www.cs.cmu.edu/afs/cs/academic/class/15213-f99/www/class26/tcpclient.c

int connectLogger() {
  debug("Connecting to logging server: " LOG_SVR_NAME);

  int sockfd, portno = LOG_SVR_PORT;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname = LOG_SVR_NAME;

  // Create the socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
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
  serveraddr.sin_port = htons(portno);

  // Start TCP connection
  if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
    error("Connection Refused\n");
    return -1;
  }

  note("Connected to " LOG_SVR_NAME ":" __XSTR__(LOG_SVR_PORT) " on fd %d",
       sockfd);

  return sockfd;
}

// Use of any output macro in this function could lead to stack overflow...
// Do not use output macros within this function
int logSend(log_t *l) {
  if (logging_socket < 0) {
    fprintf(stderr, "Attempt to send before socket open: %d", logging_socket);
    return -1;
  }

  char buf[MAX_PACKET_SIZE];

  switch (l->type) {
  case Message:
    snprintf(&buf[0], MAX_PACKET_SIZE, "POD1%s\n", l->content.message);
    break;
  case Telemetry:
    snprintf(&buf[0], MAX_PACKET_SIZE, "POD2%s %d\n", l->content.data.name,
             l->content.data.value);
    break;
  default:
    fprintf(stderr, "Unknown Log Type: %d", l->type);
    return -1;
  }

  /* send the message line to the server */
  int n = write(logging_socket, buf, strlen(buf));
  if (n < 0) {
    fprintf(stderr, "ERROR writing to socket\n");
    return -1;
  }
  /* print the server's reply */
  bzero(buf, MAX_PACKET_SIZE);

  return 0;
}

int logEnqueue(log_t *l) {
  if (!logbuf.initialized) {
    bufInit(&logbuf, &logbuf_data, LOG_BUF_SIZE, sizeof(log_t));
  }

  bufAdd(*l, &logbuf);
  return 0;
}

int podLog(char *fmt, ...) {
  va_list arg;
  log_t l = {.type = Message, .content = {{0}}};

  char *msg = l.content.message;
  /* Write the error message */
  va_start(arg, fmt);
  vsnprintf(msg, MAX_LOG_LINE, fmt, arg);
  va_end(arg);

  printf("%s\n", msg);
  fflush(stdout);

  static FILE *log_file = NULL;

  if (log_file == NULL) {
    log_file = fopen(LOG_FILE_PATH, "a+");

    if (!log_file) {
      fprintf(stderr, "Failed to Open Log File: " LOG_FILE_PATH);
    }
  } else {
    fprintf(log_file, "%s\n", msg);
    fflush(log_file);
    fsync(fileno(log_file));
  }

  return logEnqueue(&l);
}

void logDump(pod_state_t *state) {
  debug("Logging System -> Dumping");

  debug("mode: %s, ready: %d", pod_mode_names[getPodMode()],
        getPodField(&(state->ready)));

  debug("acl m/s/s: x: %f, y: %f, z: %f", getPodField_f(&(state->accel_x)),
        getPodField_f(&(state->accel_y)), getPodField_f(&(state->accel_z)));

  debug("vel m/s  : x: %f, y: %f, z: %f", getPodField_f(&(state->velocity_x)),
        getPodField_f(&(state->velocity_y)),
        getPodField_f(&(state->velocity_z)));

  debug("pos m    : x: %f, y: %f, z: %f", getPodField_f(&(state->position_x)),
        getPodField_f(&(state->position_y)),
        getPodField_f(&(state->position_z)));

  debug("skates   : %d", state->tmp_skates);
  debug("brakes   : %d", state->tmp_brakes);
}

void *loggingMain(void *arg) {
  debug("[loggingMain] Thread Start");

  pod_state_t *state = getPodState();
  logging_socket = connectLogger();

  if (logging_socket < 0) {
    // attempt to put the pod into shutdown mode if we are in the boot state
    // if we have already completed boot, then
    setPodMode(Shutdown, "Logging socket failed to connect.");
  }

  // Post to the boot sem to tell the main thread that we have initialized
  // Main thread will assert that pod mode is still Boot

  info("punching boot_sem to proceed");
  sem_post(state->boot_sem);

  // Start the log forwarding loop until shutdown
  log_t l;
  bool running = true;
  while (getPodMode() != Shutdown && running == true) {
    // Try to pop off a log_t that has been queued
    int r = bufPop(&l, &logbuf);

    if (r == 0) {
      // Send the log
      int result = logSend(&l);

      // Failed to send logs.
      // Regardless of the reason, we need to stop
      if (result < 0) {
        running = false;
        fprintf(stderr, "Alert: Log Send Failed %d\n", result);
        setPodMode(Emergency, "Log Send Failed");
      }
    } else {
      // No logs in the queue, lets sit for a while
      usleep(LOGGING_THREAD_SLEEP);
    }
  }

  error("=== Logging system is going down ===");
  return NULL;
}
