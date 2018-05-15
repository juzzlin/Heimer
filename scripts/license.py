#!/usr/bin/python3

# This file is part of Heimer.
# Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
# 
# DustRAC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# DustRAC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

# Usage:
#
# This script replaces the license header of the given file(s) with
# the given new license header. License lines MUST begin with "//".
#
# e.g. ./license.py gpl_header.txt `find . -name "*.hh"`

import sys

def main(argv):

    if len(argv) < 3:
        print("Usage: ./license.py [licenseHeaderFile] [sourceFiles]")
        return 1

    # Read the new license
    licenseFileName = argv[1]
    print("Opening {0} for read..".format(licenseFileName))
    with open(licenseFileName, 'r') as licenseFile:
        licenseLines = licenseFile.readlines()

    for j in range(2, len(argv)):
        inputFileName = argv[j]

        # Read the original file
        print("Opening {0} for read..".format(inputFileName))
        with open(inputFileName, 'r') as inputFile:
            origLines = inputFile.readlines()

        # Skip the current license header
        headerSkip = True
        newLines = []
        for line in origLines:
            if line.find('//') == -1 or line.find('\n') == 0 or not headerSkip:
                headerSkip = False
                newLines.append(line)

        # Add the new license
        newLines = licenseLines + newLines 

        # Modify the source file
        print("Opening {0} for write..".format(inputFileName))
        with open(inputFileName, 'w') as inputFile:
            inputFile.writelines(newLines)

    print("Done.")
    return 0

if __name__ == '__main__':
    main(sys.argv)

