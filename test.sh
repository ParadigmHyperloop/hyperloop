#!/bin/bash
$1 -t -i - &
TESTPID=$!
echo "Started $TESTPID"

sleep 5
echo Connecting Mock Commander
(sleep 60; echo exit 0; sleep 1) | telnet localhost 7779
echo Sent exit command to controller
wait $TESTPID
EXIT=$?
echo PID $TESTPID Exited with $EXIT
exit $EXIT
