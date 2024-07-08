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

#include "../../simple_logger.hpp"

// Don't compile asserts away
#ifdef NDEBUG
#undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace juzzlin::StreamTest {

void assertString(std::stringstream & stream, const std::string & message)
{
    if (stream.str().find(message) == std::string::npos) {
        throw std::runtime_error("ERROR!!: '" + message + "' not found in '" + stream.str() + "'");
    }
}

void assertMessage(std::stringstream & stream, const std::string & message, const std::string & timestampSeparator)
{
    assertString(stream, message);
    assertString(stream, timestampSeparator);
}

void assertNotString(std::stringstream & stream, const std::string & message)
{
    if (stream.str().find(message) != std::string::npos) {
        throw std::runtime_error("ERROR!!: '" + message + "' was found in '" + stream.str() + "'");
    }
}

void assertNotMessage(std::stringstream & stream, const std::string & message, const std::string & timestampSeparator)
{
    assertNotString(stream, message);
    assertNotString(stream, timestampSeparator);
}

void testFatal_noneLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::None);
    std::stringstream ss;
    L::setStream(L::Level::Fatal, ss);
    L().fatal() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testFatal_fatalLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Fatal, ss);
    L::setLoggingLevel(L::Level::Fatal);
    L().fatal() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testError_higherLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::Fatal);
    std::stringstream ss;
    L::setStream(L::Level::Error, ss);
    L().error() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testError_errorLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Error, ss);
    L::setLoggingLevel(L::Level::Error);
    L().error() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testWarning_higherLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::Error);
    std::stringstream ss;
    L::setStream(L::Level::Warning, ss);
    L().warning() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testWarning_warningLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Warning, ss);
    L::setLoggingLevel(L::Level::Warning);
    L().warning() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testInfo_higherLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::Warning);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testInfo_infoLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L::setLoggingLevel(L::Level::Info);
    L().info() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testDebug_higherLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::Info);
    std::stringstream ss;
    L::setStream(L::Level::Debug, ss);
    L().debug() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testDebug_debugLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Debug, ss);
    L::setLoggingLevel(L::Level::Debug);
    L().debug() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testTrace_higherLoggingLevel_shouldNotPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    L::setLoggingLevel(L::Level::Debug);
    std::stringstream ss;
    L::setStream(L::Level::Trace, ss);
    L().trace() << message;
    assertNotMessage(ss, message, timestampSeparator);
}

void testTrace_traceLoggingLevel_shouldPrintMessage(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Trace, ss);
    L::setLoggingLevel(L::Level::Trace);
    L().trace() << message;
    assertMessage(ss, message, timestampSeparator);
}

void testTag_fatalLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Fatal, ss);
    L::setLoggingLevel(L::Level::Fatal);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void testTag_errorLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Error, ss);
    L::setLoggingLevel(L::Level::Error);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void testTag_warningLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Warning, ss);
    L::setLoggingLevel(L::Level::Warning);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void testTag_infoLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L::setLoggingLevel(L::Level::Info);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void testTag_debugLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Debug, ss);
    L::setLoggingLevel(L::Level::Debug);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void testTag_traceLevel_shouldPrintTag(const std::string & message, const std::string & timestampSeparator)
{
    std::stringstream ss;
    L::setStream(L::Level::Trace, ss);
    L::setLoggingLevel(L::Level::Trace);
    const std::string tag = "TAG";
    L(tag).fatal() << message;
    assertMessage(ss, tag + ": " + message, timestampSeparator);
}

void initializeLogger(const std::string & timestampSeparator)
{
    L::enableEchoMode(true);
    L::setTimestampMode(L::TimestampMode::DateTime);
    L::setTimestampSeparator(timestampSeparator);
}

void testTimestampMode_none_shouldNotPrintTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::None);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L::setLoggingLevel(L::Level::Info);
    L().info() << message;
    assertString(ss, message);
    assertNotString(ss, "##");
}

void testTimestampMode_dateTime_shouldPrintDateTimeTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::DateTime);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;

    assert(ss.str().find(message) != std::string::npos);

    // Example of expected format: "Mon Jan 1 00:00:00 2021"
    // Regex to match date-time format (handling single-digit day with optional space)
    std::regex dateTimeRegex(R"(\w{3} \w{3} \s?\d{1,2} \d{2}:\d{2}:\d{2} \d{4} ##)");
    assert(std::regex_search(ss.str(), dateTimeRegex));
}

