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
 * like "ebrake"
 *
 *   $ telnet <beaglebone> 7779
 *   Trying <ip>...
 *   Connected to <hostname>.
 *   Escape character is '^]'.
 *   ping  <<< You type this line as "ping" plus enter
 *   PONG  >>> BBB Command Server replies with "PONG\n"
 */

#include "pod.h"
#include <netinet/in.h>
#include <netinet/tcp.h>

int startTCPCommandServer(int portno) {
  int sockfd;
  struct sockaddr_in self;

  /*---Create streaming socket---*/
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket");
    exit(errno);
  }

  /*---Initialize address/port structure---*/
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(portno);
  self.sin_addr.s_addr = INADDR_ANY;

  /*---Assign a port number to the socket---*/
  if (bind(sockfd, (struct sockaddr *)&self, sizeof(self)) != 0) {
    perror("socket--bind");
    exit(errno);
  }

  /*---Make it a "listening socket"---*/
  if (listen(sockfd, 20) != 0) {
    perror("socket--listen");
    exit(errno);
  }

  return sockfd;
}

char *doCommand(const char *input) {
  printf("Recived Command %s\n", input);

  if (strncmp(input, CMD_EBRAKE, strlen(CMD_EBRAKE)) == 0) {
    if (setPodMode(Emergency) == 0) {
      return __STR__(CMD_EBRAKE) " => OK\n";
    } else {
      return __STR__(CMD_EBRAKE) " => FAIL\n";
    }
  } else if (strncmp(input, CMD_PING, strlen(CMD_PING)) == 0) {
    return CMD_PING_RES "\n";
  } else {
    return CMD_UNKNOWN_RES "\n";
  }
}

/**
 * Main function for the remote control thread. This thread operatates the
 * command server that will recieve commands from both stdin and over the TCP
 * command server
 */
void *commandMain(void *arg) {
  debug("[commandMain] Command Thread Started");
  int i, serverfd = startTCPCommandServer(CMD_SVR_PORT);

  debug("TCP Network Command Server Started on port: %d", CMD_SVR_PORT);

  char buffer[MAX_PACKET_SIZE];
  int clients[MAX_CMD_CLIENTS];
  int nclients = 0;
  fd_set active_fd_set, read_fd_set;

  /*---Forever... ---*/
  while (1) {
    /* Initialize the set of active sockets. */
    FD_ZERO(&active_fd_set);
    FD_SET(serverfd, &active_fd_set);
    FD_SET(fileno(stdin), &active_fd_set);
    for (i = 0; i < nclients; i++) {
      if (clients[i] >= 0) {
        FD_SET(clients[i], &active_fd_set);
      }
    }

    read_fd_set = active_fd_set;
    debug("WAITING ON COMMAND INPUT");
    // 1 second timeout allows for a log to ensure thread didn't die
    struct timeval tv = {1, 0};
    if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, &tv) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }
    debug("Command Thread select() Returned");
    if (FD_ISSET(serverfd, &read_fd_set)) {
      int clientfd;
      struct sockaddr_in client_addr;
      unsigned int addrlen = sizeof(client_addr);

      /*---accept a connection (creating a data pipe)---*/
      clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &addrlen);
      printf("%s:%d connected as %d\n", inet_ntoa(client_addr.sin_addr),
             ntohs(client_addr.sin_port), clientfd);

      // http://stackoverflow.com/questions/31426420/configuring-tcp-keepalive-after-accept
      int yes = 1;
      setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

      int idle = 1;
      setsockopt(clientfd, IPPROTO_TCP, TCP_KEEPALIVE, &idle, sizeof(int));

      int interval = 1;
      setsockopt(clientfd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

      int maxpkt = 10;
      setsockopt(clientfd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));

      if (recv(clientfd, buffer, MAX_PACKET_SIZE, 0) < 0) {
        printf("command: recv error");
        continue;
      }

      char *response = doCommand(buffer);

      /*---Echo back anything sent---*/
      send(clientfd, response, strlen(response), 0);

      if (nclients < MAX_CMD_CLIENTS) {
        clients[nclients] = clientfd;
        nclients++;
      }
    }
    if (FD_ISSET(fileno(stdin), &read_fd_set)) {
      read(fileno(stdin), &buffer[0], MAX_PACKET_SIZE);

      char *response = doCommand(buffer);

      printf("COMMAND RESPONSE (%s): %s\n", buffer, response);
    }

    int i;
    for (i = 0; i < nclients; i++) {
      if (clients[i] == -1) {
        debug("Skipping CLient Numver %d", i);
        continue;
      }

      if (FD_ISSET(clients[i], &read_fd_set)) {
        debug("Recv new command from existing client fd(%d)", clients[i]);
        if (recv(clients[i], buffer, MAX_PACKET_SIZE, 0) <= 0) {
          debug("command: recv error/empty");

          int j;
          for (j=i+1; j<nclients; i++) {
            clients[j-1] = clients[j];
          }

          warn("Removed Client %d", i);

          // Backup i once to repeat this index number
          i--;

          // Back down nclients
          nclients--;


          continue;
        }

        char *response = doCommand(buffer);

        /*---Echo back anything sent---*/
        send(clients[i], response, strlen(response), 0);
      }
    }
  }

  /*---Clean up (should never get here!)---*/
  close(serverfd);
}
