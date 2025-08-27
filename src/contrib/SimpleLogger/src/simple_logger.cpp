// MIT License
//
// Copyright (c) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#include <array>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>

namespace juzzlin {

class SimpleLogger::Impl
{
public:
    Impl();

    Impl(const std::string & tag);

    ~Impl();

    std::ostringstream & traceStream();

    std::ostringstream & debugStream();

    std::ostringstream & infoStream();

    std::ostringstream & warningStream();

    std::ostringstream & errorStream();

    std::ostringstream & fatalStream();

    static void enableEchoMode(bool enable);

    static void setLevelSymbol(SimpleLogger::Level level, std::string symbol);

    static void setLoggingLevel(SimpleLogger::Level level);

    static void setCustomTimestampFormat(std::string format);

    static void setTimestampMode(SimpleLogger::TimestampMode timestampMode);

    static void setTimestampSeparator(std::string separator);

    static void setStream(Level level, std::ostream & stream);

    static void initialize(std::string filename, bool append);

    void flush();

    std::ostringstream & prepareStreamForLoggingLevel(SimpleLogger::Level level);

private:
    std::string currentDateTime(std::chrono::time_point<std::chrono::system_clock> now, const std::string & dateTimeFormat) const;

    void flushFileIfOpen();

    void flushEchoIfEnabled();

    void prefixWithLevelAndTag(SimpleLogger::Level level);

    void prefixWithTimestamp();

    bool shouldFlush() const;

    static bool m_echoMode;

    static SimpleLogger::Level m_level;

    static SimpleLogger::TimestampMode m_timestampMode;

    static std::string m_timestampSeparator;

    static std::string m_customTimestampFormat;

    static std::ofstream m_fileStream;

    using SymbolMap = std::map<SimpleLogger::Level, std::string>;
    static SymbolMap m_symbols;

    using StreamMap = std::map<SimpleLogger::Level, std::ostream *>;
    static StreamMap m_streams;

    static std::recursive_mutex m_mutex;

    SimpleLogger::Level m_activeLevel = SimpleLogger::Level::Info;

    std::lock_guard<std::recursive_mutex> m_lock;

    std::string m_tag;

