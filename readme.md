# IPK PROJECT 1 - HTTP SERVER

### Author: Milan Hrabovský - xhrabo15

### Description:
A server that enable client to obtain info from server. Client can obtain, hostname, cpu name, cpu's load. 

## Installation and use

* Build code with command ```make```.
* Use command ```./hinfosvc {port}``` to create local server.
* Use command ```GET http://localhost:{port}/{request}``` - port has to be unsigned int

## Valid requests example

* hostname - return network name of the server host
```
./hinfosvc 12345 &
GET http://localhost:12345/hostname
DESKTOP-HPQJNF1
```
* cpu-name - return name of the server's cpu
```
./hinfosvc 12345 &
GET http://localhost:12345/cpu-name
AMD Ryzen 9 5900X 12-Core Processor
```
* load - returns server's cpu load
```
./hinfosvc 12345 &
GET http://localhost:12345/load
12%
```

### Sources
* Socket server: https://www.geeksforgeeks.org/socket-programming-cc/
* CPU load calculations: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux