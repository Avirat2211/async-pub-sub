# Asynchronous Publish-Subscribe system in C++
---

## Features
- **Multithreaded broker** that handles multiple clients concurrently
- **Subscribers** can register to one or more topics
- **Publishers** can broadcast messages to a topic
- Thread-safe topic management using `std::mutex`
- Designed from scratch — no external messaging libraries

---

## Project Structure

```text
.
├── broker.cpp       # Central server (broker)
├── publisher.cpp    # Client that sends messages
├── subscriber.cpp   # Client that receives messages
├── Makefile         # Build and run commands
├── .env             # Configuration (IP/PORT)
├── .gitignore
└── README.md
```

---

## Setup

1. Clone the repo
   ```bash
       git clone https://github.com/Avirat2211/async-pub-sub
   ```
2. Create a `.env` file
   ```bash
       BROKER_IP=127.0.0.1
       BROKER_PORT=8080
   ```
3. Build all components
   ```bash
       make
   ```

## Running the system

1. Start the broker
   ```bash
       make run-broker
   ```  
2. Start the subscriber
   ```bash
       make run-subscriber
   ``` 
3. Start the publisher
   ```bash
       make run-publisher
   ```


## Future Work
- Authentication
- Encryption
- Persistence

