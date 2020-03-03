// MIT License
//
// Copyright (c) 2020 Jussi Lind <jussi.lind@iki.fi>
//
// https://github.com/juzzlin/SimpleLogger
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "simple_logger.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <sstream>

void assertMessage(std::stringstream & stream, std::string message, std::string timestampSeparator)
{
    assert(stream.str().find(message) != std::string::npos);
    assert(stream.str().find(timestampSeparator) != std::string::npos);
}

void assertNotMessage(std::stringstream & stream, std::string message, std::string timestampSeparator)
{
    assert(stream.str().find(message) == std::string::npos);
    assert(stream.str().find(timestampSeparator) == std::string::npos);
}

int main(int, char **)
{
    using juzzlin::L;

    L::enableEchoMode(true);

    const std::string message = "Hello, world!";
    const std::string timestampSeparator = " ## ";
    L::setTimestampMode(L::TimestampMode::DateTime, timestampSeparator);

    L::setLoggingLevel(L::Level::None);
    std::stringstream ssF;
    L::setStream(L::Level::Fatal, ssF);
    L().fatal() << message;
    assertNotMessage(ssF, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Fatal);
    L().fatal() << message;
    assertMessage(ssF, message, timestampSeparator);

    std::stringstream ssE;
    L::setStream(L::Level::Error, ssE);
    L().error() << message;
    assertNotMessage(ssE, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Error);
    L().error() << message;
    assertMessage(ssE, message, timestampSeparator);

    std::stringstream ssW;
    L::setStream(L::Level::Warning, ssW);
    L().warning() << message;
    assertNotMessage(ssW, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Warning);
    L().warning() << message;
    assertMessage(ssW, message, timestampSeparator);

    std::stringstream ssI;
    L::setStream(L::Level::Info, ssI);
    L().info() << message;
    assertNotMessage(ssI, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Info);
    L().info() << message;
    assertMessage(ssI, message, timestampSeparator);

    std::stringstream ssD;
    L::setStream(L::Level::Debug, ssD);
    L().debug() << message;
    assertNotMessage(ssD, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Debug);
    L().debug() << message;
    assertMessage(ssD, message, timestampSeparator);

    std::stringstream ssT;
    L::setStream(L::Level::Trace, ssT);
    L().trace() << message;
    assertNotMessage(ssT, message, timestampSeparator);
    L::setLoggingLevel(L::Level::Trace);
    L().trace() << message;
    assertMessage(ssT, message, timestampSeparator);

    return EXIT_SUCCESS;
}
