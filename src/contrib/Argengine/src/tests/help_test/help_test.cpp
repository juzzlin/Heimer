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

#include "argengine.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <sstream>

using juzzlin::Argengine;

const std::string name = "Argengine";

void testDefaultHelpOverride_HelpActive_ShouldFail()
{
    Argengine ae({ "test" });
    std::string error;
    try {
        ae.addOption({ "-h" }, [&] {
        });
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(error == name + ": Option '-h, --help' already defined!");
}

void testDefaultHelpOverride_HelpNotActive_ShouldSucceed()
{
    Argengine ae({ "test" }, false);
    std::string error;
    try {
        ae.addOption({ "-h" }, [&] {
        });
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(error == "");
}

void testDefaultHelp_PrintToStream_ShouldSucceed()
{
    Argengine ae({ "test" });
    std::stringstream ss;
    ae.setOutputStream(ss);
    ae.printHelp();
    const std::string answer = "Usage: " + ae.arguments().at(0) + " [OPTIONS]\n\nOptions:\n\n" + //
      "-h, --help  Show this help.\n\n";
    assert(ss.str() == answer);
}

void testDefaultHelp_ClearHelpText_ShouldSucceed()
{
    Argengine ae({ "test" });
    std::stringstream ss;
    ae.setOutputStream(ss);
    ae.setHelpText("");
    ae.printHelp();
    const std::string answer = "Options:\n\n-h, --help  Show this help.\n\n";
    assert(ss.str() == answer);
}

int main(int, char **)
{
    testDefaultHelpOverride_HelpActive_ShouldFail();

    testDefaultHelpOverride_HelpNotActive_ShouldSucceed();

    testDefaultHelp_PrintToStream_ShouldSucceed();

    testDefaultHelp_ClearHelpText_ShouldSucceed();

    return EXIT_SUCCESS;
}
