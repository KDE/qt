/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QWSServer>

#include "calibration.h"
#include "scribblewidget.h"

//! [0]
int main(int argc, char **argv)
{
    QApplication app(argc, argv, QApplication::GuiServer);

    if (!QWSServer::mouseHandler())
        qFatal("No mouse handler installed");

    {
        QMessageBox message;
        message.setText("<p>Please press once at each of the marks "
                        "shown in the next screen.</p>"
                        "<p>This messagebox will timout after 10 seconds "
                        "if you are unable to close it.</p>");
        QTimer::singleShot(10 * 1000, &message, SLOT(accept()));
        message.exec();
    }

//! [0] //! [1]
    Calibration cal;
    cal.exec();
//! [1]

//! [2]
    {
        QMessageBox message;
        message.setText("<p>The next screen will let you test the calibration "
                        "by drawing into a widget.</p><p>This program will "
                        "automatically close after 20 seconds.<p>");
        QTimer::singleShot(10 * 1000, &message, SLOT(accept()));
        message.exec();
    }

    ScribbleWidget scribble;
    scribble.showMaximized();
    scribble.show();

    app.setActiveWindow(&scribble);
    QTimer::singleShot(20 * 1000, &app, SLOT(quit()));

    return app.exec();
}
//! [2]

