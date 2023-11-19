// MIT License
//
// Copyright (c) 2020 Jussi Lind <jussi.lind@iki.fi>
//
// https://github.com/juzzlin/Argengine
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

#ifndef JUZZLIN_ARGENGINE_HPP
#define JUZZLIN_ARGENGINE_HPP

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace juzzlin {

class Argengine
{
public:
    //! Constructor.
    //! \param argc Argument count as in `main(int argc, char ** argv)`
    //! \param argv Argument array as in `main(int argc, char ** argv)`
    //! \param addDefaultHelp If true, a default help action for "-h" and "--help" is added.
    Argengine(int argc, char ** argv, bool addDefaultHelp = true);

    //! Constructor.
    //! \param args The arguments as a vector of strings. It is assumed, that the first element is the name of the executed application.
    //! \param addDefaultHelp If true, a default help action for "-h" and "--help" is added.
    using ArgumentVector = std::vector<std::string>;
    explicit Argengine(ArgumentVector args, bool addDefaultHelp = true);

    //! Destructor.
    ~Argengine();

    //! Adds a valueless option to the configuration.
    //! \param optionVariants A set of possible options for the given action, usually the short and long form: {"-f", "--foo"}
    //! \param callback Callback to be called when the option has been given. Signature: `void()`.
    //! \param required If true, an error will follow if the option is not present during parse().
    //! \param infoText Short info text shown in help/usage.
    using OptionVariants = std::set<std::string>;
    using ValuelessCallback = std::function<void()>;
    void addOption(OptionVariants optionVariants, ValuelessCallback callback, bool required = false, std::string infoText = "");

    //! Adds an option with a single value to the configuration.
    //! \param optionVariants A set of possible options for the given action, usually the short and long form: {"-f", "--foo"}
    //! \param callback Callback to be called when the option has been given with a value. Signature: `void(std::string)`.
    //! \param required \see addOption(OptionVariants optionVariants, SingleStringCallback callback, bool required).
    //! \param infoText Short info text shown in help/usage.
    //! \param valueName Name of the value in help.
    using SingleStringCallback = std::function<void(std::string)>;
    void addOption(OptionVariants optionVariants, SingleStringCallback callback, bool required = false, std::string infoText = "", std::string valueName = "VALUE");

    //! Special method to add custom help / decorate output of `printHelp()`. Help is always executed first if present.
    //! \param optionVariants A set of possible options for help, usually the short and long form: {"-h", "--help"}
    //! \param callback Callback to be called when the help option has been given. Signature: `void()`.
    void addHelp(OptionVariants optionVariants, ValuelessCallback callback);

    //! Adds a options that cannot coexist.
    //! \param conflictingOptions A set of possible options that cannot coexist, e.g.: {"--bar", "--foo"}
    using ConflictingOptionSet = std::set<std::string>;
    void addConflictingOptions(ConflictingOptionSet conflictingOptionSet);

    //! \return All given arguments.
    ArgumentVector arguments() const;

    //! Set info text printed on help/usage before argument help.
    //! \param helpText Text shown in help. E.g. "MyApplication v1.0.0, Copyright (c) 2020 Foo Bar".
    void setHelpText(std::string helpText);

    //! \return The current help text.
    std::string helpText() const;

    //! Sorting order of arguments in help.
    enum class HelpSorting
    {
        None,
        Ascending
    };

    //! Set sorting style of arguments in help.
    //! \param helpSorting The sorting direction enum. Default is HelpSorting::None.
    void setHelpSorting(HelpSorting helpSorting);

    //! Set handler for positional arguments.
    using StringValueVector = std::vector<std::string>;
    using MultiStringCallback = std::function<void(StringValueVector)>;
    void setPositionalArgumentCallback(MultiStringCallback callback);

    //! Set custom output stream. Default is std::cout.
    //! \param out The new output stream.
    void setOutputStream(std::ostream & out);

    //! Parses by using the current config. Throws `std::runtime_error` on error.
    void parse();

    //! Error structure set by parse().
    struct Error
    {
        enum class Code
        {
            Ok,
            Failed
        };

        Code code = Code::Ok;

        std::string message;
    };

    //! Parses by using the current config.
    //! \param error Contains error info error.
    void parse(Error & error);

    //! Prints help/usage.
    void printHelp();

    //! \return Library version in x.y.z
    static std::string version();

private:
    Argengine(const Argengine & r) = delete;
    Argengine & operator=(const Argengine & r) = delete;

    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // juzzlin

#endif // JUZZLIN_ARGENGINE_HPP
