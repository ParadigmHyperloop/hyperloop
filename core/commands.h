
typedef struct {
  char *name;
  int (*func)(int argc, char *argv[], int outbufc, char outbuf[]);
} command_t;

/**
 * Print help
 */
int helpCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A simple command that just echos back the text "PONG"
 */
int pingCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A command that controls the brakes manually
 */
int brakeCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A command that controls the skates manually
 */
int skateCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A command that declares an emergency
 */
int emergencyCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A command that immediately kills the controller
 */
int killCommand(int argc, char *argv[], int outbufc, char outbuf[]);
