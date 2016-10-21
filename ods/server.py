#!/usr/bin/env python2.7
##
# Copyright (c) OpenLoop, 2016
#
# This material is proprietary of The OpenLoop Alliance and its members.
# All rights reserved.
# The methods and techniques described herein are considered proprietary
# information. Reproduction or distribution, in whole or in part, is forbidden
# except by express written permission of OpenLoop.
#
# Source that is published publicly is for demonstration purposes only and
# shall not be utilized to any extent without express written permission of
# OpenLoop.
#
# Please see http://www.opnlp.co for contact information
##

import os
from datetime import datetime
import logging
import SocketServer
import argparse
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
                logging.error("[DISCONNECT]")
                break

            self.data = self.data.strip("\r\n ")

            logging.debug("[DATA] '{}'".format(self.data))

            response = None

            if self.valid(self.data):
                pkt_type = int(self.data[3])
                if pkt_type == 1:
                    response = self.handle_log(self.data[4:])
                elif pkt_type == 2:
                    response = self.handle_data(self.data[4:])
                else:
                    logging.error("Unknown Type {}".format(pkt_type))
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

        try:
            value = float(value)
        except ValueError:
            return "ERROR: Bad Value '{}'".format(value)

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


if "__main__" == __name__:
    parser = argparse.ArgumentParser(description="Openloop Data Shuttle")

    parser.add_argument("-v", "--verbose", action="store_true")

    parser.add_argument("-p", "--port", type=int, default=7778,
                        help="Server listen port")

    parser.add_argument("-d", "--directory", default=BASE_PATH,
                        help="directory to store raw log and data files in")

    # Influx arguments
    parser.add_argument("--influx-host", default=INFLUX_HOST,
                        help="Influxdb hostname")

    parser.add_argument("--influx-port", default=INFLUX_PORT,
                        help="Influxdb port")

    parser.add_argument("--influx-user", default=INFLUX_USER,
                        help="Influxdb username")

    parser.add_argument("--influx-pass", default=INFLUX_PASS,
                        help="Influxdb password")

    parser.add_argument("--influx-name", default=INFLUX_NAME,
                        help="Influxdb database name")

    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)

    BASE_PATH = args.directory
    INFLUX_HOST = args.influx_host
    INFLUX_PORT = args.influx_port
    INFLUX_USER = args.influx_user
    INFLUX_PASS = args.influx_pass
    INFLUX_NAME = args.influx_name

    print("Starting TCP Server on 0.0.0.0:{}".format(args.port))
    server = SocketServer.TCPServer(("0.0.0.0", args.port), LoggingHandler)

    server.serve_forever()
