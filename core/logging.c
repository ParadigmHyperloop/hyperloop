#include "pod.h"


void error(char *err) {
  printf("%s\n", err);
  exit(102);
}

int connectLogger() {
  int sockfd, portno;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname;

  hostname = LOG_SVR_NAME;
  portno = LOG_SVR_PORT;

  /* socket: create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    error("ERROR opening socket");
    exit(101);
  }

  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(hostname);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host as %s\n", hostname);
    exit(101);
  }

  /* build the server's Internet address */
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr,
        server->h_length);
  serveraddr.sin_port = htons(portno);

  /* connect: create a connection with the server */
  if (connect(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
    error("Connection Refused");

  return sockfd;
}

int logSend(log_t *log) {
  static int logging_socket = -1;

  char buf[MAX_PACKET_SIZE];

  if (logging_socket < 0) {
    logging_socket = connectLogger();
  }

  snprintf(&buf[0], MAX_PACKET_SIZE, "POD%d%s\n", 1, log->content.message);

  /* send the message line to the server */
  int n = write(logging_socket, buf, strlen(buf));
  if (n < 0)
    error("ERROR writing to socket");

  /* print the server's reply */
  bzero(buf, MAX_PACKET_SIZE);

  return 0;
}

int podLog(char *fmt, ...) {
  va_list arg;

  /* Write the error message */
  va_start(arg, fmt);
  char msg[MAX_PACKET_SIZE];
  vsnprintf(&msg[0], MAX_PACKET_SIZE, fmt, arg);
  va_end(arg);

  printf("%s", msg);

  FILE *log_file = fopen(LOG_FILE_PATH, "a+");
  if (!log_file) {
    error("Failed to Open Log File: " LOG_FILE_PATH);
  }
  fwrite(fmt, 1, strlen(&msg[0]), log_file);
  fflush(log_file);
  fsync(fileno(log_file));

  log_t l = {.type = 1, .content = {&msg[0]}};

  return logSend(&l);
}

void *loggingMain(void *arg) {
  debug("[loggingMain] Thread Start");

  pod_state_t *state = getPodState();

  while (1) {

    debug("Logging System -> Dumping");

    // TODO: Use the freaking Mutexes
    debug("acl: x: %d, y: %d, z: %d", getPodField(&(state->accel_x)),
          getPodField(&(state->accel_x)), getPodField(&(state->accel_x)));

    debug("vel: x: %d, y: %d, z: %d", getPodField(&(state->velocity_x)),
          getPodField(&(state->velocity_y)), getPodField(&(state->velocity_z)));

    debug("pos: x: %d, y: %d, z: %d", getPodField(&(state->position_x)),
          getPodField(&(state->velocity_y)), getPodField(&(state->velocity_z)));

    usleep(LOGGING_THREAD_SLEEP);
  }
}
