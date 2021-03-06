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

#include <QtGui>

#include "xbelgenerator.h"

XbelGenerator::XbelGenerator(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
}

bool XbelGenerator::write(QIODevice *device)
{
    out.setDevice(device);
    out.setCodec("UTF-8");
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<!DOCTYPE xbel>\n"
        << "<xbel version=\"1.0\">\n";

    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        generateItem(treeWidget->topLevelItem(i), 1);

    out << "</xbel>\n";
    return true;
}

QString XbelGenerator::indent(int depth)
{
    const int IndentSize = 4;
    return QString(IndentSize * depth, ' ');
}

QString XbelGenerator::escapedText(const QString &str)
{
    QString result = str;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    return result;
}

QString XbelGenerator::escapedAttribute(const QString &str)
{
    QString result = escapedText(str);
    result.replace("\"", "&quot;");
    result.prepend("\"");
    result.append("\"");
    return result;
}

void XbelGenerator::generateItem(QTreeWidgetItem *item, int depth)
{
    QString tagName = item->data(0, Qt::UserRole).toString();
    if (tagName == "folder") {
        bool folded = !treeWidget->isItemExpanded(item);
        out << indent(depth) << "<folder folded=\"" << (folded ? "yes" : "no")
                             << "\">\n"
            << indent(depth + 1) << "<title>" << escapedText(item->text(0))
                                 << "</title>\n";

        for (int i = 0; i < item->childCount(); ++i)
            generateItem(item->child(i), depth + 1);

        out << indent(depth) << "</folder>\n";
    } else if (tagName == "bookmark") {
        out << indent(depth) << "<bookmark";
        if (!item->text(1).isEmpty())
            out << " href=" << escapedAttribute(item->text(1));
        out << ">\n"
            << indent(depth + 1) << "<title>" << escapedText(item->text(0))
                                 << "</title>\n"
            << indent(depth) << "</bookmark>\n";
    } else if (tagName == "separator") {
        out << indent(depth) << "<separator/>\n";
    }
}
