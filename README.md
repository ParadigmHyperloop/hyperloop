# Paradigm Hyperloop Pod Software [![Build Status](https://ci.paradigmhyperloop.com/buildStatus/icon?job=ParadigmHyperloop/hyperloop/master&build=19)](https://ci.paradigmhyperloop.com/job/ParadigmHyperloop/job/hyperloop/job/master/19/)

The code for the Paradigm Pod Control systems.

[![Paradigm Hyperloop Intro](https://user-images.githubusercontent.com/1410448/28060825-469cafb4-65dd-11e7-8c29-82d99712fb81.png)](https://www.youtube.com/watch?v=6h-bhs3t1jQ "Paradigm Hyperloop Intro")

# About

The Core Control Software found in this repository is designed to run in a
RealTime Linux environment on a high performance ARM core. Suitable platforms
include the TI Sitara am335x series of processors, conveniently found in the
consumer and industrial grade [BeagleBoard](http://beagleboard.org) Open Source
Hardware. We utilize the BeagleBone black hardware and make extensive use of
the onboard TI Sitara ARM core to manage the system level control loop for the
Paradigm Pod.

This breaks down into several responsibilities:

 - Maintain communications with an external control point (Operations Center/Mission Control)
   - Stream Telemetry Data
   - Accept Commands
   - Exchange heartbeats to detect connection loss
   - Accept Software Updates
   - Enable Live Debugging and Testing of the pod's control system
 - Read and process information from sensors
   - Gather acceleration data from our IMU
   - Read various analog sensors attached to the Motherboard over SPI
     - IR Distance/Proximity Sensors
     - Inductive Proximity Sensors
     - Pressure transducers
     - Thermocouple Amplifiers
   - Communicate on a shared RS485 bus
     - Gather information from our tachometer/counter modules
     - Interact with our external status indicator module
   - Communicate with our Battery Management Systems (USB-to-RS232)
 - Control our various outputs
   - Solenoids
   - Actuated Ball Valves
   - MPYE Valves
   - Main Power Board
   - Battery Packs

# Documentation

There is more documentation in this repository and on the internal Paradigm
Google Drive.

# Getting Started

- If you are new to Paradigm be sure to check in and say hi to the Paradigm
software team at one of our weekly meetings!
- If you are interested in contributing to any of Paradigm's software
initiatives, please feel get in touch with us via https://paradigm.earth or at
one of our meetings.  You can also jump right in and open a pull request if
you already see something you like!

The following is a high level getting started geared for new developers

# Overview

The minimal controls system is comprised of 4 distinct systems that you will need to install.

1. [The Core Controller](https://github.com/ParadigmHyperloop/hyperloop-core) (this repo)
2. [The OpenLoop Data Shuttle](https://github.com/ParadigmHyperloop/ODS)
3. [Influxdb](https://www.influxdata.com)
4. [Grafana](https://grafana.net)

You will also need clang/LLVM and Python development environments installed.

## Setup Build Environment

### Mac

* You will need [Xcode](https://apple.com/xcode/)

### Linux

```
sudo apt-get install build-essential python-pip clang llvm cmake
```

You will also need to build and install [LibBlocksRuntime](https://github.com/mackyle/blocksruntime)

## Setup Influxdb and Grafana

Install influxdb and grafana per the instructions on their respective websites

## Install ODS

Clone ODS in a new terminal window following the instructions on the
[ODS README](https://github.com/ParadigmHyperloop/ODS)

## Install & Build Core

Clone this repo somewhere safe, then cd into it.

```
mkdir ~/dev
cd ~/dev
git clone git@github.com:ParadigmHyperloop/hyperloop.git
cd hyperloop
```

This is a CMake project, meaning that you need to first build the build
system, then build the project with the new build system.

```
cd ./proj
cmake -DCMAKE_C_COMPILER=$(which clang) ..
```

You should get something like this:

```
-- The C compiler identification is Clang 3.8.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/clang
-- Check for working C compiler: /usr/bin/clang -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Configuring done
-- Generating done
-- Build files have been written to: /home/edhurtig/hyperloop-core/proj
```

Now if you run `make` you should get a built controller.

```
...
[ 94%] Building C object core/CMakeFiles/core.dir/telemetry.c.o
[ 97%] Building C object core/CMakeFiles/core.dir/tests.c.o
[100%] Linking C executable core
[100%] Built target core
```

## Core Startup

You should now be running all 3 of the aforementioned services (InfluxDB,
Grafana, ODS) on your dev machine.  Now it is time to start up core

From the `./proj` folder in a terminal window, run `./core/core -i -`

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

Start ODS if not already, the ODS terminal window will start printing telemetry
data. The telemetry data will be accessible in Grafana at
http://localhost:3000 as well, but you will need to configure a data source and
some dashboards (more on that later)

# Glossary

* **BBB**: Beagle Bone Black, an ARM micro-controller
* **IMU**: Inertial Measurement Unit, a really sick accelerometer
* **ODS**: OpenLoop Data Shuttle: the server that the pod sends telemetry data
  and logs to. [Click here](https://github.com/ParadigmHyperloop/ODS).

# Jenkins

A Jenkins server builds and tests all changes made to this repository
automatically.  Whenever you open a pull request, a build will be triggered and
a link to the results will be attached to the PR.

Before you checkin code, run the following to catch errors before the
Jenkins server catches them


```
make clean all install integration
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
