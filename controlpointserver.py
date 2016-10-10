#!/usr/bin/env python2.7
import sys
import socket
import datetime


def main():
    # read input
    args = sys.argv[:]
    if (len(args) != 3 or not isPositiveInt(args[1])):
        print("Usage: control-point.py <port> <directory>")
        sys.exit(1)

    serverPort = args[1]
    loggingPath = args[2]
    if loggingPath[-1] != '/':
        loggingPath = loggingPath + '/'

    server(serverPort, loggingPath)


def server(port, path):
    # initialize
    print("Starting server on port {}".format(port))

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(("localhost", int(port)))
    sock.listen(1)

    connection, addr = sock.accept()
    currentTime = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    loggingFile = open(path + "logging" + currentTime + ".csv", 'w+')
    dataFile = open(path + "data" + currentTime + ".csv", 'w+')

    # Process messages
    currentBuffer = ""
    while True:
        try:
            # Message size arbitrary, adjust if needed.
            rawMessage = connection.recv(1024)
            nextBuffer = ""

            if rawMessage is None or rawMessage == "":
                print "Null Message Recieved"
                break

            print "Recieved Message: {}".format(rawMessage)
            if rawMessage[-1] != "\n":
                nextBuffer = rawMessage[rawMessage.rfind("\n")+1:]
                rawMessage = rawMessage[:rawMessage.rfind("\n")+1]

            for message in rawMessage.splitlines():
                if len(currentBuffer) > 0:
                    message = currentBuffer + message
                    currentBuffer = ""
                print(message)

                if not validMessage(message):
                    print "DROPPING MESSAGE: {}".format(message)
                    continue

                messageCode = int(message[3])
                if messageCode == 1:
                    loggingFile.write(writeLine(message[4:].split("\n")))
                elif messageCode == 2:
                    dataFile.write(writeLine(message[4:].split(" ")))
            currentBuffer = nextBuffer
        except KeyboardInterrupt:
            loggingFile.close()
            dataFile.close()
            print("Server closing")
            exit()

    loggingFile.close()
    dataFile.close()
    print("Server is shutting down")
    sock.close()

    return 0


def validMessage(s):
    return len(s) >= 4 and s[0:3] == "POD" and isPositiveInt(s[3])


def isPositiveInt(s):
    try:
        return int(s) > 0
    except ValueError:
        return False


def writeLine(strings):
    lineToWrite = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    for string in strings:
        lineToWrite = lineToWrite + "," + string.replace(",", "\,")
    return lineToWrite + "\n"


if "__main__" == __name__:
    main()
