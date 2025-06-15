ifneq (,$(wildcard .env))
    include .env
    export
endif

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++23

all: broker publisher subscriber

broker: broker.cpp
	$(CXX) $(CXXFLAGS) broker.cpp -o broker

publisher: publisher.cpp
	$(CXX) $(CXXFLAGS) publisher.cpp -o publisher

subscriber: subscriber.cpp
	$(CXX) $(CXXFLAGS) subscriber.cpp -o subscriber

run-broker:
	BROKER_IP=$(BROKER_IP) BROKER_PORT=$(BROKER_PORT) ./broker

run-publisher:
	BROKER_IP=$(BROKER_IP) BROKER_PORT=$(BROKER_PORT) ./publisher

run-subscriber:
	BROKER_IP=$(BROKER_IP) BROKER_PORT=$(BROKER_PORT) ./subscriber

clean:
	rm -f broker publisher subscriber
