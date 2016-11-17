# hyperloop-core [![Build Status](https://travis-ci.org/openloopalliance/hyperloop-core.svg?branch=master)](https://travis-ci.org/openloopalliance/hyperloop-core)

The code for the Hyperloop Pod Control systems

* **Core**: the core C controller.
* **ODS**: receive logs from the pod over TCP
* **Client**: client (telnet) for interacting with the pod remotely

# Glossary

* **BBB**: Beagle Bone Black, an ARM micro-controller
* **IMU**: Inertial Measurement Unit, a really sick accelerometer
* **ODS**: OpenLoop Data Shuttle: the server that the pod sends telemetry data
  and logs to.

# Documentation

There is more documentation in this repository and on the OpenLoop Google Drive

* [Core Control Program Documentation](core/README.md)

# Getting Started

The following is a high level getting started geared for Openloop developers

## Usage

To fire it up locally on your mac you need 3 terminal windows and some `clang`

* You will need `Xcode` if you are developing on a Mac in order to get clang
* You will need a standard build environment if you are developing on Linux.
  * You may use a `clang/llvm` stack or a `gcc` stack, we support both

* You will also need a semi-sane Python environment, set that up on a mac with:
  ```
  sudo easy_install pip
  sudo pip install virtualenv
  ```

### Clone required sources

1. Clone this repo somewhere safe, then cd into it.
  ```
  mkdir ~/dev
  cd dev
  git clone git@github.com:openloopalliance/hyperloop-core.git
  cd hyperloop-core
  ```
2. Clone the appropriate support repo in `~/dev/hyperloop-core` for the IMU
  * Real Version
    ```
    git clone git@github.com:EdHurtig/libimu-private.git libimu
    ```
    _Note that it must be cloned as "libimu" not "libimu-private"_
  * Public Stub Version
    ```
    git clone https://github.com/openloopalliance/libimu.git
    ```
3. Build the `libimu` project so that the `hyperloop-core` project has an IMU
   implementation it can use
   ```
   cd libimu
   make && make install
   cd ..
   ```
3. In a *NEW* terminal window, Clone the ODS repo somewhere, this one can be
   cloned in `~/dev` or within your `~/dev/hyperloop-core` folder. We will call
   this window the *ODS Terminal Window*
   ```
   git clone git@github.com:openloopalliance/ODS.git
   cd ODS
   ```

### Startup The Environment

#### Control Point ODS server

1. In the ODS terminal window, fire up the ODS server: type `make run-ods`
    ```
→ make run
rm -f server.out server.err test-data.txt *.csv
pip install -r requirements.txt
Requirement already satisfied (use --upgrade to upgrade): influxdb==3.0.0 in /Users/ehurtig/dev/hyperloop-core/.env/lib/python2.7/site-packages (from -r requirements.txt (line 1))
Requirement already satisfied (use --upgrade to upgrade): python-dateutil==2.5.3 in /Users/ehurtig/dev/hyperloop-core/.env/lib/python2.7/site-packages (from -r requirements.txt (line 2))
Requirement already satisfied (use --upgrade to upgrade): pytz==2016.7 in /Users/ehurtig/dev/hyperloop-core/.env/lib/python2.7/site-packages (from -r requirements.txt (line 3))
Requirement already satisfied (use --upgrade to upgrade): requests==2.11.1 in /Users/ehurtig/dev/hyperloop-core/.env/lib/python2.7/site-packages (from -r requirements.txt (line 4))
Requirement already satisfied (use --upgrade to upgrade): six==1.10.0 in /Users/ehurtig/dev/hyperloop-core/.env/lib/python2.7/site-packages (from -r requirements.txt (line 5))
./server.py
Starting TCP Server on 0.0.0.0:7778
```

#### Pod controller

