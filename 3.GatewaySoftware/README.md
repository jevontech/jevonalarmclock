# jevonhome-core

## Introduction
JevonHome is meant to be a home automation framework that can integrate home automation hardware from various suppliers and brands

##Features v1.0
The following features are planned for version 1.0:

- basic rule engine that can execute commands based on incoming messages
- use a 433 MHz remote control to switch devices on and off

##Features v2.0
TBD

## Architecture and technology
Jevonhome is written in C++.
This application has a distributed architecture in a client-server setup.
The main server runs an AMQP broker and clients can use this AMQP broker to communicate with the server.
These clients can run on a different system then the server, they just need to be on the same network.
AMQP functionality is provided by the QPID framework http://qpid.apache.org 

The code is divided into modules 
Modules that are intended for shared use between server and client applications are compiled as static libraries.
To improve loose coupling of classes, I use the IoC principle (using the http://autowiring.io library)

## Status
Work in progress....

## Build instructions
This project uses CMake  as it's build system and Google Test for unit testing.
This project is being developed on Linux (Ubuntu) and has not(yet) been tested on other platforms.
When building and installing jevonhome, the default install location is '/opt/jevonhome'.
This can be changed in the main CmakeLists.txt to any other location.

1. Install Cmake (version > 3.0) and all other build tools

2. run downloadexternal.sh to download external libraries that are required to build jevonhome

3. run build.sh to build and install
