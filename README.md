# Reliable File Transfer Protocol (RFTP)

### An rdt 2.0–inspired file transfer system built over UDP

---

## Overview

RFTP is a simple, reliable file transfer protocol implemented on top of UDP. Since UDP does not provide guarantees for delivery, ordering, or duplication, this project recreates those guarantees using concepts from the Reliable Data Transfer (rdt 2.0) model.

The goal of this project is to demonstrate how reliability can be implemented manually at the application layer, rather than relying on TCP.

---

## Features

* Reliable data transfer over UDP
* Stop-and-wait protocol design
* Checksum-based error detection
* Acknowledgment-based communication (ACK)
* Automatic retransmission on timeout
* Resume support for interrupted transfers
* Chunk-based file transmission

---

## Protocol Design

### Connection Setup (Resume Mechanism)

Before sending file data, the client and server synchronize:

```
Client → START|file → Server  
Server → RESUME|<sequence_number> → Client
```

The server checks how much of the file has already been received and informs the client where to resume from.

---

### Data Transfer

Each packet follows this structure:

```
<sequence_number>|<data>|<checksum>
```

Flow:

* The client sends a packet
* The server verifies the checksum
* If valid, the server writes the data and sends an ACK
* If invalid, the packet is ignored
* The client retransmits if no valid ACK is received within a timeout

---

### Termination

The client sends an `EOF` message to indicate the end of the file transfer.

---

## Packet Structure

| Field           | Description                       |
| --------------- | --------------------------------- |
| Sequence Number | Tracks packet order               |
| Data            | File chunk (up to 512 bytes)      |
| Checksum        | Sum of byte values for validation |

---

## Implementation Details

### Client

* Reads the file in fixed-size chunks
* Computes checksum for each chunk
* Sends packets and waits for acknowledgment
* Retransmits packets on timeout
* Resumes transfer based on server response

### Server

* Listens for incoming packets
* Verifies checksum before accepting data
* Writes valid data to file
* Sends acknowledgment for each received packet
* Supports resuming transfers using file size

---

## Technology Stack

* Language: C
* Networking: UDP (Winsock2)
* Platform: Windows

---

## How to Run

### Compile

```
gcc server.c -o server -lws2_32
gcc client.c -o client -lws2_32
```

### Run Server

```
./server
```

### Run Client

```
./client
```

Enter the server IP when prompted.

---

## File Behavior

* The client reads from `file.txt`
* The server writes to `received_file.txt`
* If the transfer is interrupted, it resumes from the last successfully received chunk

---

## Limitations

* Does not handle packet loss explicitly (limitation of rdt 2.0)
* Uses a simple checksum instead of a stronger error detection method
* Stop-and-wait protocol limits throughput
* No encryption or security features

---

## Future Improvements

* Extend to rdt 3.0 to handle packet loss
* Implement sliding window protocols (Go-Back-N or Selective Repeat)
* Replace checksum with CRC
* Add file metadata (name, size)
* Introduce encryption for secure transfer

---

## Learning Outcomes

This project helps in understanding:

* How reliable transport protocols work
* The difference between UDP and TCP
* Error detection and retransmission strategies
* Trade-offs in protocol design

---

## Notes

This implementation focuses on clarity and correctness rather than performance. It is intended as a learning tool and a foundation for more advanced protocol designs.

---
