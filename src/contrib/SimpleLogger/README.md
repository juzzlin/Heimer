SimpleLogger
============

Looking for a simple logger for your C++ project? `SimpleLogger` might be for you.

# Features

* Based on RAII
* Configurable level symbols
* Datetime, ISO Datetime, EPOCH, and custom timestamp formats
* Logging levels: `Trace`, `Debug`, `Info`, `Warning`, `Error`, `Fatal`
* Log to file and/or console
* Thread-safe
* Uses streams (<< operator)
* Very easy to use

# Installation

## Use from sources

Just add `src/simple_logger.hpp` and `src/simple_logger.cpp` to your project and start using it!

## Use as a CMake subproject (Recommended)

Copy contents of `SimpleLogger` under your main project (or clone as a Git submodule).

In your `CMakeLists.txt`:

```
add_subdirectory(SimpleLogger EXCLUDE_FROM_ALL)
include_directories(SimpleLogger/src)
```

Link to the library:

```
target_link_libraries(${YOUR_TARGET_NAME} SimpleLogger_static)
```

In your code:

```
#include "simple_logger.hpp"
```

## Use as a library

Build and install:

`$ mkdir build && cd build`

`$ cmake ..`

`$ make`

`$ sudo make install`

Link to `libSimpleLogger_static.a` or `libSimpleLogger.so`.

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

L::initialize("/tmp/myLog.txt");

L().info() << "Something happened";
```

## Log only to file

```
using juzzlin::L;

L::initialize("/tmp/myLog.txt");
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

## Log with a tag

```
using juzzlin::L;

L::setLoggingLevel(L::Level::Info);

L("MyTag").info() << "Something happened";
```

Outputs something like this:

`Sat Oct 13 22:38:42 2018 I: MyTag: Something happened`

## Set custom level symbols

```
using juzzlin::L;

L::setLoggingLevel(L::Level::Debug);
L::setLevelSymbol(L::Level::Debug, "<DEBUG>");

L().debug() << "A debug thing happened";
```

Outputs something like this:

`Sat Oct 13 22:38:42 2018 <DEBUG> A debug thing happened`

## Set timestamp mode and optional timestamp separator

Possible timestamp modes: `None`, `EpochSeconds`, `EpochMilliseconds`, `EpochMicroseconds`, `DateTime`, `ISODateTime`.

```
using juzzlin::L;

L::setTimestampMode(L::TimestampMode::EpochMilliseconds);
L::setTimestampSeparator(" ## ");

L().info() << "Something happened";
```

Outputs something like this:

`1562955750677 ## I: Something happened`

## Set custom timestamp format

By setting a custom timestamp format the timestamp mode is set to `Custom`:

```
using juzzlin::L;

L::setCustomTimestampFormat("%H:%M:%S_%Y-%m-%d");
L::setTimestampSeparator(" ## ");

L().info() << "Something happened";
```

Outputs something like this:

`12:34:58_2024-07-06 ## I: Something happened`

## Set custom output stream

```
using juzzlin::L;

std::stringstream ssI;
L::setStream(L::Level::Info, ssI);
```

# Requirements

C++17

# Licence

MIT

# Projects currently using SimpleLogger

* https://github.com/juzzlin/DustRacing2D

* https://github.com/juzzlin/Heimer

