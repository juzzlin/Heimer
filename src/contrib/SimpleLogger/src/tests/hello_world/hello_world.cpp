// MIT License
//
// Copyright (c) 2019 Jussi Lind <jussi.lind@iki.fi>
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
#include <fstream>

int main(int, char **)
{
    using juzzlin::L;

    const auto logFile = "hello_world.log";

    L::init(logFile);
    L::enableEchoMode(true);
    L::setLoggingLevel(L::Level::Trace);

    const std::string message = "Hello, world!";
    const std::string timestampSeparator = " ## ";
    L::setTimestampMode(L::TimestampMode::DateTime, timestampSeparator);

    L().trace() << message;
    L().debug() << message;
    L().info() << message;
    L().warning() << message;
    L().error() << message;
    L().fatal() << message;

    std::ifstream fin{logFile};
    assert(fin.is_open());

    int lines = 0;
    std::string line;
    while (std::getline(fin, line))
    {
        assert(line.find(message) != std::string::npos);
        assert(line.find(timestampSeparator) != std::string::npos);
        lines++;
    }

    assert(lines == 6);

    return EXIT_SUCCESS;
}
