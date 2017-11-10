# Core

All the source that compiles to form the controller, command server, and
logging client. You will also find the state machine, hardware initialization
and other important sources in this folder.

# High Level Design Overview

## Threads

  * Each module has its own thread (commands, control loop, logging).
  * Threads also improve efficiency of the entire codebase by parallelizing     
    tasks that may block

## State

There is a global pod state, `pod_t` struct that stores the state of the
entire pod, including mode (pushing, coasting, braking), kinematics (position,
velocity, acceleration), and sensor readings (pressure, temperature, etc)

Use `pod_t * pod = get_pod()` to get a pointer to the current pod
state.

## Pod mode
The pod's mode, `pod_mode_t`, defines the behavior of the control loop.

| V  | Enum Name | Description                         |
|----|-----------|-------------------------------------|
| -1 | NonState  | NULL State                          |
| 0  | POST      | Initializing software               |
| 1  | Boot      | Ready for configuration             |
| 2  | HPFill    | Fill Air Supply                     |
| 3  | Load      | Ready to load into tube             |
| 4  | Standby   | Pusher present, ready to arm        |
| 5  | Armed     | Vehicle is ready to receive a push  |
| 6  | Pushing   | Vehicle is being pushed             |
| 7  | Coasting  | Vehicle is coasting                 |
| 8  | Braking   | Vehicle is braking                  |
| 9  | Vent      | Vent remaining air from air supply  |
| 10 | Retrieval | Vehicle Safe                        |
| 11 | Emergency | Emergency condition, stop and vent  |
| 12 | Shutdown  | Shutdown the controller             |
| 13 | Manual    | Manual Teleoperation                |

The pod's mode should be retrieved with `pod_mode_t * mode = get_pod_mode()`

**Important** Any component wishing to update the `get_pod()->mode` shall
call `set_pod_mode(<new_mode>, <reason>)` to perform validation of the new
state. `<reason>` describes why the state changed for debugging and tracing.

## Pod Values

Most values are wrapped in a `pod_value_t`. This struct has the sole purpose of
wrapping the value in with a mutex to enable multithreaded read/write.

For example, the kinematic states (Acceleration, Velocity, and Position) are
stored as floats.  These values are read by the command thread and by the
control loop. Control loop will compute new values and grab a write lock on
`pod->accel_x.mutex` by calling the helper `set_value_f(&pod->accel_x, new_a)`.   

a pod_value_t may be a 32 bit integer or a 32 bit float. The data type is
unionized and therefore, one must take care to consistently use the proper
versions of the helper functions

 * `get_value` / `set_value` interpret the `pod_value_t` value as an int32
 * `get_value_f` / `set_value_f` interpret the `pod_value_t` value as an float

## Sensors

Sensors are represented and processed by the `sensor_t`, and the various
`*_sensor()` functions.  A `sensor_t` handles storage of the ADC and channel
numbers, raw and calibrated values as well as all the configuration for
converting raw ADC readings to meaningful values (quadratic mapping function),
and a bias for smoothing input.

```c
typedef struct {
  int sensor_id; // The internal id number for the sensor
  char name[MAX_NAME]; // The Human readable name of the sensor
  int adc_num; // ADC Number
  int input; // Input Channel
  pod_value_t value; // The last converted sensor value
  pod_value_t raw; // The most recent raw ADC reading (-1 if no recent value)
  double cal_a; // quadratic mapping coefficients value = ax^2 + bx + c
  double cal_b;
  double cal_c;
  double alpha; // low pass filter alpha
  double offset; // Manual linear offset
} sensor_t;
```

## Batteries

Battery packs have many important metrics that must be collected.
`battery_pack_t` struct bundles all this telemetry into a single structure

```c
typedef struct {
  sensor_t voltage;
  sensor_t current;
  sensor_t temperature;
  sensor_t charge;
  sensor_t remaining_time;
} pod_battery_t;
```

## Controller Resets

The software has 3 different shutdown modes:
* Cold Shutdown - Exit and don't come back
* Warm Reboot - Kill threads, Reinitialize the pod state, and start new threads
* Cold Reboot - Exit w/ an exit status that a wrapper will read and restart

```c
typedef enum pod_shutdown {
  Halt = 0,
  WarmReboot = 1,
  ColdReboot = 2,
} pod_shutdown_t;
```

## Flight Profiles

This is subject to change as control strategies change.  A flight profile is
provided over the remote command interface (See the Commands documentation) and
stored in a `flight_profile_t`

```c
// Used to configure flight profiles
typedef struct flight_profile {
  useconds_t watchdog_timer; // Main braking timeout initiated by pushing state
                             // 5MPH 4300000
  useconds_t emergency_hold; // time held in the emergency state
  useconds_t braking_wait;   // Time before engaging secondary brake, if needed
  useconds_t braking_hold;   // min time to hold brakes before vent

  useconds_t pusher_timeout; // Timeout for the pusher plate debounce
  float pusher_state_accel_min; // m/s/s Threshold for transitioning into the
                                // pushing state
  useconds_t pusher_state_min_timer; // Minimium time in the pushing state
  float pusher_distance_min; // mm Distance to register the pusher as present
  float primary_braking_accel_min; // m/s/s min acceptable acceleration while
                                   // braking
  pthread_rwlock_t lock;
} flight_profile_t;
```


# Sensor Numbers / Naming conventions

_See Pod Layout Below for orientation_

There are various sensors on the pod, and the software is designed to easily
adapt to a changing set of inputs and outputs. Most of the sensors and
actuators are defined in the `core/config-....h` files.

Sensors and actuators are named with a descriptive name followed by a number or
letter if there are multiple copies of the same sensor. Letters and Numbers
always start with the front left corner of the pod and then zig-zag
right-to-left as if you were reading a book. For example: There are 4 low
pressure packages A, B, C, and D. A is front-left, B is Front-right, C is
rear-left, D is rear-right. The same applies for sensor indices (starting at
zero in the front-left most)

# Cartesian Coordinates

The code is written with a standard Right hand Cartesian coordinate system in
mind. Take your **right hand**, point your index finger forward down the
imaginary Hyperloop tube in front of you (X), point your Middle out to the left
(Y), stick your thumb up to the sky (Z). This is the coordinate system we
assume.
