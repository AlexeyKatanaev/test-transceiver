# test-transceiver

A test project in C language for posix systems that emulates a udp server that
change state of the transciever and amplifier.
* “amplifier” - possible values - “on” and “off” (tag 0x0, values 0 and 1).
* “transceiver" - possible values – “rx”, “tx” and “rxtx” (tag 0x1, values 0, 1 and 2).

UPD messages has TLV format (tag-length-value) and one message can contain several commands.
Server reports the current state to stderr every second.


## Requirements
* Linux
* GNU Make >= 4.2
* GCC > 3.4
* Python 3.6+ (for tests)


# Build & Run
To build server
```
make all
```

To execute tests
```
make test
```

To run server
```
./bin/server -p <port>
```

To test server use netcat
```
TAG=\x00\x01
LENGTH=\x00\x01
VALUES=\0x02
echo -e -n "$(TAG)$(LENGTH)$(VALUES)" | nc -u -q 1 localhost <port>
```
Where:
```
+-------------+----------------+-------------------------------+
|  0-1 byte   |    2-3 byte    |          4-... byte           |
+-------------+----------------+-------------------------------+
| Device Tag  | Commands Count | Commands (1 byte per command) |
+-------------+----------------+-------------------------------+
```
