/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget *window = new QWidget();
//! [create, lay out widgets and show]
    QLabel *queryLabel = new QLabel(tr("Query:"));
    QLineEdit *queryEdit = new QLineEdit();
    QTableView *resultView = new QTableView();

    QHBoxLayout *queryLayout = new QHBoxLayout();
    queryLayout->addWidget(queryLabel);
    queryLayout->addWidget(queryEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(queryLayout);
    mainLayout->addWidget(resultView);
    window->setLayout(mainLayout);
//! [create, lay out widgets and show]

    QStandardItemModel model;
    model.setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Office"));
    QList<QStringList> rows = QList<QStringList>()
        << (QStringList() << "Verne Nilsen" << "123")
        << (QStringList() << "Carlos Tang" << "77")
        << (QStringList() << "Bronwyn Hawcroft" << "119")
        << (QStringList() << "Alessandro Hanssen" << "32")
        << (QStringList() << "Andrew John Bakken" << "54")
        << (QStringList() << "Vanessa Weatherley" << "85")
        << (QStringList() << "Rebecca Dickens" << "17")
        << (QStringList() << "David Bradley" << "42")
        << (QStringList() << "Knut Walters" << "25")
        << (QStringList() << "Andrea Jones" << "34");
    foreach (QStringList row, rows) {
        QList<QStandardItem *> items;
        foreach (QString text, row)
            items.append(new QStandardItem(text));
        model.appendRow(items);
    }

    resultView->setModel(&model);
    resultView->verticalHeader()->hide();
    resultView->horizontalHeader()->setStretchLastSection(true);
    window->setWindowTitle(tr("Nested layouts"));
    window->show();
    return app.exec();
}
