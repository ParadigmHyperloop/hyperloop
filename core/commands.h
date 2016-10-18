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
 * Sets the pod's ready-to-proceed bit to allow it to finish the boot
 */
int readyCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * Prints the pod's current status
 */
int statusCommand(int argc, char *argv[], int outbufc, char outbuf[]);

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
 * A command that exit the pod program
 */
int exitCommand(int argc, char *argv[], int outbufc, char outbuf[]);

/**
 * A command that immediately kills the controller
 */
int killCommand(int argc, char *argv[], int outbufc, char outbuf[]);
