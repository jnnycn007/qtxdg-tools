/*
 * libqtxdg - An Qt implementation of freedesktop.org xdg specs
 * Copyright (C) 2018  Luís Pereira <luis.artur.pereira@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include "matcommandinterface.h"

#include <QCommandLineParser>

MatCommandInterface::MatCommandInterface(const QString &name, const QString &description, QCommandLineParser *parser)
    : mName(name),
      mDescription(description),
      mParser(parser)
{
    Q_CHECK_PTR(parser);
}

MatCommandInterface::~MatCommandInterface() = default;

void MatCommandInterface::showHelp(int exitCode) const
{
    if (!mParser)
        return;

    mParser->showHelp(exitCode);
}

void MatCommandInterface::showVersion() const
{
    if (!mParser)
        return;

    mParser->showVersion();
}
