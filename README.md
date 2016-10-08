# hyperloop-code

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

| V | Enum Name | Description                        |
|---|-----------|------------------------------------|
| 0 | Boot      | initializing systems               |
| 1 | Ready     | idle, stationary, ready for push   |
| 2 | Pushing   | pusher engaged                     |
| 3 | Coasting  | pusher disengaged, just coasting   |
| 4 | Braking   | normal braking mode                |
| 5 | Shutdown  | pod stationary and in a safe state |
| 6 | Emergency | emergency braking                  |

**Important** Any component wishing to update the `getPodState()->mode` shall
call `int setPodMode(<new_mode>)` to perform validation of the new state

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

# Operating System

At this time... we are just running a vanilla Linux on the BBB, this will
cause many issues with the standard scheduler that comes with Debian or
Angstrom.
