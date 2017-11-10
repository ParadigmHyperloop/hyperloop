import subprocess
import select
import socket
import time


TCP_IP = "127.0.0.1"
TCP_PORT = 7779
BUFFER_SIZE = 4096
COMMAND_WAITING_MESSAGE = 'Waiting for first commander connection'
MAX_TCP_RETRIES = 5

class ControllerInstance(object):
    def __init__(self, path, imu=None, POST=None):
        self._path = path
        self._imu = imu
        self._POST = POST
        self._process = None
        self._tcp = None

    def start(self):
        """Start the controller instance on the local machine"""
        args = [self._path]

        if self._imu:
            args = args + ["-i", self._imu]

        if self._POST:
            args = args + ["-t"]

        print("Starting Controller: {}".format(args))

        p = subprocess.Popen(args, stdin=subprocess.PIPE,
                             stderr=subprocess.PIPE, stdout=subprocess.PIPE)

        self._process = p

        print("Started Controller: {}".format(p.pid))

        print("Connecting to {}:{}".format(TCP_IP, TCP_PORT))

        for i in range(0, MAX_TCP_RETRIES):
            try:
                client = socket.create_connection((TCP_IP, TCP_PORT), 1)
            except Exception as e:
                print("Connect Failure: {}".format(e))
                if i == MAX_TCP_RETRIES - 1:
                    self.shutdown()
                    return False

        print("Connected!")

        self._tcp = client

        response = self.command(['ping'], timeout=1)

        if response and "PONG" in response:
            print("Successful Heartbeat with controller")
            return True
        else:
            print("Failed to ping controller")

            self.shutdown()
            return False

    def shutdown(self):
        """Shutdown the process and cleanup the TCP session"""
        # Kill the TCP Session
        if self._tcp:
            print("Controller Shutting Down TCP")
            self._tcp.close()
            self._tcp = None

        # Kill the process
        if self._process:
            print("Controller terminating process")
            self._process.terminate()
            time.sleep(1)
            if self.alive():
                self._process.kill()

            self._process = None

    def stderr(self):
        """Get the stderr of the process"""
        if self._process:
            return self._process.stderr
        return None

    def stdout(self):
        """Get the stdout of the process"""
        if self._process:
            return self._process.stdout
        return None

    def stdin(self):
        """Get the stdin of the process"""
        if self._process:
            return self._process.stdin
        return None

    def command(self, command, timeout=1):
        """Send a command to the Remote Command Server and get the response"""
        if self.alive():
            print("Command: {}".format(command))
            self._tcp.send(' '.join(command) + '\n')
            ready = select.select([self._tcp], [], [], timeout)
            if self._tcp in ready[0]:
                data = self._tcp.recv(BUFFER_SIZE)
                print("Response: {}".format(data))
                return data
            print("Command Timwed Out: {}".format(command))

        return None

    def alive(self):
        """Determine if the process is alive and running"""
        try:
            return self._process and self._process.poll() is None
        except Exception as e:
            return False
