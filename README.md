## About

**atom-engine** service facilitates conducting Atomic Swap trades with swap data persistence, leaving both clearing and custody to the client-side (ATOM DEX), eliminating the need of trusted 3rd parties.

The service is designed to run as a daemon on any sort of Unix. 
It operates a registry of swap requests (similar to orders in classic markets), so that one can host his own decentralized exchange based on the Atomic Swap technology.

At its core, **atom-engine** provides the following functionality:
1. Atomic Swap trade requests managed in-memory
2. Atomic Swap data persistence for facilitating client-side swaps

## How to build

**1. Install libraries: libevent, hiredis**

On Debian / Ubuntu:
```
sudo apt install libevent-dev
sudo apt install libhiredis-dev
```

**2. Run install scripts:**

```
sudo ./configure_swap_server
sudo ./make_swap_server
```

## Running the service

Before running, you have to

**1. Install Redis**

On Debian / Ubuntu:

```
sudo apt install wget curl build-essential tcl
sudo apt install redis-server
```

**2. Create the 'settings.ini' file in the 'build' directory**

Configure parameters in 'settings.ini':

```
[SwapServer]
Ip = <IP address to bind>
Port = <port to listen>
WorkersCount = <the number of handlers for incoming requests; as the engine is designed async, it is recommended to set the count to 1>

[Redis]
Ip = <IP addess of the Redis instance>
Port = <port listened by the Redis instance>
```

To run the service, either launch 'run_swap_server' or execute ./swap_server from the 'build' directory.


## API methods

Documentation is available [here](https://github.com/bitcoin-atom/atom-engine/blob/master/docs/METHODS.md).
