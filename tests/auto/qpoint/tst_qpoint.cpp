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
****************************************************************************/


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qpoint.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QPoint : public QObject
{
Q_OBJECT

public:
    tst_QPoint();
    virtual ~tst_QPoint();

private slots:
    void getSetCheck();
    void division();
};

tst_QPoint::tst_QPoint()
{
}

tst_QPoint::~tst_QPoint()
{
}

// Testing get/set functions
void tst_QPoint::getSetCheck()
{
    QPoint obj1;
    // int QPoint::x()
    // void QPoint::setX(int)
    obj1.setX(0);
    QCOMPARE(0, obj1.x());
    obj1.setX(INT_MIN);
    QCOMPARE(INT_MIN, obj1.x());
    obj1.setX(INT_MAX);
    QCOMPARE(INT_MAX, obj1.x());

    // int QPoint::y()
    // void QPoint::setY(int)
    obj1.setY(0);
    QCOMPARE(0, obj1.y());
    obj1.setY(INT_MIN);
    QCOMPARE(INT_MIN, obj1.y());
    obj1.setY(INT_MAX);
    QCOMPARE(INT_MAX, obj1.y());

    QPointF obj2;
    // qreal QPointF::x()
    // void QPointF::setX(qreal)
    obj2.setX(0.0);
    QCOMPARE(0.0, obj2.x());
    obj2.setX(1.1);
    QCOMPARE(1.1, obj2.x());

    // qreal QPointF::y()
    // void QPointF::setY(qreal)
    obj2.setY(0.0);
    QCOMPARE(0.0, obj2.y());
    obj2.setY(1.1);
    QCOMPARE(1.1, obj2.y());
}

static inline qreal dot(const QPointF &a, const QPointF &b)
{
    return a.x() * b.x() + a.y() * b.y();
}

void tst_QPoint::division()
{
    {
        QPointF p(1e-14, 1e-14);
        p = p / sqrt(dot(p, p));
        qFuzzyCompare(dot(p, p), 1);
    }
    {
        QPointF p(1e-14, 1e-14);
        p /= sqrt(dot(p, p));
        qFuzzyCompare(dot(p, p), 1);
    }
}

QTEST_MAIN(tst_QPoint)
#include "tst_qpoint.moc"