void testTimestampMode_epochSeconds_shouldPrintEpochSecondsTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::EpochSeconds);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assert(ss.str().find(message) != std::string::npos);

    // Regex to match epoch seconds format
    const std::regex epochSecondsRegex(R"(\d{10} ##)");
    assert(std::regex_search(ss.str(), epochSecondsRegex));
}

void testTimestampMode_epochMilliseconds_shouldPrintEpochMillisecondsTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::EpochMilliseconds);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assert(ss.str().find(message) != std::string::npos);

    // Regex to match epoch milliseconds format
    const std::regex epochMillisecondsRegex(R"(\d{13} ##)");
    assert(std::regex_search(ss.str(), epochMillisecondsRegex));
}

void testTimestampMode_epochMicroseconds_shouldPrintEpochMicrosecondsTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::EpochMicroseconds);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assert(ss.str().find(message) != std::string::npos);

    // Regex to match epoch microseconds format
    const std::regex epochMicrosecondsRegex(R"(\d{16} ##)");
    assert(std::regex_search(ss.str(), epochMicrosecondsRegex));
}

void testTimestampMode_ISODateTime_shouldPrintISODateTimeTimestamp(const std::string & message)
{
    L::setTimestampMode(L::TimestampMode::ISODateTime);
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assert(ss.str().find(message) != std::string::npos);

    // Regex to match ISO 8601 date-time format
    const std::regex isoDateTimeRegex(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:.\d+)?Z? ##)");
    assert(std::regex_search(ss.str(), isoDateTimeRegex));
}

void testTimestampMode_Custom_shouldPrintCustomTimestamp(const std::string & message)
{
    L::setCustomTimestampFormat("%H:%M:%S_%Y-%m-%d");
    std::stringstream ss;
    L::setStream(L::Level::Info, ss);
    L().info() << message;
    assert(ss.str().find(message) != std::string::npos);

    // Regex to match custom format
    const std::regex isoDateTimeRegex(R"(\d{2}:\d{2}:\d{2}_\d{4}-\d{2}-\d{2} ##)");
    assert(std::regex_search(ss.str(), isoDateTimeRegex));
}

void runTests()
{
    const std::string message = "Hello world!";
    const std::string timestampSeparator = " ## ";

    initializeLogger(timestampSeparator);

    testFatal_noneLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testFatal_fatalLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testError_higherLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testError_errorLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testWarning_higherLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testWarning_warningLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testInfo_higherLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testInfo_infoLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testDebug_higherLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testDebug_debugLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testTrace_higherLoggingLevel_shouldNotPrintMessage(message, timestampSeparator);

    testTrace_traceLoggingLevel_shouldPrintMessage(message, timestampSeparator);

    testTag_fatalLevel_shouldPrintTag(message, timestampSeparator);

    testTag_errorLevel_shouldPrintTag(message, timestampSeparator);

    testTag_warningLevel_shouldPrintTag(message, timestampSeparator);

    testTag_infoLevel_shouldPrintTag(message, timestampSeparator);

    testTag_debugLevel_shouldPrintTag(message, timestampSeparator);

    testTag_traceLevel_shouldPrintTag(message, timestampSeparator);

    testTimestampMode_none_shouldNotPrintTimestamp(message);

    testTimestampMode_dateTime_shouldPrintDateTimeTimestamp(message);

    testTimestampMode_epochSeconds_shouldPrintEpochSecondsTimestamp(message);

    testTimestampMode_epochMilliseconds_shouldPrintEpochMillisecondsTimestamp(message);

    testTimestampMode_epochMicroseconds_shouldPrintEpochMicrosecondsTimestamp(message);

    testTimestampMode_ISODateTime_shouldPrintISODateTimeTimestamp(message);

    testTimestampMode_Custom_shouldPrintCustomTimestamp(message);
}

} // namespace juzzlin::StreamTest

int main()
{
    juzzlin::StreamTest::runTests();

    return EXIT_SUCCESS;
}
