# Remote Command API

The Remote Command Server is a simple TCP server that listens for
input commands formatted much like a simple shell. This makes it
incredibly easy to integrate with other tools like `telnet`, `nc`,
and with custom written applications as the only requirement is a
TCP client.

Let's look at an example command/response:

```
> state
< Pod Mode: 11 (Emergency)
```

All you need to do is `send("state\n")` and then call `recv(CMD_OUT_BUF)` (note there's caviats to this strategy).
Note that `CMD_OUT_BUF` is a constant defined in [core/config'h](core/config.h) and it is safe to assume it will
remain at 4096 for the time being.  Commands that must output more than 4096 bytes are not suited for this
connection (which is expected to be fast, efficent, and low bandwidth)

# Commands

Currently there are 20 commands registered in [core/commands.c](core/commands.c).





| command   | arguments          | description                                       | Example                                                                                                                                                                                                                                                    | Deprecated |
|-----------|--------------------|---------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|------------|
| emergency | none               | Declare and Emergency                             | > Emergency <br> < Pod Mode: 11                                                                                                                                                                                                                             |            |
| calibrate | none               | Zero out the IMU                                  | > calibrate <br> < CALIBRATION SET <br> < X: -0.000000 <br>< Y: -0.000000 <br>< Z: -0.000000                                                                                                                                                                                 |            |
| override  | <solenoid> <state> | Override a solenoid state                         | -                                                                                                                                                                                                                                                          | Deprecated |
| standby   | none               | Put the pod into Standby mode                     | > standby <br>< Failed to enter Standby                                                                                                                                                                                                                        |            |
| status    | none               | Dump the current state of the pod                 | > status <br>< mode: Emergency <br>< reason: Motherboard Demux Communication Failure <br>< acl m/s/s: x: 0.000000, y: 0.000000, z: 0.000000 <br>< vel m/s,: x: 0.000000, y: 0.000000, z: 0.000000 <br>< pos m,: x: 0.000000, y: 0.000000, z: 0.000000 <br>< Pusher Plate: INACTIVE |            |
| manual    | [[mask]...]        | Get or set the current manual valve overrides     | > manual <br>< 0 0 2 2 1 1 2 2 2 2 2048 2048 2048 2048 <br>> manual 1 1 0 0 1 1 2 2 2 2 2048 2048 2048 2048 <br>< 1 1 0 0 1 1 2 2 2 2 2048 2048 2048 2048                                                                                                              | [[Manual Args]]           |
| offset    | <sensor> <offset>  | Adjust the offset in for the given sensor         | > offset brake_tank_pressure_0 150 <br>< Changed offset of brake_tank_pressure_0: 0.000000 -> 150.000000. Reading: 133.744705                                                                                                                                  |            |
| ready     | none               | Unused                                            | > ready <br>< OK: SET POD READY BIT => 1                                                                                                                                                                                                                       | Deprecated |
| state     | [new_state]        | Get or Set the state of the pod                   | # Get the state <br>> state <br>< Pod Mode: 11 (Emergency) <br># Set the state to load state (3) <br>> state 3 <br>< Pod Mode: 3 (Load) <br># Set the state by name <br>> state Boot <br>< Pod Mode: 1 (Boot)                                                                              |            |
| reset     | none               | Perform a warm reset of the control software      | # Request a controller reset <br>> reset <br>< Reset Request Declined POD-775b748                                                                                                                                                                                  |            |
|fpget      | none               | Get the flight profile information                | >fpget<br>< watchdog_timer: 0<br>< emergency_hold: 0<br>< braking_wait: 0<br>< pusher_timeout: 0<br>< pusher_state_accel_min: 0.000000<br>< pusher_state_min_timer: 0<br>< pusher_distance_min: 0.000000<br>< primary_braking_accel_min: 0.000000||
| vent      | none               | Set the state to vent                             | > vent <br>< Venting Started                                                                                                                                                                                                                                   |            |
| pack      | <pack num> <0/1>   | Engage or disengage the given battery pack number | # Turn on Battery Pack 0 (Pack A / Port Side Battery Pack) <br> > pack 0 1 <br>< Set pack_0 to 1                                                                                                                                                                   |            |
| help      | none               | Print help message                                | > help <br>< Pod CLI v1.0.0-PD_GIT_SHA1_SHORT. Copyright 2016 Paradigm - Controls Team <br> < ...                                                                                                                                                                   |            |
| fill      | none               | Set the mode to fill                              | # Attempt to open the fill valve while pressure is in the system <br>> fill <br>< HP Fill Pre-Check Failure <br># Force the fill valve to open discarding safeguards <br>> state HPFill <br>< Pod Mode 2: HPFill                                                               |            |
| ping      | none               | Ping the controller (get back PONG)               | > ping <br>< PONG                                                                                                                                                                                                                                              |            |
| exit      | [code]             | Graceful shutdown                                 | # Exit Normally <br>> exit ... No Response/ TCP Server Disconnect ... <br># Exit with status code <br>> exit 3 ... No Response / TCP Server Disconnect ...                                                                                                             |            |
| push      | none               | Override Pusher Detector State                    | -                                                                                                                                                                                                                                                          | Deprecated |
| kill      | none               | Forceful kill.  Calls pod_panic()                 | # Immediately Kill <br>> kill ... No Response / TCP Server Disconnect ...                                                                                                                                                                                      |            |
| arm       | none               | Set the state to Armed                            | # Attempt to arm the pod <br>> arm <br>< Pusher plate is not depressed. Cannot Arm. # Attempt again <br>> arm <br>< Pod not ready to arm. core checklist failed. <br> # Attempt Again <br>> arm  <br>< Armed                                                                           |            |
| fp        | [[parameter] [value]...] | Change a parameter in the flight profile | > fp -w 100<br>< watchdog_timer: 100<br>< emergency_hold: 0<br>< braking_wait: 0<br>< pusher_timeout: 0<br>< pusher_state_accel_min: 0.000000<br>< pusher_state_min_timer: 0<br>< pusher_distance_min: 0.000000<br>< primary_braking_accel_min: 0.000000||
| e         | none               | alias for emergency                               | > e <br>< Pod Mode: 11                                                                                                                                                                                                                                         |            ||


# Manual Args

The `manual` command uses a simple format of specifying manual overrides,
but it is a pretty useless command without knowing what those arguments are.

```
> manual
< 0 0 2 2 1 1 2 2 2 2 2048 2048 2048 2048
> manual 1 1 0 0 1 1 2 2 2 2 2048 2048 2048 2048
< 1 1 0 0 1 1 2 2 2 2 2048 2048 2048 2048
```

To find this information, check the `manualCommand()` functtion in [core/commands.c](core/commands.c) where you can figure out how the command is
interpretted.

```
c->front_brake = atoi(argv[1]);
c->rear_brake = atoi(argv[2]);
c->vent = atoi(argv[3]);
c->fill = atoi(argv[4]);
c->battery_a = atoi(argv[5]);
c->battery_b = atoi(argv[6]);
c->skate_a = atoi(argv[7]);
c->skate_b = atoi(argv[8]);
c->skate_c = atoi(argv[9]);
c->skate_d = atoi(argv[10]);
c->mpye_a = (atoi(argv[11]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) + MPYE_A_SETPOINT;
c->mpye_b = (atoi(argv[12]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) + MPYE_A_SETPOINT;
c->mpye_c = (atoi(argv[13]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) + MPYE_A_SETPOINT;
c->mpye_d = (atoi(argv[14]) * (MPYE_B_SETPOINT - MPYE_A_SETPOINT) / 255) + MPYE_A_SETPOINT;
```

Currently the order of the valve overrides looks like this:

```
manual <brake_a> <brake_b> <vent> <fill> <pack_a> <pack_b> <skate_a> <skate_b> <skate_c> <skate_d> <mpye_a> <mpye_b> <mpye_c> <mpye_d>
```

**Brakes** take an integer 0, 1, or 2 which represents the state of the 5/3 brake solenoid. `0: Closed, 1: Engage, 2: Release`

**Vent, Fill, Batteries, and Skates** are all binary outputs (ON or OFF).  0 is OFF, 1 is ON. Note that for the vent valve 1 means ON which means power is applied to close it as it's a normally open valve.

**[MPYEs](https://www.festo.com/cat/ro_ro/data/doc_engb/PDF/EN/MPYE_EN.PDF)** are slide valves that take an input from 0-255 (ED CHECK THIS).  They are 5/3 slide valves so the midpoint (128) means the valve is closed. 0 means port A is fully open, 255 means port B is fully open. Any number inbetween is a proportion of A and B.
