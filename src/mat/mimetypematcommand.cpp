/*
 * libqtxdg - An Qt implementation of freedesktop.org xdg specs
 * Copyright (C) 2019  Luís Pereira <luis.artur.pereira@gmail.com>
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
#include "mimetypematcommand.h"
#include "matglobals.h"

#include "xdgdesktopfile.h"
#include "xdgmimeapps.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStringList>
#include <QtGlobal>
#include <QUrl>

#include <iostream>

using namespace Qt::Literals::StringLiterals;

MimeTypeMatCommand::MimeTypeMatCommand(QCommandLineParser *parser)
    : MatCommandInterface(u"mimetype"_s,
                          u"Determines a file (mime)type"_s,
                          parser)
{
}

MimeTypeMatCommand::~MimeTypeMatCommand() = default;

static CommandLineParseResult parseCommandLine(QCommandLineParser *parser, QString *file, QString *errorMessage)
{
    parser->clearPositionalArguments();
    parser->setApplicationDescription(u"Determines a file (mime)type"_s);

    parser->addPositionalArgument(u"mimetype"_s, u"file | URL"_s,
                                  QCoreApplication::tr("[file | URL]"));

    const QCommandLineOption helpOption = parser->addHelpOption();
    const QCommandLineOption versionOption = parser->addVersionOption();

    if (!parser->parse(QCoreApplication::arguments())) {
        *errorMessage = parser->errorText();
        return CommandLineError;
    }

    if (parser->isSet(versionOption)) {
        return CommandLineVersionRequested;
    }

    if (parser->isSet(helpOption) || parser->isSet(u"help-all"_s)) {
        return CommandLineHelpRequested;
    }

    QStringList fs = parser->positionalArguments();
    if (fs.size() < 2) {
        *errorMessage = u"No file given"_s;
        return CommandLineError;
    }

    fs.removeAt(0);

    if (fs.size() > 1) {
        *errorMessage = u"Only one file, please"_s;
        return CommandLineError;
    }
    *file = fs.at(0);

    return CommandLineOk;
}

int MimeTypeMatCommand::run(const QStringList &arguments)
{
    Q_UNUSED(arguments);

    QString errorMessage;
    QString file;

    switch(parseCommandLine(parser(), &file, &errorMessage)) {
    case CommandLineOk:
        break;
    case CommandLineError:
        std::cerr << qPrintable(errorMessage);
        std::cerr << "\n\n";
        std::cerr << qPrintable(parser()->helpText());
        return EXIT_FAILURE;
    case CommandLineVersionRequested:
        parser()->showVersion();
        Q_UNREACHABLE();
    case CommandLineHelpRequested:
        parser()->showHelp();
        Q_UNREACHABLE();
    }

    bool isLocalFile = false;
    QString localFilename;
    const QUrl url(file);
    const QString scheme = url.scheme();
    if (scheme.isEmpty()) {
        isLocalFile = true;
        localFilename = file;
    } else if (scheme == "file"_L1) {
        isLocalFile = true;
        localFilename = QUrl(file).toLocalFile();
    }

    if (isLocalFile) {
        const QFileInfo info(file);
        if (!info.exists(localFilename)) {
            std::cerr << qPrintable(u"Cannot access '%1': No such file or directory\n"_s.arg(file));
            return EXIT_FAILURE;
        } else {
            QMimeDatabase mimeDb;
            const QMimeType mimeType = mimeDb.mimeTypeForFile(info, QMimeDatabase::MatchExtension);
            std::cout << qPrintable(mimeType.name()) << "\n";
            return EXIT_SUCCESS;
        }
    } else { // not a local file
        std::cerr << qPrintable(u"Can't handle '%1': '%2' scheme not supported\n"_s.arg(file, scheme));
        return EXIT_FAILURE;
    }
}
