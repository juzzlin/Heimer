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

#ifndef JUZZLIN_SIMPLE_LOGGER_HPP
#define JUZZLIN_SIMPLE_LOGGER_HPP

#include <memory>
#include <sstream>

namespace juzzlin {

/*!
 * Example initialization:
 *
 * using juzzlin::L;
 *
 * L::initialize("myLog.txt");
 *
 * Example logging:
 *
 * L().info() << "Initialization finished.";
 * L().error() << "Foo happened!";
 */
class SimpleLogger
{
public:
    enum class Level
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        None
    };

    enum class TimestampMode
    {
        None,
        DateTime,
        EpochSeconds,
        EpochMilliseconds,
        EpochMicroseconds,
        ISODateTime,
        Custom
    };

    //! Constructor.
    SimpleLogger();

    //! Constructor.
    //! \param tag Tag that will be added to the message.
    SimpleLogger(const std::string & tag);

    //! Destructor.
    ~SimpleLogger();

    /*! Initialize the logger.
     *  \param filename Log to filename. Disabled if empty.
     *  \param append The existing log will be appended if true.
     *  Throws on error. */
    static void initialize(std::string filename, bool append = false);

    //! Enable/disable echo mode.
    //! \param enable Echo everything if true. Default is false.
    static void enableEchoMode(bool enable);

    //! Set the logging level.
    //! \param level The minimum level. Default is Info.
    static void setLoggingLevel(Level level);

    //! Set custom symbol for the given logging level.
    //! \param level The level.
    //! \param symbol The symbol outputted for the messages of this level.
    static void setLevelSymbol(Level level, std::string symbol);

    //! Set/enable timestamp mode.
    //! \param timestampMode Timestamp mode enumeration.
    static void setTimestampMode(TimestampMode timestampMode);

    //! Set custom timestamp format. Sets timestamp mode to TimestampMode::Custom.
    //! \param customTimestampFormat Timestamp format e.g. "%Y-%m-%dT%H:%M:%S".
    static void setCustomTimestampFormat(std::string customTimestampFormat);

    //! Set/enable timestamp separator.
    //! \param separator Separator string outputted after timestamp.
    static void setTimestampSeparator(std::string separator);

    //! Set specific stream.
    //! \param level The level.
    //! \param stream The output stream.
    static void setStream(Level level, std::ostream & stream);

    //! \return Library version in x.y.z
    static std::string version();

    //! Get stream to the trace log message.
    std::ostringstream & trace();

    //! Get stream to the debug log message.
    std::ostringstream & debug();

    //! Get stream to the info log message.
    std::ostringstream & info();

    //! Get stream to the warning log message.
    std::ostringstream & warning();

    //! Get stream to the error log message.
    std::ostringstream & error();

    //! Get stream to the fatal log message.
    std::ostringstream & fatal();

private:
    SimpleLogger(const SimpleLogger &) = delete;
    SimpleLogger & operator=(const SimpleLogger &) = delete;

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

using L = SimpleLogger;

} // namespace juzzlin

#endif // JUZZLIN_SIMPLE_LOGGER_HPP
