#!/bin/bash
sleep 15
echo "System rebooted, KNX launcher running..."
stty -F /dev/ttyAMA0 19200 cs8 parenb -parodd -icrnl -opost -isig -icanon -echo
echo "serial ttyAMAO configured, this are the new settings:"
stty -F /dev/ttyAMA0 -a
echo "Starting KNX URL Gateway server..."
/usr/local/gateway/build/urlGateway
echo "Starting KNX monitoring at file knxMonitor.log..."
cat /dev/ttyAMA0 | od -tx1 >> knxMonitor.log








