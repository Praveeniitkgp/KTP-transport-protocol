# KTP-transport-protocol
This Project implements a custom transport protocol, **KTP (KGP Transport Protocol)**, on top of unreliable UDP communication. The protocol ensures in-order and reliable delivery using a window-based flow control mechanism.

## Features

- Custom socket API: `k_socket`, `k_bind`, `k_sendto`, `k_recvfrom`, `k_close`
- Sender/Receiver windows (`swnd`, `rwnd`)
- Shared memory for socket management
- Drop simulation via `dropMessage(p)`
- Multi-threaded control: thread R (recv), thread S (retransmit)
- File transfer test using `user1.c` and `user2.c`

## Files

- `ksocket.h`, `ksocket.c`: KTP socket API
- `initksocket.c`: Initializes threads and shared memory
- `user1.c`, `user2.c`: Test applications
- `Makefile`s for building library and executables
- `documentation.txt`: Explanation and experiment results

## Usage

```bash
make init      # Builds initksocket
make user1     # Builds user1
make user2     # Builds user2
./initksocket  # Starts KTP infrastructure
./user1        # Sender
./user2        # Receiver
