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

#include "../../argengine.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <iostream>

using juzzlin::Argengine;

void testSinglePositionalArgument_NoOtherArguments_ShouldSucceed()
{
    Argengine ae({ "test", "a" });
    Argengine::ArgumentVector ps;
    ae.setPositionalArgumentCallback([&](Argengine::ArgumentVector args) {
        ps = args;
    });
    ae.parse();
    assert(ps.at(0) == ae.arguments().at(1));
}

void testSinglePositionalArgument_OtherArgumentsBefore_ShouldSucceed()
{
    Argengine ae({ "test", "--foo", "-b", "a" });
    Argengine::ArgumentVector ps;
    ae.addOption({ "--foo" }, [] {
    });
    ae.addOption({ "-b" }, [] {
    });
    ae.setPositionalArgumentCallback([&](Argengine::ArgumentVector args) {
        ps = args;
    });
    ae.parse();
    assert(ps.at(0) == ae.arguments().at(3));
}

void testSinglePositionalArgument_OtherArgumentsAfter_ShouldSucceed()
{
    Argengine ae({ "test", "a", "--foo", "-b" });
    Argengine::ArgumentVector ps;
    ae.addOption({ "--foo" }, [] {
    });
    ae.addOption({ "-b" }, [] {
    });
    ae.setPositionalArgumentCallback([&](Argengine::ArgumentVector args) {
        ps = args;
    });
    ae.parse();
    assert(ps.at(0) == ae.arguments().at(1));
}

void testSinglePositionalArgument_OtherArgumentsAround_ShouldSucceed()
{
    Argengine ae({ "test", "--foo", "a", "-b" });
    Argengine::ArgumentVector ps;
    ae.addOption({ "--foo" }, [] {
    });
    ae.addOption({ "-b" }, [] {
    });
    ae.setPositionalArgumentCallback([&](Argengine::ArgumentVector args) {
        ps = args;
    });
    ae.parse();
    assert(ps.at(0) == ae.arguments().at(2));
}

void testMultiplePositionalArguments_NoOtherArguments_ShouldSucceed()
{
    Argengine ae({ "test", "a", "42" });
    Argengine::ArgumentVector ps;
    ae.setPositionalArgumentCallback([&](Argengine::ArgumentVector args) {
        ps = args;
    });
    ae.parse();
    assert(ps.at(0) == ae.arguments().at(1));
    assert(ps.at(1) == ae.arguments().at(2));
}

int main(int, char **)
{
    testSinglePositionalArgument_NoOtherArguments_ShouldSucceed();

    testSinglePositionalArgument_OtherArgumentsAfter_ShouldSucceed();

    testSinglePositionalArgument_OtherArgumentsAround_ShouldSucceed();

    testSinglePositionalArgument_OtherArgumentsBefore_ShouldSucceed();

    testMultiplePositionalArguments_NoOtherArguments_ShouldSucceed();

    return EXIT_SUCCESS;
}
