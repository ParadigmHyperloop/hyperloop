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

/**
 * This file is responsible for handling human generated commands for manual
 * manipulation of the pod's internal state object.
 *
 * For example: An operator would give a command to set the state of the pod to
 * Emergency to engage emergency braking.  This directive overrides any and all
 * directives delivered by the pod's internal state computations, i.e. the
 * decisions made based on data from the IMU or from the photoelectric sensors
 *
 * Usage: Commands can be issued on the CLI where hyperloop-core was launched,
 * or by connecting to the command server on port 7779 and typing a command,
 * like "clamp"
 *
 *   $ telnet <beaglebone> 7779
 *   Trying <ip>...
 *   Connected to <hostname>.
 *   Escape character is '^]'.
 *   ping  <<< You type this line as "ping" plus enter
 *   PONG  >>> OPC Command Server replies with "PONG\n"
 */
#include "commander.h"

#include <netinet/in.h>
#include <netinet/tcp.h>

extern command_t commands[];


static
void commander_init(commander_t *commander) {
  memset(commander, 0, sizeof(commander_t));
  commander->first_client = true;
}

// Much of this code is based on this UTAH tcpserver example, thanks!
// https://www.cs.utah.edu/~swalton/listings/sockets/programs/part2/chap6/simple-server.c

/**
 * Start the TCP server: open the listen socket and bind to the given port
 */
int cmd_start_tcp_server(int portno) {
  int fd;
  struct sockaddr_in self;

  // Create a socket
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    error("Command Server socket() %s", strerror(errno));
    return -1;
  }

  int option = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
#ifdef SO_REUSEPORT
  setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &option, sizeof(option));
#endif
  
  setsockopt(fd, SOL_SOCKET, SO_LINGER, &option, sizeof(option));

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

  // Initialize address/port structure
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(portno);
  self.sin_addr.s_addr = INADDR_ANY;

  // Assign a port number to the socket
  if (bind(fd, (struct sockaddr *)&self, sizeof(self)) != 0) {
    error("Command Server Socket bind() %s", strerror(errno));
    return -1;
  }

  // Make it a "listening socket"
  if (listen(fd, 20) != 0) {
    error("Command Server listen() %s", strerror(errno));
    return -1;
  }

  return fd;
}

/**
 * Process A single command
 */
int cmd_do_command(size_t inputc, char *input, size_t outputc, char output[]) {
  int i = 0;
  int count = 0;
  while (commands[i].name != NULL) {
    size_t len = strlen(commands[i].name);
    if (inputc < len) {
      len = inputc;
    }
    if (strncmp(commands[i].name, input, len) == 0) {
      break;
    }
    i++;
  }

  if (commands[i].name == NULL) {
    count = snprintf(output, outputc, "FAIL: Unknown Command, try 'help'");
    return count;
  }

  char *argv[CMD_MAX_ARGS];
  argv[0] = input;
  size_t ci, argc = 1;

  for (ci = 0; ci < (inputc - 1) && argc < CMD_MAX_ARGS; ci++) {
    if (input[ci] == ' ') {
      input[ci] = '\0';
      argv[argc++] = input + ci + 1;
    } else if (input[ci] == '\r' && ci == inputc - 2) {
      input[ci] = '\0';
    }
  }

  argv[argc] = NULL;

  int retval = commands[i].func(argc, argv, outputc, output);

  if (retval < 0) {
    warn("Command Failed with exit code %d", retval);
  }

  return retval;
}

/**
 * Accept the connection from the waiting client and set the socket params
 */
int cmd_process_client(int fd) {
  int clientfd;
  struct sockaddr_in client_addr;
  unsigned int addrlen = sizeof(client_addr);

  // Accept the connection
  clientfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
  info("Client %s:%d connected as fd %d", inet_ntoa(client_addr.sin_addr),
       ntohs(client_addr.sin_port), clientfd);

  if (clientfd < 0) {
    return -1;
  }

  // http://stackoverflow.com/questions/31426420/configuring-tcp-keepalive-after-accept
  int yes = 1;
  setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

  int interval = 1;
  setsockopt(clientfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

  int maxpkt = 10;
  setsockopt(clientfd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));

  return clientfd;
}

/**
 * Given a server file descriptor, accept the new client cmd_respond with an
 * error
 * message, and then close them down
 */
int cmd_reject_client(int fd) {
  int clientfd = cmd_process_client(fd);

  if (clientfd < 0) {
    return -1;
  }

  send(clientfd, POD_ETOOMANYCLIENTS_TXT, strlen(POD_ETOOMANYCLIENTS_TXT), 0);

  close(clientfd);
  return 0;
}


/**
 * Given a server file descriptor, accept the new client and return the
 * client's filedescriptor
 */
int cmd_accept_client(int fd) {
  int clientfd = cmd_process_client(fd);

  if (clientfd < 0) {
    return -1;
  }

  return clientfd;
}

/**
 * Read in a command, process it with cmd_do_command(), and write out the
 * respnse
 */