    std::ostringstream m_message;
};

bool SimpleLogger::Impl::m_echoMode = true;

SimpleLogger::Level SimpleLogger::Impl::m_level = SimpleLogger::Level::Info;

SimpleLogger::TimestampMode SimpleLogger::Impl::m_timestampMode = SimpleLogger::TimestampMode::DateTime;

std::string SimpleLogger::Impl::m_timestampSeparator = ": ";

std::string SimpleLogger::Impl::m_customTimestampFormat;

std::ofstream SimpleLogger::Impl::m_fileStream;

// Default level symbols
SimpleLogger::Impl::SymbolMap SimpleLogger::Impl::m_symbols = {
    { SimpleLogger::Level::Trace, "T:" },
    { SimpleLogger::Level::Debug, "D:" },
    { SimpleLogger::Level::Info, "I:" },
    { SimpleLogger::Level::Warning, "W:" },
    { SimpleLogger::Level::Error, "E:" },
    { SimpleLogger::Level::Fatal, "F:" }
};

// Default streams
SimpleLogger::Impl::StreamMap SimpleLogger::Impl::m_streams = {
    { SimpleLogger::Level::Trace, &std::cout },
    { SimpleLogger::Level::Debug, &std::cout },
    { SimpleLogger::Level::Info, &std::cout },
    { SimpleLogger::Level::Warning, &std::cerr },
    { SimpleLogger::Level::Error, &std::cerr },
    { SimpleLogger::Level::Fatal, &std::cerr }
};

std::recursive_mutex SimpleLogger::Impl::m_mutex;

SimpleLogger::Impl::Impl()
  : m_lock(m_mutex)
{
}

SimpleLogger::Impl::Impl(const std::string & tag)
  : m_lock(m_mutex)
  , m_tag(tag)
{
}

SimpleLogger::Impl::~Impl()
{
    flush();
}

void SimpleLogger::Impl::enableEchoMode(bool enable)
{
    m_echoMode = enable;
}

std::ostringstream & SimpleLogger::Impl::prepareStreamForLoggingLevel(SimpleLogger::Level level)
{
    m_activeLevel = level;
    prefixWithTimestamp();
    prefixWithLevelAndTag(level);
    return m_message;
}

void SimpleLogger::Impl::setLevelSymbol(Level level, std::string symbol)
{
    m_symbols[level] = symbol;
}

void SimpleLogger::Impl::setLoggingLevel(SimpleLogger::Level level)
{
    m_level = level;
}

void SimpleLogger::Impl::setCustomTimestampFormat(std::string customTimestampFormat)
{
    m_customTimestampFormat = customTimestampFormat;
}

void SimpleLogger::Impl::setTimestampMode(TimestampMode timestampMode)
{
    m_timestampMode = timestampMode;
}

void SimpleLogger::Impl::setTimestampSeparator(std::string separator)
{
    m_timestampSeparator = separator;
}

std::string SimpleLogger::Impl::currentDateTime(std::chrono::time_point<std::chrono::system_clock> now, const std::string & dateTimeFormat) const
{
    std::ostringstream oss;
    const auto rawTime = std::chrono::system_clock::to_time_t(now);
    oss << std::put_time(std::localtime(&rawTime), dateTimeFormat.c_str());

    return oss.str();
}

void SimpleLogger::Impl::prefixWithLevelAndTag(SimpleLogger::Level level)
{
    m_message << m_symbols[level] << (!m_tag.empty() ? " " + m_tag + ":" : "") << " ";
}

static std::string isoDateTimeMilliseconds()
{
    using std::chrono::duration_cast;
    using std::chrono::system_clock;

    const auto now = system_clock::now();
    const auto nowMs = duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; // Milliseconds part
    const auto timeTNow = system_clock::to_time_t(now); // Convert to time_t for strftime

    // Format the datetime part
    std::array<char, 64> datetimeBuffer;
    std::strftime(datetimeBuffer.data(), datetimeBuffer.size(), "%Y-%m-%dT%H:%M:%S", std::localtime(&timeTNow));

    // Append milliseconds
    std::ostringstream oss;
    oss << datetimeBuffer.data() << '.' << std::setfill('0') << std::setw(3) << nowMs.count();

    return oss.str();
}

void SimpleLogger::Impl::prefixWithTimestamp()
{
    std::string timestamp;

    using std::chrono::duration_cast;
    using std::chrono::system_clock;

    switch (m_timestampMode) {
    case SimpleLogger::TimestampMode::None:
        break;
    case SimpleLogger::TimestampMode::DateTime: {
        timestamp = currentDateTime(system_clock::now(), "%a %b %e %H:%M:%S %Y");
    } break;
    case SimpleLogger::TimestampMode::ISODateTime: {
        timestamp = currentDateTime(system_clock::now(), "%Y-%m-%dT%H:%M:%S");
    } break;
    case SimpleLogger::TimestampMode::ISODateTimeMilliseconds: {
        timestamp = isoDateTimeMilliseconds();
    } break;
    case SimpleLogger::TimestampMode::EpochSeconds:
        timestamp = std::to_string(duration_cast<std::chrono::seconds>(system_clock::now().time_since_epoch()).count());
        break;
    case SimpleLogger::TimestampMode::EpochMilliseconds:
        using std::chrono::duration_cast;
        timestamp = std::to_string(duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count());
        break;
    case SimpleLogger::TimestampMode::EpochMicroseconds:
        using std::chrono::duration_cast;
        timestamp = std::to_string(duration_cast<std::chrono::microseconds>(system_clock::now().time_since_epoch()).count());
        break;
    case SimpleLogger::TimestampMode::Custom:
        timestamp = currentDateTime(system_clock::now(), m_customTimestampFormat);
        break;
    }

    if (!timestamp.empty()) {
        m_message << timestamp << m_timestampSeparator;
    }
}

bool SimpleLogger::Impl::shouldFlush() const
{
    return m_activeLevel >= m_level && !m_message.str().empty();
}

void SimpleLogger::Impl::flushFileIfOpen()
{
    if (m_fileStream.is_open()) {
        m_fileStream << m_message.str() << std::endl;
        m_fileStream.flush();
    }
}

void SimpleLogger::Impl::flushEchoIfEnabled()
{
    if (m_echoMode) {
        if (auto && stream = m_streams[m_activeLevel]; stream) {
            *stream << m_message.str() << std::endl;
            stream->flush();
        }
    }
}

void SimpleLogger::Impl::flush()
{
    if (shouldFlush()) {
        flushFileIfOpen();
        flushEchoIfEnabled();
    }
}

void SimpleLogger::Impl::initialize(std::string filename, bool append)
{
    if (!filename.empty()) {
        m_fileStream.open(filename, append ? std::ofstream::out | std::ofstream::app : std::ofstream::out);
        if (!m_fileStream.is_open()) {
            throw std::runtime_error("ERROR!!: Couldn't open '" + filename + "' for write.\n");
        }
    }
}

std::ostringstream & SimpleLogger::Impl::traceStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Trace);
}

