SimpleLogger
============

Looking for a simple logger for your C++ project? `SimpleLogger` might be for you.

# Features

* Based on RAII
* Configurable level symbols
* Datetime / EPOCH timestamps
* Logging levels: `Trace`, `Debug`, `Info`, `Warning`, `Error`, `Fatal`
* Log to file and/or console
* Thread-safe
* Uses streams (<< operator)
* Very easy to use

# Installation

## Use from sources

Just add `src/simple_logger.hpp` and `src/simple_logger.cpp` to your project and start using it!

## Use as a CMake subproject

Copy contents of `SimpleLogger` under your main project (or clone as a Git submodule).

In your `CMakeLists.txt`:

```
add_subdirectory(SimpleLogger)
include_directories(SimpleLogger/src)
```

## Use as a library

`$ mkdir build && cd build`

`$ cmake ..`

`$ make`

`$ sudo make install`

# Examples

Note: instead of `Logger` class you can simply use `L`.

## Log to console

```
using juzzlin::L;

L().info() << "Something happened";
```

Outputs something like this:

`Sat Oct 13 22:38:42 2018 I: Something happened`

## Log to file and console

```
using juzzlin::L;

L::init("/tmp/myLog.txt");

L().info() << "Something happened";
```

## Log only to file

```
using juzzlin::L;

L::init("/tmp/myLog.txt");
L::enableEchoMode(false);

L().info() << "Something happened";
```

## Set logging level

```
using juzzlin::L;

L::setLoggingLevel(L::Level::Debug);

L().info() << "Something happened";
L().debug() << "A debug thing happened";
```

Outputs something like this:

`Sat Oct 13 22:38:42 2018 I: Something happened`

`Sat Oct 13 22:38:42 2018 D: A debug thing happened`

## Set custom level symbols

```
using juzzlin::L;

L::setLoggingLevel(L::Level::Debug);
L::setLevelSymbol(L::Level::Debug, "<DEBUG>");

L().debug() << "A debug thing happened";
```

Outputs something like this:

`Sat Oct 13 22:38:42 2018 <DEBUG> A debug thing happened`

## Set timestamp mode and optional custom separator

Possible modes: `None`, `EpochSeconds`, `EpochMilliseconds`, `EpochMicroseconds`, `DateTime`.

```
using juzzlin::L;

L::setTimestampMode(L::TimestampMode::EpochMilliseconds, " ## ");

L().info() << "Something happened";
```

Outputs something like this:

`1562955750677 ## I: Something happened`

# Requirements

C++11

# Licence

MIT

# Projects currently using SimpleLogger

* https://github.com/juzzlin/DustRacing2D

* https://github.com/juzzlin/Heimer

