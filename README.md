
# Embedded C++ SuperLoop Scheduler

A lightweight task scheduling framework implemented in C++ for embedded software concepts and Linux-based simulation.

## Overview

This project implements a SuperLoop-style scheduler designed to organize and execute periodic tasks.

The project explores task registration, scheduling logic, task status management, and timing-related behavior using C++.

## Features

- Task registration with configurable execution periods
- Task status management
- Periodic task scheduling
- Configurable timing settings
- Modular C++ implementation
- Linux-hosted development and testing
- Missed execution tracking
- Task error handling and reporting
- CRC calculation and data validation
- Separate IRQ and scheduler threads
- Task capacity management

## Project Structure

```text
src/
├── main.cpp
├── SuperLoop.cpp
├── SuperLoop.hpp
├── SuperLoopCommon.cpp
├── SuperLoopCommon.hpp
├── SuperloopConfig.cpp
└── SuperLoopConfig.hpp
```

## Technologies

- C++
- Git and GitHub
- Eclipse IDE
- Linux/WSL development environment

## Build

Compile the project from the repository root directory:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic src/main.cpp src/SuperLoop.cpp src/SuperLoopCommon.cpp src/SuperloopConfig.cpp -pthread -o superloop
```

## Project Scope

This project is intended to explore task scheduling and SuperLoop architecture in C++.

It is a software scheduling project and should not be considered a production-ready real-time operating system.

## Run

```bash
./superloop
```

## Author

Mohammad Seyedi