std::ostringstream & SimpleLogger::Impl::debugStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Debug);
}

std::ostringstream & SimpleLogger::Impl::infoStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Info);
}

std::ostringstream & SimpleLogger::Impl::warningStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Warning);
}

std::ostringstream & SimpleLogger::Impl::errorStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Error);
}

std::ostringstream & SimpleLogger::Impl::fatalStream()
{
    return prepareStreamForLoggingLevel(SimpleLogger::Level::Fatal);
}

void SimpleLogger::Impl::setStream(Level level, std::ostream & stream)
{
    m_streams[level] = &stream;
}

SimpleLogger::SimpleLogger()
  : m_impl(std::make_unique<SimpleLogger::Impl>())
{
}

SimpleLogger::SimpleLogger(const std::string & tag)
  : m_impl(std::make_unique<SimpleLogger::Impl>(tag))
{
}

void SimpleLogger::initialize(std::string filename, bool append)
{
    Impl::initialize(filename, append);
}

void SimpleLogger::enableEchoMode(bool enable)
{
    Impl::enableEchoMode(enable);
}

void SimpleLogger::setLoggingLevel(Level level)
{
    Impl::setLoggingLevel(level);
}

void SimpleLogger::setLevelSymbol(Level level, std::string symbol)
{
    Impl::setLevelSymbol(level, symbol);
}

void SimpleLogger::setTimestampMode(TimestampMode timestampMode)
{
    Impl::setTimestampMode(timestampMode);
}

void SimpleLogger::setCustomTimestampFormat(std::string customTimestampFormat)
{
    Impl::setTimestampMode(TimestampMode::Custom);
    Impl::setCustomTimestampFormat(customTimestampFormat);
}

void SimpleLogger::setTimestampSeparator(std::string timestampSeparator)
{
    Impl::setTimestampSeparator(timestampSeparator);
}

void SimpleLogger::setStream(Level level, std::ostream & stream)
{
    Impl::setStream(level, stream);
}

std::ostringstream & SimpleLogger::trace()
{
    return m_impl->traceStream();
}

std::ostringstream & SimpleLogger::debug()
{
    return m_impl->debugStream();
}

std::ostringstream & SimpleLogger::info()
{
    return m_impl->infoStream();
}

std::ostringstream & SimpleLogger::warning()
{
    return m_impl->warningStream();
}

std::ostringstream & SimpleLogger::error()
{
    return m_impl->errorStream();
}

std::ostringstream & SimpleLogger::fatal()
{
    return m_impl->fatalStream();
}

std::string SimpleLogger::version()
{
    return "2.0.0";
}

SimpleLogger::~SimpleLogger() = default;

} // juzzlin