int cmd_process_request(int infd, int outfd, commander_t *commander) {
  char inbuf[MAX_PACKET_SIZE];
  
  ssize_t nbytes = read(infd, &inbuf[0], MAX_PACKET_SIZE - 1);

  if (nbytes <= 0) {
    warn("read error on fd %d: %s", infd, strerror(errno));
    return -1;
  }

  int base = 0;
  inbuf[nbytes] = '\0';

  debug("Recv new command '%s' from existing client (fd %d)", inbuf, infd);

  int i = 0;
  int nbytesout = 0;
  while (i < nbytes) {
    if (inbuf[i] == ';') {
      inbuf[i] = '\0';

      // Process the command
      nbytesout = cmd_do_command(i - base, &inbuf[base], CMD_OUT_BUF, commander->cmdbuffer);
      write(outfd, commander->cmdbuffer, nbytesout);
      base = i + 1;
    }
    i++;
  }
  
  nbytesout = cmd_do_command(i - base, &inbuf[base], CMD_OUT_BUF, commander->cmdbuffer);
  commander->cmdbuffer[nbytesout] = '\n';
  nbytesout++;
  write(outfd, commander->cmdbuffer, nbytesout);
  return 0;
}

/**
 * Muxes all command inputs from the TCP clients and STDIN
 */
int cmd_server() {
  // TODO: There is a minor bug where one of the input/output buffers is not
  // being cleared or capped with a null terminator
  commander_t commander;
  commander_init(&commander);

  debug("Starting TCP Network Command Server");
  commander.serverfd = cmd_start_tcp_server(CMD_SVR_PORT);

  if (commander.serverfd < 0) {
    return -1;
  }

  note("TCP Network Command Server Started on port: %d", CMD_SVR_PORT);

  fd_set active_fd_set, read_fd_set;

  note("=== Waiting for first commander connection (%d) ===", CMD_SVR_PORT);

  while (get_pod_mode() != Shutdown) {
    // Setup All File Descriptors we are going to read from
    FD_ZERO(&active_fd_set);
    FD_SET(commander.serverfd, &active_fd_set);
    FD_SET(STDIN_FILENO, &active_fd_set);
    int i;
    for (i = 0; i < commander.nclients; i++) {
      if (commander.clients[i] >= 0) {
        FD_SET(commander.clients[i], &active_fd_set);
      }
    }

    read_fd_set = active_fd_set;

    // 1 second timeout allows for a log to ensure thread didn't die
    struct timeval tv = {10, 0};

    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0) {
      error("select() failed");
      continue;
    }

    // New Clients (serverfd)
    if (FD_ISSET(commander.serverfd, &read_fd_set)) {
      if (commander.nclients >= MAX_CMD_CLIENTS - 1) {
        // Send an error response and close them down
        cmd_reject_client(commander.serverfd);
      } else {
        // Accept the client
        int clientfd = cmd_accept_client(commander.serverfd);
        commander.clients[commander.nclients] = clientfd;
        commander.nclients++;

        // We wait for a client to connect before the pod progresses with it's
        // boot phase (i.e. it starts up it's core)
        if (commander.first_client) {
          commander.first_client = false;
          sem_post(get_pod()->boot_sem);
        }
      }
    }

    // STDIN
    if (FD_ISSET(STDIN_FILENO, &read_fd_set)) {
      if (cmd_process_request(STDIN_FILENO, STDOUT_FILENO, &commander) < 0) {
        panic(POD_LOGGING_SUBSYSTEM, "STDIN unprocessable for commands");
      }
    }

    // Existing Clients
    for (i = 0; i < commander.nclients; i++) {
      if (FD_ISSET(commander.clients[i], &read_fd_set)) {
        if (cmd_process_request(commander.clients[i], commander.clients[i], &commander) < 0) {
          // remove the client
          set_pod_mode(Emergency, "Operator Client %d (fd %d) disconnected", i,
                       commander.clients[i]);
          close(commander.clients[i]);
          int j;
          for (j = i + 1; j < commander.nclients; j++) {
            commander.clients[j - 1] = commander.clients[j];
          }
          warn("Removed Client %d", i);
          // Backup i once to repeat this index number
          i--;
          // Back down nclients
          commander.nclients--;
        }
      }
    }
    // pthread cancellation point
    usleep(0);
  }

  // Should not get here
  info("Closing command server socket");
  close(commander.serverfd);
  return 0;
}
/**
 * Main function for the remote control thread. This thread operatates the
 * command server that will recieve commands from both stdin and over the TCP
 * command server
 */
void *command_main(__unused void *arg) {
  int retval = cmd_server();

  if (retval < 0) {
    switch (get_pod_mode()) {
    case Boot:
    case Shutdown:
      set_pod_mode(Shutdown, "Command Server Shutdown");
      break;
    default:
      set_pod_mode(Emergency, "Command Server Failed");
    }
  }

  warn("Command Thread Exiting");
  return NULL;
}
