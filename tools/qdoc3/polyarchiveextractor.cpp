/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  polyarchiveextractor.cpp
*/

#include "command.h"
#include "polyarchiveextractor.h"

QT_BEGIN_NAMESPACE

/*!
  \class PolyArchiveExtractor
  
  \brief The PolyArchiveExtractor class is a class for unpacking
  archive files.

  This subclass of ArchiveExtractor contains a parameterized
  command for doing the archive extraction.

  It has an extractArchive() function you call to do the
  actual archive extraction.
 */

/*!
  The constructor takes the list of filename \a extensions,
  which it passes to the base class, and the \a commandFormat,
  which it stores locally. The \a commandFormat is a command
  template string.
 */
PolyArchiveExtractor::PolyArchiveExtractor( const QStringList& extensions,
					    const QString& commandFormat )
    : ArchiveExtractor( extensions ), cmd( commandFormat )
{
}

/*!
  The destructor doesn't have to do anything.
 */
PolyArchiveExtractor::~PolyArchiveExtractor()
{
}

/*!
  Call this function to do the actual archive extraction. It calls
  the executeCommand() function to do the work. That's all it does.
 */
void PolyArchiveExtractor::extractArchive( const Location& location,
					   const QString& filePath,
					   const QString& outputDir )
{
    executeCommand( location, cmd, QStringList() << filePath << outputDir );
}

QT_END_NAMESPACE
