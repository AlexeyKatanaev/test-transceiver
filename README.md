# test-transceiver

A test project in C language for posix systems that emulates a udp server that
change state of the transciever and amplifier.
* “amplifier power” - possible values - “on” and “off” (tag 0x0, values 0 and 1).
* “transceiver mode” - possible values – “rx”, “tx” and “rxtx” (tag 0x1).

UPD messages has TLV format (tag-length-value) and one message can contain several commands.
Server reports the current state to stderr every second.

# TODO:
* Implement function to handle TLV message from UDP socket
* Add Amplifier entinty


## Requirements
* POSIX
* GNU Make >= 4.2
* GCC > 3.4


# Build & Run
Build server app via make
```
make all
```

Run server
```
./bin/server -p <port>
```

To test server use netcat
```
nc -u -l <port>
```
