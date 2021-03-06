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

//! [0]
QTextEdit *editor = new QTextEdit;
MyHighlighter *highlighter = new MyHighlighter(editor->document());
//! [0]


//! [1]
void MyHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "\\bMy[A-Za-z]+\\b";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }
}
//! [1]


//! [2]
QTextCharFormat multiLineCommentFormat;
multiLineCommentFormat.setForeground(Qt::red);

QRegExp startExpression("/\\*");
QRegExp endExpression("\\*/");

setCurrentBlockState(0);

int startIndex = 0;
if (previousBlockState() != 1)
    startIndex = text.indexOf(startExpression);

while (startIndex >= 0) {
   int endIndex = text.indexOf(endExpression, startIndex);
   int commentLength;
   if (endIndex == -1) {
       setCurrentBlockState(1);
       commentLength = text.length() - startIndex;
   } else {
       commentLength = endIndex - startIndex
                       + endExpression.matchedLength();
   }
   setFormat(startIndex, commentLength, multiLineCommentFormat);
   startIndex = text.indexOf(startExpression,
                             startIndex + commentLength);
}
//! [2]


//! [3]
void MyHighlighter::highlightBlock(const QString &text)
{
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "\\bMy[A-Za-z]+\\b";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
     }
 }
//! [3]


//! [4]
struct ParenthesisInfo
{
    QChar char;
    int position;
};

struct BlockData : public QTextBlockUserData
{
    QVector<ParenthesisInfo> parentheses;
};
//! [4]
