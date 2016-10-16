#!/usr/bin/env python2.7
import sys
import os
from datetime import datetime
import logging
import SocketServer
from influxdb import InfluxDBClient

BASE_PATH = '.'
INFLUX_HOST = 'localhost'
INFLUX_PORT = 8086
INFLUX_USER = 'root'
INFLUX_PASS = 'root'
INFLUX_NAME = 'example'


class LoggingHandler(SocketServer.StreamRequestHandler):
    """
    The request handler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """
    # def __init__(self, data_file, log_file):
    #     self.data_file = data_file
    #     self.log_file = log_file
    #     SocketServer.StreamRequestHandler.__init__(self)

    def handle(self):
        startTime = datetime.utcnow().strftime("%Y-%m-%d %H:%M:%S")

        log_fname = os.path.join(BASE_PATH, "logging" + startTime + ".csv")
        data_fname = os.path.join(BASE_PATH, "data" + startTime + ".csv")

        self.influx = InfluxDBClient(INFLUX_HOST, INFLUX_PORT, INFLUX_USER,
                                     INFLUX_PASS, INFLUX_NAME)

        self.influx.create_database(INFLUX_NAME)

        self.log_file = open(log_fname, 'w+')
        self.data_file = open(data_fname, 'w+')

        # self.rfile is a file-like object created by the handler;
        # we can now use e.g. readline() instead of raw recv() calls
        while True:

            self.data = self.rfile.readline()

            # if disconnected, then break
            if not self.data:
                break

            self.data = self.data.strip("\r\n ")

            logging.debug("[DATA] {}".format(self.data))

            response = None

            if self.valid(self.data):
                pkt_type = int(self.data[3])
                if pkt_type == 1:
                    response = self.handle_log(self.data[4:])
                elif pkt_type == 2:
                    response = self.handle_data(self.data[4:])
            else:
                logging.warn("[DROP] '{}'".format(self.data))

            # Likewise, self.wfile is a file-like object used to write back
            # to the client
            if response is None:
                self.wfile.write("FAIL: " + self.data + "\n")
            else:
                self.wfile.write(response + "\n")

    def handle_data(self, msg):
        logging.info("[DATA] {}".format(msg))
        self.data_file.write(makeLine(msg))
        (name, value) = msg.split(' ', 1)

        value = float(value)

        measurement = [
            {
                "measurement": name,
                "tags": {},
                "time":  datetime.utcnow().isoformat() + "Z",
                "fields": {
                    "value": value
                }
            }
        ]

        self.influx.write_points(measurement)

        return "OK: ({},{})".format(name, value)

    def handle_log(self, msg):
        logging.info("[LOG] {}".format(msg))
        self.log_file.write(makeLine(msg))

        return "OK: LOGGED"

    def valid(self, s):
        return len(s) >= 4 and s[0:3] == "POD" and isPositiveInt(s[3])


def isPositiveInt(s):
    try:
        return int(s) > 0
    except ValueError:
        return False


def makeLine(msg):
    """Takes a message and prepends a timestamp to it for logging"""
    lineToWrite = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    return lineToWrite + msg + "\n"


def main():
    args = sys.argv[:]
    if (len(args) != 3 or not isPositiveInt(args[1])):
        print("Usage: control-point.py <port> <directory>")
        sys.exit(1)

    port = args[1]
    path = args[2]
    if path[-1] != '/':
        path = path + '/'

    print("Starting TCP Server on 0.0.0.0:{}".format(port))
    server = SocketServer.TCPServer(("0.0.0.0", int(port)), LoggingHandler)

    server.serve_forever()

if "__main__" == __name__:
    main()
