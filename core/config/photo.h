#ifndef _OPENLOOP_POD_CONFIG_PHOTO_
#define _OPENLOOP_POD_CONFIG_PHOTO_

// --------------------------
// Vertical
// --------------------------

#define N_CORNER_DISTANCE 4
#define CORNER_DISTANCE_MUX DISTANCE_MUX
#define CORNER_DISTANCE_INPUTS                                                 \
  { 0, 1, 2, 3 }

#define N_WHEEL_DISTANCE 3
#define WHEEL_DISTANCE_MUX DISTANCE_MUX
#define WHEEL_DISTANCE_INPUTS                                                  \
  { 4, 5, 6 }

// --------------------------
// Lateral
// --------------------------

#define N_LATERAL_DISTANCE 3
#define LATERAL_DISTANCE_MUX DISTANCE_MUX
#define LATERAL_DISTANCE_INPUTS                                                \
  { 7, 8, 9 }

// --------------------------
// Shell
// --------------------------

#define N_SHELL_PHOTO 4
#define SHELL_PHOTO_MUX PHOTO_MUX
#define SHELL_PHOTO_INPUTS                                                     \
  { 0, 1, 2, 3 }

// --------------------------
// Wheels
// --------------------------

#define N_WHEEL_PHOTO 3
#define WHEEL_PHOTO_MUX PHOTO_MUX
#define WHEEL_PHOTO_INPUTS                                                     \
  { 4, 5, 6 }

#endif