1. In the first terminal window (in `hyperloop-core`), fire up the core
   controller. type `make run` again
    ```
    → make run
    cd core && /Applications/Xcode.app/Contents/Developer/usr/bin/make run
    /Users/ehurtig/dev/hyperloop-core/core
    rm -f *.o *~ core
    gcc -Wall -std=gnu99  -pthread  core.c main.c imu.c braking.c lateral.c logging.c height.c pod.c pod-helpers.c commands.c commander.c   -o core
    ./core
    [INFO]  [main] {main.c:65} POD Booting...
    [INFO]  [main] {main.c:66} Initializing Pod State
    [WARN]  [getPodState] {pod.c:92} Pod State is not initialized
    [DEBUG] [initializePodState] {pod.c:61} initializing State at 0x1057f0240
    [INFO]  [main] {main.c:73} Loading POD state struct for the first time
    [INFO]  [main] {main.c:76} Registering POSIX signal handlers
    [INFO]  [main] {main.c:83} Starting the Logging Client Connection
    [DEBUG] [loggingMain] {logging.c:230} [loggingMain] Thread Start
    [DEBUG] [connectLogger] {logging.c:95} Connecting to logging server: pod-server.openloopalliance.com
    [NOTE]  [connectLogger] {logging.c:130} Connected to pod-server.openloopalliance.com:7778 on fd 5
    [INFO]  [loggingMain] {logging.c:244} punching boot_sem to proceed
    [INFO]  [main] {main.c:101} Booting Command and Control Server
    [DEBUG] [commandServer] {commander.c:224} Starting TCP Network Command Server
    [NOTE]  [commandServer] {commander.c:231} TCP Network Command Server Started on port: 7779
    [NOTE]  [commandServer] {commander.c:239} === Waiting for first commander connection ===
    ```

2. The controller is is waiting for an operator to connect to it. This is the
   first of many safety features that ensure that the pod does not "run away".
   To connect to the controller, *switch to a third terminal window* and type:

   ```
   telnet localhost 7779
   ```

   You should be prompted with the controller CLI.  This is a basic command
   line for interacting with the pod.  Should you disconnect prematurely, the
   pod will enter emergency mode as a safety measure.

   ```
   → telnet localhost 7779
    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    OpenLoop Pod CLI 0.0.1-alpha. Copyright 2016
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
3. The pod will then start the core controller loop. This loop reads data,
   Makes a decision on it, and then adjusts any control surfaces. However,
   In order to tell the pod to turn on the skates, you need to transition it
   to the `Ready` state.  This allows for pre-flight inspection of the pod
   before the controller takes over completely. To inform the controller that
   the pod is ready to go, issue the `ready` command on the Controller CLI
   that you just connected in step 4.

4. The controller will start the skates and monitor the IMU for motion in the
   `+X` direction.  As soon as it records this, it will change it's state to
   `Pushing`.  As soon as the acceleration drops to `-X`, the pod will set
   it's state to `Coasting`. Finally as soon as the X position is determined
   to be approaching the end of the track, the controller will engage the
   primary brakes and set the mode to `Braking`. If the controller detects
   an error in the sensor readings at any point it will declare an
   `Emergency`


## Makefiles

Most of the tasks you will need to do are handled by Makefiles. There is a top
level Makefile in this repository that provides a few helpful targets.

* Just typing "make" will compile all the projects
* `run`: Will start up the core controller (the thing that runs on the BBB)
* `run-ods`: Will start up the OpenLoop Data Shuttle server.

## Coding Practices

The code running on the BBB is mostly written in a single C application with
a few different threads for critically different tasks. This minimizes the
complexity of the application and dependency tree.

Anything running at the control point is fair game though. The control point
is not limited in resources like the BBB so the only rule we really impose is
that it works, it's tested, and it is reliable. We will have a full Dell R7**
server running in the control area to receive and process data from the pod.
The server will be virtualized with a VMware vSphere hypervisor that can host
dozens of virtual machines at once. We hope to keep the control point simple
for the time being though, so this is mostly over-preparing

# License

See the [LICENSE](LICENSE) for full licensing details.

In summary, (you still need to read the whole thing), this code is for
OpenLoop and OpenLoop only. It is shared with the world for the benefit of
observers and potential developers. If you wish to utilize this code in any
way, you must contact us first and receive written permission to utilize the
source for the purpose you require.

Lastly, DON'T trust this code to work for your HyperLoop pod, or your project.
This code is only being verified and tested on the OpenLoop platform.
