#!/usr/bin/python

# This file is part of Dust Racing (DustRAC).
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
# 
# Usage:
#
# This script replaces the license plate of the given file(s) with
# the given new license plate. license lines MUST begin with "//".
#
# e.g. python license.py gpl_header.txt `find . -name "*.hh"`

import sys

def main(argv):

    if len(argv) < 3:
        print("Usage: python license.py [licenseHeaderFile] [sourceFiles]")
        return 1

    # Read the new license
    fName = argv[1]
    print("Opening " + fName + " for read..")
    f = open(fName, 'r')
    licenseLines = f.readlines()
    f.close()

    for j in xrange(2, len(argv)):
        fName = argv[j]

        # Read the original file
        print("Opening " + fName + " for read..")
        f = open(fName, 'r')
        origLines = f.readlines()
        f.close()

        # Skip the current license plate
        headerSkip = True
        newLines   = []
        for i in origLines:
            if (i.find('//') == 0 or i.find('\n') == 0) and headerSkip:
                pass
            else:
                headerSkip = False
                newLines.append(i)

        # Add the new license
        newLines = licenseLines + newLines 

        # Modify the source file
        print("Opening " + fName + " for write..")
        f = open(fName, 'w')    
        f.writelines(newLines)
        f.close()

    print("Done.")
    return 0

if __name__ == '__main__':
    main(sys.argv)

