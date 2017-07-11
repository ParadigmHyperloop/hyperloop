# hyperloop-core ![Build Status](https://ci.paradigmhyperloop.com/buildStatus/icon?job=ParadigmHyperloop/hyperloop-core/master)

The code for the Hyperloop Pod Control systems

# About

The Core Control Software found in this repository is designed to run in a RealTime Linux environment on a high performance 
ARM core. Suitable platforms include the TI Sitara am335x series of processors, convieniently found in the consumer and
industrial grade [BeagleBoard](http://beagleboard.org) Open Source Hardware. We utilize the BeagleBone black hardware 
and make extensive use of the onboard TI Sitara ARM core to manage the system level control loop for the Paradigm Pod

This breaks down into several responsibilies:
 - Maintain comunications with an external control point (Operations Center/Mission Control)
   - Stream Telemetry Data
   - Accept Commands
   - Exchange heartbeats to detect connection loss
   - Accept Software Updates
   - Enable Live Debugging and Testing of the pod's control system
 - Read and process information from sensors
   - Gather acceleration data from our IMU
   - Read various analog sensors attached to the Motherboard over SPI
     - Optical Distance Sensors
     - Inductive Proximity Sensors
     - Pressure transducers
     - Thermocouple Amplifiers
     - Pusher Detection Module
   - Communicate on a shared RS485 bus
     - Gather information from our tachometer/counter modules
     - Interact with our external status indicator module
 - Control our various outputs
   - Solenoids
   - Actuated Ball Valves
   - MPYE Valves
   - Main Power Board
   - Battery Packs

# Documentation

There is more documentation in this repository and on the internal Paradigm Google Drive.

* [Core Control Program Documentation](core/README.md)

# Getting Started

- If you are new to Paradigm be sure to check and say hi to the Paradigm
software team at one of our weekly meetings!
- If you are interested in contributing to any of Paradigm's software initiatives, please feel get in touch with us via http://paradigm.team or at
one of our meetings.  You can also jump right in and open a pull request if
you already see something you like!

The following is a high level getting started geared for new developers

## Overview

The minimal controls system is comprised of 5 distinct systems that you will need to install.

1. [The Core Controller](https://github.com/ParadigmTransportation/hyperloop-core) (this repo)
2. [The OpenLoop Data Shuttle](https://github.com/ParadigmTransportation/ODS)
3. [The PodCtl Tool](https://github.com/ParadigmTransportation/podctl)
4. [Influxdb](https://www.influxdata.com)
5. [Grafana](https://grafana.net)

You will also need sane C and Python development environments installed.

## Build Environment

### Mac

* You will need [Xcode](https://apple.com/xcode/)
* You will also need a semi-sane Python environment. Set that up on a mac with:

```
sudo easy_install pip
sudo pip install virtualenv
```

### Linux

```
sudo apt-get install build-esential python-pip
```

## Influxdb and Grafana

Install influxdb and grafana per the instructions on their respective websites

## Core Control Code

Clone this repo somewhere safe, then cd into it.

```
mkdir ~/dev
cd dev
git clone git@github.com:ParadigmTransportation/hyperloop-core.git
cd hyperloop-core
```

## ODS

Clone and run ODS in a new terminal window following the instructions on the
[ODS README](https://github.com/ParadigmTransportation/ODS)

## PodCTL

PodCTL is now bundled as part of the ODS Project

## Core Startup

You should now be running all 4 of the aforementioned services on your dev machine.  Now it is time to start up core

- Clone this repo and then cd into it.
- Run `make`
- To start the core controller in developer mode `make run`

You should see the core controller start with output like this

```
[INFO]  [main] {main.c:65} POD Booting...
[INFO]  [main] {main.c:66} Initializing Pod State
[WARN]  [get_pod_state] {pod.c:92} Pod State is not initialized
[DEBUG] [init_pod_state] {pod.c:61} initializing State at 0x1057f0240
[INFO]  [main] {main.c:73} Loading POD state struct for the first time
[INFO]  [main] {main.c:76} Registering POSIX signal handlers
[INFO]  [main] {main.c:83} Starting the Logging Client Connection
[DEBUG] [logging_main] {logging.c:230} [logging_main] Thread Start
[DEBUG] [log_connect] {logging.c:95} Connecting to logging server: pod-server.openloopalliance.com
[NOTE]  [log_connect] {logging.c:130} Connected to pod-server.openloopalliance.com:7778 on fd 5
[INFO]  [logging_main] {logging.c:244} punching boot_sem to proceed
[INFO]  [main] {main.c:101} Booting Command and Control Server
[DEBUG] [cmd_server] {commander.c:224} Starting TCP Network Command Server
[NOTE]  [cmd_server] {commander.c:231} TCP Network Command Server Started on port: 7779
[NOTE]  [cmd_server] {commander.c:239} === Waiting for first commander connection ===
```

The podctl terminal window should connect and show you a prompt like this

```
→ ./podctl.py
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Paradigm Pod CLI 0.0.1-alpha. Copyright 2016
This tool allows you to control various aspects of the pod
- TCP:7779
- STDIN

Available Commands:
 - help
 - ping
 - ready
 - brake
 - skate
 - status
 - emergency (alias: e)
 - exit
 - kill

>
```

The ODS window will start printing telemetry data to it's terminal window.
The telemetry data will be accessible in Grafana at http://localhost:3000 as
well

# Glossary

* **BBB**: Beagle Bone Black, an ARM micro-controller
* **IMU**: Inertial Measurement Unit, a really sick accelerometer
* **ODS**: OpenLoop Data Shuttle: the server that the pod sends telemetry data
  and logs to.

# Jenkins

A Jenkins server builds and tests all changes made to this repository
automatically.  Whenever you open a pull request, a build will be triggered and
a link to the results will be attached to the PR.

Before you checkin code, run the following to catch any errors before the
Jenkins server catches them

```
make clean all install test
```

# License

See the [LICENSE](LICENSE) for full licensing details.

In summary, (you still need to read the whole thing), this code is for
OpenLoop and OpenLoop only. It is shared with the world for the benefit of
observers and potential developers. If you wish to utilize this code in any
way, you must contact us first and receive written permission to utilize the
source for the purpose you require.

Lastly, DON'T trust this code to work for your HyperLoop pod, or your project.
This code is only being verified and tested on the OpenLoop platform.
