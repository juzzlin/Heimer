// This file is part of Heimer.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Heimer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Heimer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Heimer. If not, see <http://www.gnu.org/licenses/>.

#ifndef MINDMAPDATABASE_HPP
#define MINDMAPDATABASE_HPP

#include <QString>

class Graph;

//! Common base class for track data shared by the editor and the game.
class MindMapDataBase
{
public:
    /** Keywords used in the track data files.
     *  TODO: Use constexpr when MSVC supports it properly. */
    struct DataKeywords
    {
        struct Header
        {
            static QString ver()
            {
                return "version";
            }

            static QString name()
            {
                return "name";
            }
        };

        struct Node
        {
            static QString index()
            {
                return "i";
            }

            static QString x()
            {
                return "x";
            }

            static QString y()
            {
                return "y";
            }

            static QString width()
            {
                return "w";
            }

            static QString height()
            {
                return "h";
            }
        };
    };

    //! Constructor.
    MindMapDataBase(QString name);

    //! Copy constructor.
    MindMapDataBase(const MindMapDataBase & other);

    MindMapDataBase & operator=(const MindMapDataBase & other) = delete;

    //! Destructor.
    virtual ~MindMapDataBase();

    virtual QString name() const;

    virtual void setName(QString name);

    virtual QString fileName() const = 0;

    virtual void setFileName(QString fileName) = 0;

    virtual unsigned int index() const;

    virtual void setIndex(unsigned int index);

    virtual Graph & graph() = 0;

    virtual const Graph & graph() const = 0;

private:
    QString m_name;

    unsigned int m_index;
};

#endif // MINDMAPDATABASE_HPP
