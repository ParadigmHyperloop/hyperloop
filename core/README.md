# hyperloop-core

The c code to run the OpenLoop HyperLoop pod. Runs on the BeagleBone Black board.

# Glossary

* **BBB**: Beagle Bone Black, an ARM microcontroller
* **IMU**: Inertial Measurement Unit, a really sick accelerometer

# High Level Overview

## threads

_Threads are the name of the game_

  * Each module has its own thread (braking, logging, IMU, ect).  This was done
    to enable better multi-core compatibility if/when the time comes when we
    actually have better multi-core hardware.
  * Threads also improve efficiency of the entire codebase by parallelizing     
    tasks that may block

## State

There is a global pod state, `pod_state_t` struct that stores the state of the
entire pod, including mode (pushing, coasting, braking), position, velocity,
acceleration, and other very important information.

Use `post_state_t * getPodState(void)` to get a pointer to the current pod
state. There is no global state variable accessible by other means.

### Pod mode
The pod's mode, `pod_mode_t`, is one of the following:

| V | Enum Name | Description                         |
|---|-----------|-------------------------------------|
| 0 | Boot      | initializing systems                |
| 1 | Ready     | idle, stationary, ready for push    |
| 2 | Pushing   | pusher engaged                      |
| 3 | Coasting  | pusher disengaged, just coasting    |
| 4 | Braking   | normal braking mode                 |
| 5 | Shutdown  | pod is in a safe state, do shutdown |
| 6 | Emergency | emergency braking                   |

The pod's mode should be retrieved with `pod_mode_t getPodMode(void)`

**Important** Any component wishing to update the `getPodState()->mode` shall
call `int setPodMode(pod_mode_t <new_mode>, char * <reason>)` to perform
validation of the new state. `<reason>` describes why the state is changing
for better log interpretation.

### Values

Most values are wrapped in a `pod_value_t` (don't ask, it's terrible). This
struct has the sole purpose of wrapping the value in with a mutex to enable
multithreaded read/write.

For example, the IMU currently will be directly setting the pod's Acceleration,
Velocity, and Position state variables (This will probably change with the
Kalman filter). The IMU (`imu.c`) will compute the new acceleration, obtain a
write lock on `getPodState()->accel_x.mutex`.  

Upon a successful lock, it will then write the new acceleration and release the
write lock.

Meanwhile, other systems, especially the logging module, will be reading
the pod state values and logging them to the BBB

## Division of Labor

The multi-module design is also meant to simplify and separate out the distinct
logical units of the pod.  To express this, here is an example of how a pod
might go about it's control decisions

* The pod is in a `Coasting` mode as determined by the IMU and previous events
* The IMU recalculates the pod's position_x on the track regularly
* The tracking module monitors the position using the pod state
* The tracking module determines the position_x is nearing the end of the tube
* The tracking module updates the pod mode to Braking
* The braking module monitors the pod's state for `Braking` or `Emergency`
* The Braking module observes that the state is now `Braking` and applies brakes
* The tracking module observes that the pod's velocity is too high for its
  distance to the end of the tube.  It sets the state to `Emergency`
* The braking module observes that the state is now `Emergency` and applies Emergency Braking

## Sensor numbers

_See Pod Layout Below for orientation_

These values are defined in [config.h](config.h). Numbers are used internally
to index control surfaces and sensors

### Skates

| No. | Location    |
|-----|-------------|
| 0   | FRONT LEFT  |
| 1   | FRONT RIGHT |
| 2   | MID LEFT    |
| 3   | MID RIGHT   |
| 4   | REAR LEFT   |
| 5   | REAR RIGHT  |

### Skates Regulator Thermocouples

| No. | Location    |
|-----|-------------|
| 0   | FRONT LEFT  |
| 1   | FRONT RIGHT |
| 2   | MID LEFT    |
| 3   | MID RIGHT   |
| 4   | REAR LEFT   |
| 5   | REAR RIGHT  |

### Skates Regulator Pressure Sensors

| No. | Location    |
|-----|-------------|
| 0   | FRONT LEFT  |
| 1   | FRONT RIGHT |
| 2   | MID LEFT    |
| 3   | MID RIGHT   |
| 4   | REAR LEFT   |
| 5   | REAR RIGHT  |


### Wheel Solenoids

| No. | Location |
|-----|----------|
| 0   | FRONT    |
| 1   | MID      |
| 2   | REAR     |

