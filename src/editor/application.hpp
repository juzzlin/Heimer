// This file is part of Dementia.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
//
// Dementia is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dementia is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dementia. If not, see <http://www.gnu.org/licenses/>.

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QApplication>
#include <QTranslator>

class MainWindow;

class Application
{
public:

    Application(int & argc, char ** argv);

    ~Application();

    int run();

private:

    void parseArgs(int argc, char ** argv);

    QApplication m_app;

    QTranslator m_appTranslator;

    QString m_mindMapFile;

    MainWindow * m_mainWindow;
};

#endif // APPLICATION_HPP
