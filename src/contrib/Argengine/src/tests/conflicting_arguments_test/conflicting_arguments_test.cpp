// MIT License
//
// Copyright (c) 2023 Jussi Lind <jussi.lind@iki.fi>
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

#include "../../argengine.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>

using juzzlin::Argengine;

const auto name = "Argengine";

void testConflictingArguments_ConflictingArgumentsGiven_ShouldFail()
{
    Argengine ae({ "test", "foo", "bar" });
    bool fooCalled {};
    ae.addOption({ "foo" }, [&] {
        fooCalled = true;
    });
    bool barCalled {};
    ae.addOption({ "bar" }, [&] {
        barCalled = true;
    });
    ae.addConflictingOptions({ "foo", "bar" });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!fooCalled);
    assert(!barCalled);
    assert(error == std::string(name) + ": Conflicting options: 'bar', 'foo'. These options cannot coexist.");
}

void testConflictingArguments_ConflictingArgumentsSetButNotEnoughGiven_ShouldSucceed()
{
    Argengine ae({ "test", "foo" });
    bool fooCalled {};
    ae.addOption({ "foo" }, [&] {
        fooCalled = true;
    });
    ae.addConflictingOptions({ "foo", "bar" });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(fooCalled);
    assert(error.empty());
}

void testConflictingArguments_ConflictingArgumentsGiven_ShouldNotReferToVariants()
{
    Argengine ae({ "test", "foo", "bar" });
    bool fooCalled {};
    ae.addOption({ "foo", "-f" }, [&] {
        fooCalled = true;
    });
    bool barCalled {};
    ae.addOption({ "-b", "bar" }, [&] {
        barCalled = true;
    });
    ae.addConflictingOptions({ "foo", "bar" });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!fooCalled);
    assert(!barCalled);
    assert(error == std::string(name) + ": Conflicting options: 'bar', 'foo'. These options cannot coexist.");
}

int main(int, char **)
{
    testConflictingArguments_ConflictingArgumentsGiven_ShouldFail();

    testConflictingArguments_ConflictingArgumentsSetButNotEnoughGiven_ShouldSucceed();

    testConflictingArguments_ConflictingArgumentsGiven_ShouldNotReferToVariants();

    return EXIT_SUCCESS;
}