### Wheel Pressure Sensors

| No. | Location |
|-----|----------|
| 0   | FRONT    |
| 1   | MID      |
| 2   | REAR     |

### Wheel Thermocouple Sensors

| No. | Location |
|-----|----------|
| 0   | FRONT    |
| 1   | MID      |
| 2   | REAR     |


### Emergency Brake Solenoids

| No. | Location |
|-----|----------|
| 0   | FRONT    |
| 1   | MID      |
| 2   | REAR     |

### Emergency Brake Pressure Sensors

| No. | Location |
|-----|----------|
| 0   | FRONT    |
| 1   | MID      |
| 2   | REAR     |

### OMRON Sensors (height sensors)

// TODO: Confirm this for final numbers

| No. | Location    |
|-----|-------------|
| 0   | FRONT LEFT  |
| 1   | FRONT RIGHT |
| 2   | MID LEFT    |
| 3   | MID RIGHT   |
| 4   | REAR LEFT   |
| 5   | REAR RIGHT  |

### Sharp Sensors (lateral sensors)

// TODO: Confirm this for final numbers

| No. | Location    |
|-----|-------------|
| 0   | FRONT LEFT  |
| 1   | FRONT RIGHT |
| 2   | MID LEFT    |
| 3   | MID RIGHT   |
| 4   | REAR LEFT   |
| 5   | REAR RIGHT  |

### IMU

The IMU just has a device path `/dev/usbmodem-00000` or something like that.

# Operating System

At this time... we are just running a vanilla Linux on the BBB, this will
cause many issues with the standard scheduler that comes with Debian or
Angstrom.


# Contributing

The Basics for how this all works

## Compiling

```
$ make clean all
$ ./pod
```

Currently we are doing development on 64-Bit Apple MacBook Pro's with the
standard Xcode Toolkit (8.1 Beta-2) Apple LLVM version 8.0.0 (clang-800.0.42)

This should be irrelevant because we also are constantly testing and Compiling
on BBB hardware using gcc version 4.6.3 (Debian 4.6.3-14)

Basically, we are getting semi-broad coverage possible platforms in our
development, which can be painful at times, but it is much better to deal with
the cross platform issues now rather than later.

## Let's get on the same page

We have made some decisions on our own for naming things

### Pod Layout

The code is written with the following Physical layout in mind

  * Imagine the pod is in a tube running North-South.  The pod is traveling
    **North**.  We call the **North** end of the pod the **Front**, and the
    **South** end, the **rear**.  
  * We call the **West** side of the pod, the **Left**, and the **East** side
    of the pod the **Right**.
  * We call the **Top** the face farther from the Track, and the bottom, the
    face closer to the track (Also the face with the skates on it)

Cool! That wasn't so hard, was it? Ok so now assigning X, Y, and Z vectors.

  * Our Pod's direction of travel down this _strait_ **North-South** track is
    is called the **Positive X** direction.  If the pod were to reverse and
    start traveling backwards, we would have a negative Velocity **X** value.
  * If our pod shifts on the track a few milimeters to the **East** this would
    be a **Positive Y** movement in the code.  Likewise a shift in the **West**
    direction would be a **Negative Y** movement
  * Finally, as the pod turns on it's skates, it lifts off the track slightly,
    this is a **Positive Z** movement, and likewise, when the pod's skates turn
    off, this is a **Negative Z** movement.

Hopefully that was not too hard, I just wanted to make the frame of reference
we have been using for this code incredibly clear.

## Developer Notes

> I've Jotted down a lot of notes in this section that may seem trivial, but
I am putting them here anyways to hopefully save even the experienced developer a few minutes of head scratching and googling

### long vs. long long

The main cross platform issue you will find between your dev machine and a BBB
will likely be 32/64-Bit issues. A quick refresher:

  * `unsigned long foo;` guarantees that you will have at least 32 bits in
    `foo`, you will actually probably 64 on 64 bit systems, but only 32 on
    32-Bit systems.
  * `unsigned long long foo` guarantees that you will have at least 64 bits
    regardless of the architecture, even if it's a 32-Bit chip.

To combat this nonsense... we use the `uint32_t` types to
explicitly state how many bytes are required for the given
variable

### Run in 32-Bit mode on a 64-Bit machine
You can force your 64-Bit system to compile and run a program in 32-Bit mode
with the following:

```
$ CFLAGS="-arch i386" make clean all
$ arch -i386 ./main
```
