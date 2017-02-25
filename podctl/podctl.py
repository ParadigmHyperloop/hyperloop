#!/usr/bin/env python3
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
import time
import socket
import logging
import argparse
import sys
import select
import threading
from datetime import datetime, timedelta

MAX_MESSAGE_SIZE = 2048

logging.basicConfig(level=logging.WARN)


class Pod:
    def __init__(self, addr):
        self.sock = None
        self.addr = addr
        self.last_ping = datetime.now()

    def ping(self, _):
        self.send("ping")

        timed_out = (datetime.now() - self.last_ping > timedelta(seconds=10))
        if self.sock is not None and timed_out:
            print("PING TIMEOUT!")
            self.sock.close()
            self.sock = None

    def handle_data(self, data):
        if "PONG" in data:
            self.last_ping = datetime.now()
        else:
            sys.stdout.write(data)

    def command(self, cmd):
        self.send(cmd + "\n")

    def transcribe(self, data):
        logging.info("[DATA] {}".format(data))

    def send(self, data):
        if self.sock is None:
            return

        try:
            self.sock.send(data.encode('utf-8'))
        except Exception as e:
            logging.error(e)
            self.close()

    def recv(self):
        if self.sock is None:
            return

        try:
            return self.sock.recv(MAX_MESSAGE_SIZE).decode('utf-8')
        except Exception as e:
            logging.error(e)
            self.close()

    def connect(self):
        while True:
            try:
                self.sock = socket.create_connection(self.addr)
                break
            except Exception as e:
                logging.error(e)
                time.sleep(1)

    def close(self):
        if self.sock is not None:
            self.sock.close()
            self.sock = None

    def is_connected(self):
        return self.sock is not None


class Heart:
    def __init__(self, interval, callback):
        self.interval = interval
        self.callback = callback
        self.running = False

    def start(self):
        self.running = True
        while self.running:
            self.callback(self)
            time.sleep(self.interval)

    def stop(self):
        self.running = False


def loop(pod):
    print("CLI Connecting to {}:{}".format(pod.addr[0], pod.addr[1]))
    pod.connect()

    while pod.is_connected():
        (ready, _, _) = select.select([pod.sock, sys.stdin], [], [], 1)

        if pod.sock in ready:
            data = pod.recv()
            if data:
                pod.handle_data(data)

        if sys.stdin in ready:
            cmd = sys.stdin.readline()
            pod.command(cmd)


def main():
    parser = argparse.ArgumentParser(description="Openloop Command Client",
                                     add_help=False)

    parser.add_argument("-v", "--verbose", action="store_true")

    parser.add_argument("-p", "--port", type=int, default=7779,
                        help="Pod server port")

    parser.add_argument("-h", "--host", default="127.0.0.1",
                        help="Pod server hostname")

    parser.add_argument("-i", "--heartbeat-interval", default="200", type=int,
                        help="heartbeat interval (ms)")

    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG)

    pod = Pod((args.host, args.port))

    heart = Heart(args.heartbeat_interval / 1000.0, pod.ping)

    threading.Thread(target=heart.start).start()

    while True:
        loop(pod)


if "__main__" == __name__:
    main()
