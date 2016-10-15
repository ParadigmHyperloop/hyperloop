#include "pod.h"
#include "commands.h"

command_t commands[] = {
  { .name = "help", .func = helpCommand },
  { .name = "ping", .func = pingCommand },
  { .name = "brake", .func = brakeCommand },
  { .name = "skate", .func = skateCommand },
  { .name = "emergency", .func = emergencyCommand },
  { .name = "e", .func = emergencyCommand },
  { .name = "kill", .func = killCommand },
  { .name = NULL }
};

int helpCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  int count = snprintf(&outbuf[0], outbufc, "%s",
        "OpenLoop Pod CLI " POD_CLI_VERSION ". Copyright " POD_COPY_YEAR "\n" \
         "This tool allows you to control various aspects of the pod\n" \
         " - TCP:" __XSTR__(CMD_SVR_PORT) "\n" \
         " - STDIN\n" \
         "\n" \
         "Available Commands:\n" \
         " - help\n" \
         " - ping\n" \
         " - brake\n" \
         " - skate\n" \
         " - emergency (alias: e)\n"
         " - kill\n");
  return count;
}

int pingCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  strncpy(&outbuf[0], "PONG", outbufc);
  return 4;
}

int brakeCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return 0;
}

int skateCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return 0;
}

int emergencyCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  return 0;
}

int killCommand(int argc, char *argv[], int outbufc, char outbuf[]) {
  panic(POD_COMMAND_SUBSYSTEM, "Command Line Initiated Kill Command");
  return -1;
}
