/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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
**
** This file was generated by dbusxml2cpp version 0.6
** Command line was: dbusxml2cpp -a chat_adaptor.h: com.trolltech.chat.xml
**
** dbusxml2cpp is Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
**
** This is an auto-generated file.
** This file may have been hand-edited. Look for HAND-EDIT comments
** before re-generating it.
**
****************************************************************************/

#ifndef CHAT_ADAPTOR_H_142741156243605
#define CHAT_ADAPTOR_H_142741156243605

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

QT_BEGIN_NAMESPACE
class QByteArray;
template<class T> class QList;
template<class Key, class Value> class QMap;
class QString;
class QStringList;
class QVariant;
QT_END_NAMESPACE

/*
 * Adaptor class for interface com.trolltech.chat
 */
class ChatAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.trolltech.chat")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"com.trolltech.chat\" >\n"
"    <signal name=\"message\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"nickname\" />\n"
"      <arg direction=\"out\" type=\"s\" name=\"text\" />\n"
"    </signal>\n"
"    <signal name=\"action\" >\n"
"      <arg direction=\"out\" type=\"s\" name=\"nickname\" />\n"
"      <arg direction=\"out\" type=\"s\" name=\"text\" />\n"
"    </signal>\n"
"  </interface>\n"
        "")
public:
    ChatAdaptor(QObject *parent);
    virtual ~ChatAdaptor();

public: // PROPERTIES
public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void action(const QString &nickname, const QString &text);
    void message(const QString &nickname, const QString &text);
};

#endif
