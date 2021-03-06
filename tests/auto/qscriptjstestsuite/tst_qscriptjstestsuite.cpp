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

#include <QtScript>

//TESTED_CLASS=
//TESTED_FILES=

// Uncomment the following define to have the autotest generate
// addExpectedFailure() code for all the tests that fail.
// This is useful when a whole new test (sub)suite is added.
// The code is stored in addexpectedfailures.cpp.
// Paste the contents into this file after the existing
// addExpectedFailure() calls.

//#define GENERATE_ADDEXPECTEDFAILURE_CODE

static QString readFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();
    QTextStream stream(&file);
    return stream.readAll();
}

static void appendCString(QVector<char> *v, const char *s)
{
    char c;
    do {
        c = *(s++);
        *v << c;
    } while (c != '\0');
}

struct TestRecord
{
    TestRecord() : lineNumber(-1) { }
    TestRecord(const QString &desc,
               bool pass,
               const QString &act,
               const QString &exp,
               const QString &fn, int ln)
        : description(desc), passed(pass),
          actual(act), expected(exp),
          fileName(fn), lineNumber(ln)
        { }
    TestRecord(const QString &skipReason, const QString &fn)
        : description(skipReason), actual("QSKIP"),
          fileName(fn), lineNumber(-1)
        { }
    QString description;
    bool passed;
    QString actual;
    QString expected;
    QString fileName;
    int lineNumber;
};

Q_DECLARE_METATYPE(TestRecord)

struct FailureItem
{
    enum Action {
        ExpectFail,
        Skip
    };
    FailureItem(Action act, const QRegExp &rx, const QString &desc, const QString &msg)
        : action(act), pathRegExp(rx), description(desc), message(msg)
        { }

    Action action;
    QRegExp pathRegExp;
    QString description;
    QString message;
};

class tst_Suite : public QObject
{

public:
    tst_Suite();
    virtual ~tst_Suite();

    static QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

private:
    void addExpectedFailure(const QString &fileName, const QString &description, const QString &message);
    void addExpectedFailure(const QRegExp &path, const QString &description, const QString &message);
    void addSkip(const QString &fileName, const QString &description, const QString &message);
    void addSkip(const QRegExp &path, const QString &description, const QString &message);
    bool isExpectedFailure(const QString &fileName, const QString &description,
                           QString *message, FailureItem::Action *action) const;
    void addFileExclusion(const QString &fileName, const QString &message);
    void addFileExclusion(const QRegExp &rx, const QString &message);
    bool isExcludedFile(const QString &fileName, QString *message) const;

    QDir testsDir;
    QList<QString> subSuitePaths;
    QList<FailureItem> expectedFailures;
    QList<QPair<QRegExp, QString> > fileExclusions;
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
    QString generatedAddExpectedFailureCode;
#endif
};

QMetaObject tst_Suite::staticMetaObject;

Q_GLOBAL_STATIC(QVector<uint>, qt_meta_data_tst_Suite)
Q_GLOBAL_STATIC(QVector<char>, qt_meta_stringdata_tst_Suite)

const QMetaObject *tst_Suite::metaObject() const
{
    return &staticMetaObject;
}

void *tst_Suite::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_tst_Suite()->constData()))
        return static_cast<void*>(const_cast<tst_Suite*>(this));
    return QObject::qt_metacast(_clname);
}

static QScriptValue qscript_void(QScriptContext *, QScriptEngine *eng)
{
    return eng->undefinedValue();
}

static QScriptValue qscript_quit(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError("Test quit");
}

static QString optionsToString(int options)
{
    QSet<QString> set;
    if (options & 1)
        set.insert("strict");
    if (options & 2)
        set.insert("werror");
    if (options & 4)
        set.insert("atline");
    if (options & 8)
        set.insert("xml");
    return QStringList(set.values()).join(",");
}

static QScriptValue qscript_options(QScriptContext *ctx, QScriptEngine *)
{
    static QHash<QString, int> stringToFlagHash;
    if (stringToFlagHash.isEmpty()) {
        stringToFlagHash["strict"] = 1;
        stringToFlagHash["werror"] = 2;
        stringToFlagHash["atline"] = 4;
        stringToFlagHash["xml"] = 8;
    }
    QScriptValue callee = ctx->callee();
    int opts = callee.data().toInt32();
    QString result = optionsToString(opts);
    for (int i = 0; i < ctx->argumentCount(); ++i)
        opts |= stringToFlagHash.value(ctx->argument(0).toString());
    callee.setData(opts);
    return result;
}

static QScriptValue qscript_TestCase(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue origTestCaseCtor = ctx->callee().data();
    QScriptValue kase = ctx->thisObject();
    QScriptValue ret = origTestCaseCtor.call(kase, ctx->argumentsObject());
    QScriptContextInfo info(ctx->parentContext());
    kase.setProperty("__lineNumber__", QScriptValue(eng, info.lineNumber()));
    return ret;
}

int tst_Suite::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (!(_id & 1)) {
            // data
            QTest::addColumn<TestRecord>("record");
            bool hasData = false;

            QString testsShellPath = testsDir.absoluteFilePath("shell.js");
            QString testsShellContents = readFile(testsShellPath);

            QDir subSuiteDir(subSuitePaths.at(_id / 2));
            QString subSuiteShellPath = subSuiteDir.absoluteFilePath("shell.js");
            QString subSuiteShellContents = readFile(subSuiteShellPath);

            QDir testSuiteDir(subSuiteDir);
            testSuiteDir.cdUp();
            QString suiteJsrefPath = testSuiteDir.absoluteFilePath("jsref.js");
            QString suiteJsrefContents = readFile(suiteJsrefPath);
            QString suiteShellPath = testSuiteDir.absoluteFilePath("shell.js");
            QString suiteShellContents = readFile(suiteShellPath);

            QFileInfoList testFileInfos = subSuiteDir.entryInfoList(QStringList() << "*.js", QDir::Files);
            foreach (QFileInfo tfi, testFileInfos) {
                if ((tfi.fileName() == "shell.js") || (tfi.fileName() == "browser.js"))
                    continue;

                QString abspath = tfi.absoluteFilePath();
                QString relpath = testsDir.relativeFilePath(abspath);
                QString excludeMessage;
                if (isExcludedFile(relpath, &excludeMessage)) {
                    QTest::newRow(relpath.toLatin1()) << TestRecord(excludeMessage, relpath);
                    continue;
                }

                QScriptEngine eng;
                QScriptValue global = eng.globalObject();
                global.setProperty("print", eng.newFunction(qscript_void));
                global.setProperty("quit", eng.newFunction(qscript_quit));
                global.setProperty("options", eng.newFunction(qscript_options));

                eng.evaluate(testsShellContents, testsShellPath);
                if (eng.hasUncaughtException()) {
                    QStringList bt = eng.uncaughtExceptionBacktrace();
                    QString err = eng.uncaughtException().toString();
                    qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
                    break;
                }

                eng.evaluate(suiteJsrefContents, suiteJsrefPath);
                if (eng.hasUncaughtException()) {
                    QStringList bt = eng.uncaughtExceptionBacktrace();
                    QString err = eng.uncaughtException().toString();
                    qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
                    break;
                }

                eng.evaluate(suiteShellContents, suiteShellPath);
                if (eng.hasUncaughtException()) {
                    QStringList bt = eng.uncaughtExceptionBacktrace();
                    QString err = eng.uncaughtException().toString();
                    qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
                    break;
                }

                eng.evaluate(subSuiteShellContents, subSuiteShellPath);
                if (eng.hasUncaughtException()) {
                    QStringList bt = eng.uncaughtExceptionBacktrace();
                    QString err = eng.uncaughtException().toString();
                    qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
                    break;
                }

                QScriptValue origTestCaseCtor = global.property("TestCase");
                QScriptValue myTestCaseCtor = eng.newFunction(qscript_TestCase);
                myTestCaseCtor.setData(origTestCaseCtor);
                global.setProperty("TestCase", myTestCaseCtor);

                global.setProperty("gTestfile", tfi.fileName());
                global.setProperty("gTestsuite", testSuiteDir.dirName());
                global.setProperty("gTestsubsuite", subSuiteDir.dirName());
                QString testFileContents = readFile(abspath);
//                qDebug() << relpath;
                eng.evaluate(testFileContents, abspath);
                if (eng.hasUncaughtException() && !relpath.endsWith("-n.js")) {
                    QStringList bt = eng.uncaughtExceptionBacktrace();
                    QString err = eng.uncaughtException().toString();
                    qWarning("%s\n%s\n", qPrintable(err), qPrintable(bt.join("\n")));
                    continue;
                }

                QScriptValue testcases = global.property("testcases");
                if (!testcases.isArray())
                    testcases = global.property("gTestcases");
                int count = testcases.property("length").toInt32();
                if (count == 0)
                    continue;

                hasData = true;
                QString title = global.property("TITLE").toString();
                for (int i = 0; i < count; ++i) {
                    QScriptValue kase = testcases.property(i);
                    QString description = kase.property("description").toString();
                    QScriptValue expect = kase.property("expect");
                    QScriptValue actual = kase.property("actual");
                    bool passed = kase.property("passed").toBoolean();
                    int lineNumber = kase.property("__lineNumber__").toInt32();

                    TestRecord rec(description, passed,
                                   actual.toString(), expect.toString(),
                                   relpath, lineNumber);

                    QTest::newRow(description.toLatin1()) << rec;
                }
            }
            if (!hasData)
                QTest::newRow("") << TestRecord(); // dummy
        } else {
            QFETCH(TestRecord, record);
            if ((record.lineNumber == -1) && (record.actual == "QSKIP")) {
                QTest::qSkip(record.description.toLatin1(), QTest::SkipAll, record.fileName.toLatin1(), -1);
            } else {
                QString msg;
                FailureItem::Action failAct;
                bool expectFail = isExpectedFailure(record.fileName, record.description, &msg, &failAct);
                if (expectFail) {
                    switch (failAct) {
                    case FailureItem::ExpectFail:
                        QTest::qExpectFail("", msg.toLatin1(),
                                           QTest::Continue, record.fileName.toLatin1(),
                                           record.lineNumber);
                        break;
                    case FailureItem::Skip:
                        QTest::qSkip(msg.toLatin1(), QTest::SkipSingle,
                                     record.fileName.toLatin1(), record.lineNumber);
                        break;
                    }
                }
                if (!expectFail || (failAct == FailureItem::ExpectFail)) {
                    if (!record.passed) {
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
                        if (!expectFail) {
                            QString escapedDescription = record.description;
                            escapedDescription.replace("\\", "\\\\");
                            escapedDescription.replace("\n", "\\n");
                            escapedDescription.replace("\"", "\\\"");
                            generatedAddExpectedFailureCode.append(
                                "    addExpectedFailure(\"" + record.fileName
                                + "\", \"" + escapedDescription +
                                "\", willFixInNextReleaseMessage);\n");
                        }
#endif
                        QTest::qCompare(record.actual, record.expected, "actual", "expect",
                                        record.fileName.toLatin1(), record.lineNumber);
                    } else {
                        QTest::qCompare(record.actual, record.actual, "actual", "expect",
                                        record.fileName.toLatin1(), record.lineNumber);
                    }
                }
            }
        }
        _id -= subSuitePaths.size()*2;
    }
    return _id;
}

tst_Suite::tst_Suite()
{
    testsDir = QDir(".");
    bool testsFound = testsDir.cd("tests");
    if (!testsFound) {
        qWarning("*** no tests/ dir!");
    }

    QString willFixInNextReleaseMessage = QString::fromLatin1("Will fix in next release");
    QString brokenOnSomePlatformsMessage = QString::fromLatin1("Doesn't behave the same on all platforms");
    QString fromCharCodeMessage = QString::fromLatin1("Test is wrong?");
    for (int i = 4256; i < 4294; ++i) {
        addExpectedFailure("ecma/String/15.5.4.11-2.js", QString::fromLatin1("var s = new String( String.fromCharCode(%0) ); s.toLowerCase().charCodeAt(0)").arg(i), fromCharCodeMessage);
    }
    addExpectedFailure("ecma/String/15.5.4.11-5.js", "var s = new String( String.fromCharCode(1024) ); s.toLowerCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.11-5.js", "var s = new String( String.fromCharCode(1037) ); s.toLowerCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.12-1.js", "var s = new String( String.fromCharCode(181) ); s.toUpperCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.12-1.js", "var s = new String( String.fromCharCode(329) ); s.toUpperCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.12-4.js", "var s = new String( String.fromCharCode(1104) ); s.toUpperCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.12-4.js", "var s = new String( String.fromCharCode(1117) ); s.toUpperCase().charCodeAt(0)", fromCharCodeMessage);
    addExpectedFailure("ecma/String/15.5.4.12-5.js", "var s = new String( String.fromCharCode(1415) ); s.toUpperCase().charCodeAt(0)", fromCharCodeMessage);

    addExpectedFailure("ecma/String/15.5.4.6-2.js", "function f() { return this; }; function g() { var h = f; return h(); }; g().toString()", willFixInNextReleaseMessage);

    addExpectedFailure("ecma/TypeConversion/9.3.1-3.js", "- \"-0x123456789abcde8\"", willFixInNextReleaseMessage);

    addExpectedFailure("ecma/extensions/15.1.2.1-1.js", "var PROPS = ''; for ( p in eval ) { PROPS += p }; PROPS", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/GlobalObject/15.1.2.2-1.js", "var PROPS=''; for ( var p in parseInt ) { PROPS += p; }; PROPS", willFixInNextReleaseMessage);

    addSkip("ecma/GlobalObject/15.1.2.2-2.js", "parseInt(s,36)", brokenOnSomePlatformsMessage);

    addExpectedFailure("ecma/GlobalObject/15.1.2.3-1.js", "var MYPROPS=''; for ( var p in parseFloat ) { MYPROPS += p }; MYPROPS", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/GlobalObject/15.1.2.4.js", "var MYPROPS=''; for ( var p in escape ) { MYPROPS+= p}; MYPROPS", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/GlobalObject/15.1.2.5-1.js", "var MYPROPS=''; for ( var p in unescape ) { MYPROPS+= p }; MYPROPS", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/GlobalObject/15.1.2.6.js", "var MYPROPS=''; for ( var p in isNaN ) { MYPROPS+= p }; MYPROPS", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/GlobalObject/15.1.2.7.js", "var MYPROPS=''; for ( p in isFinite ) { MYPROPS+= p }; MYPROPS", willFixInNextReleaseMessage);

    addExpectedFailure("ecma/Statements/12.6.3-12.js", "var result=''; for ( aVar in this ) { if (aVar == 'aVar') {return a failure}; result", willFixInNextReleaseMessage);
    addExpectedFailure("ecma/String/15.5.4.6-2.js", "var d = new Date(0); d.indexOf = String.prototype.indexOf; d.getTimezoneOffset()>0 ? d.indexOf('31') : d.indexOf('01')", willFixInNextReleaseMessage);

    // qstrtod() has problems parsing reaaaaally big numbers -- they come out as NaN rather than Infinity or Number.MAX_VALUE
    addSkip("ecma/TypeConversion/9.3.1-3.js", "parseInt(s1,10) == 1.7976931348623157e+308 || parseInt(s1,10) == Infinity", brokenOnSomePlatformsMessage);
    addSkip("ecma/TypeConversion/9.3.1-3.js", "parseInt(s2,10) == Infinity || parseInt(s2,10) == 1.7976931348623157e+308", brokenOnSomePlatformsMessage);
    addSkip("ecma/TypeConversion/9.3.1-3.js", "parseInt(s1) == 1.7976931348623157e+308 || parseInt(s1) == Infinity", brokenOnSomePlatformsMessage);
    addSkip("ecma/TypeConversion/9.3.1-3.js", "parseInt(s2) == Infinity || parseInt(s2) == 1.7976931348623157e+308", brokenOnSomePlatformsMessage);
    addSkip("ecma/TypeConversion/9.3.1-3.js", "parseInt(s,36)", brokenOnSomePlatformsMessage);

#if defined(Q_OS_SOLARIS)
    addExpectedFailure("ecma/Math/15.8.2.2.js", "Math.acos(11.00000001)", "Fails on Solaris");
    addExpectedFailure("ecma/Math/15.8.2.3.js", "Math.asin(-1.000001)", "Fails on Solaris");
#endif

    addExpectedFailure(QRegExp(), "NO TESTS EXIST", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_2/RegExp/multiline-001.js", "/.*[y]$/m.exec(ivory-billed\ndowny\nhairy\nacorn\nyellow-bellied sapsucker\nnorthern flicker\npileated\n)", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/RegExp/multiline-001.js", "/.*[d]$/m.exec(ivory-billed\ndowny\nhairy\nacorn\nyellow-bellied sapsucker\nnorthern flicker\npileated\n)", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "//.toString()", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "( Boston, Mass. 02134 ).match(/([d]{5})([- ]?[d]{4})?$/)[2]", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "( Boston, MA 02134 ).match(re = /([d]{5})([- ]?[d]{4})?$/; re.lastIndex =0)[2]", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "( Boston, MA 02134 ).match(re = /([d]{5})([- ]?[d]{4})?$/; re.lastIndex = 16)[2]", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "( Boston, MA 02134 ).match(re = /([d]{5})([- ]?[d]{4})?$/; re.lastIndex = 11)[2]", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_2/String/match-002.js", "( Boston, MA 02134 ).match(re = /([d]{5})([- ]?[d]{4})?$/; re.lastIndex = 111)[2]", willFixInNextReleaseMessage);

#if defined(Q_WS_WIN)
    addExpectedFailure(QRegExp(), "VAR1 = 0; VAR2= Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = -0; VAR2= Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = 0; VAR2= -Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = -0; VAR2= -Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = 1; VAR2= Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = -1; VAR2= Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = -1; VAR2= -Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "VAR1 = 1; VAR2= -Infinity; VAR1 %= VAR2;VAR1", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "1 % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "1 % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-1 % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-1 % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Number.MAX_VALUE % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Number.MAX_VALUE % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-Number.MAX_VALUE % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-Number.MAX_VALUE % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "0 % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "0 % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-0 % Number.POSITIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "-0 % Number.NEGATIVE_INFINITY", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Math.atan2(Infinity, Infinity)", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Math.atan2(Infinity, -Infinity)", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Math.atan2(-Infinity, Infinity)", willFixInNextReleaseMessage);
    addExpectedFailure(QRegExp(), "Math.atan2(-Infinity, -Infinity)", willFixInNextReleaseMessage);
#endif

    addExpectedFailure("ecma_3/Array/15.4.4.11-01.js", "Array.sort should not eat exceptions", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Array/15.4.4.3-1.js", "Testing Array.prototype.toLocaleString() -", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Array/15.4.5.1-01.js", "15.4.5.1 - array.length coverage", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/ExecutionContexts/10.1.4-1.js", "Expected to be able to delete x", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/extensions/regress-220367-002.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-220367-002.js", "Section 2 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-220367-002.js", "Section 3 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-220367-002.js", "Section 4 of test - ", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/extensions/regress-228087-002.js",
                       "Section 1 of test - \nregexp = /{1.*}/g\n"
                       "string = 'foo {1} foo {2} foo'\n"
                       "ERROR !!! match arrays have different lengths:\n"
                       "Expect: [\"{1} foo {2}\"]\n"
                       "Actual: []", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js",
                       "Section 1 of test - \n"
                       "regexp = /a|ab/\n"
                       "string = 'abc'\n"
                       "ERROR !!! regexp failed to give expected match array:\n"
                       "Expect: [\"a\"]\n"
                       "Actual: [\"ab\"]\n", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js",
                       "Section 2 of test - \n"
                       "regexp = /((a)|(ab))((c)|(bc))/\n"
                       "string = 'abc'\n"
                       "ERROR !!! regexp failed to give expected match array:\n"
                       "Expect: [\"abc\", \"a\", \"a\", , \"bc\", , \"bc\"]\n"
                       "Actual: [\"abc\", \"ab\", \"\", \"ab\", \"c\", \"c\", \"\"]\n", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js",
                       "Section 4 of test - \n"
                       "regexp = /a[a-z]{2,4}?/\n"
                       "string = 'abcdefghi'\n"
                       "ERROR !!! regexp FAILED to match anything !!!\n"
                       "Expect: abc\n"
                       "Actual: null\n", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js(317)",
                       "Section 5 of test - \n"
                       "regexp = /(aa|aabaac|ba|b|c)*/\n"
                       "string = 'aabaac'\n"
                       "ERROR !!! regexp failed to give expected match array:\n"
                       "Expect: [\"aaba\", \"ba\"]\n"
                       "Actual: [\"aabaac\", \"aabaac\"]\n", willFixInNextReleaseMessage);

    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 1 of test - \nregexp = /{1.*}/g\nstring = 'foo {1} foo {2} foo'\nERROR !!! match arrays have different lengths:\nExpect: [\"{1} foo {2}\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 2 of test - \nregexp = /{1.*}/g\nstring = 'foo {1} foo {2} foo'\nERROR !!! match arrays have different lengths:\nExpect: [\"{1} foo {2}\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 3 of test - \nregexp = /{1[.!}]*}/g\nstring = 'foo {1} foo {2} foo'\nERROR !!! match arrays have different lengths:\nExpect: [\"{1}\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 4 of test - \nregexp = /{1[.!}]*}/g\nstring = 'foo {1} foo {2} foo'\nERROR !!! match arrays have different lengths:\nExpect: [\"{1}\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 6 of test - \nregexp = /c{3 }/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 7 of test - \nregexp = /c{3.}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 8 of test - \nregexp = /c{3\\s}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 9 of test - \nregexp = /c{3[ ]}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 10 of test - \nregexp = /c{ 3}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{ 3}\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 12 of test - \nregexp = /c{3, }/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3, }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 13 of test - \nregexp = /c{3 ,}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 ,}\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 15 of test - \nregexp = /c{3 ,4}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3 ,4}\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 16 of test - \nregexp = /c{3, 4}/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3, 4}\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-228087.js", "Section 17 of test - \nregexp = /c{3,4 }/\nstring = 'abccccc{3 }c{ 3}c{3, }c{3 ,}c{3 ,4}c{3, 4}c{3,4 }de'\nERROR !!! regexp FAILED to match anything !!!\nExpect: c{3,4 }\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 0", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 1", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 2", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 3", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 4", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 5", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 6", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 7", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-274152.js", "Do not ignore unicode format-control characters: 8", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-368516.js", "Treat unicode BOM characters as whitespace: 0", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/extensions/regress-368516.js", "Treat unicode BOM characters as whitespace: 1", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/arguments-001.js", "Section 3 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-131964.js", "Section 3 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-131964.js", "Section 4 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-137181.js", "Section 2 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section A of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section B of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section C of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section D of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section E of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section F of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section G of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-85880.js", "Section H of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Function/regress-94506.js", "Section 3 of test", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/FunExpr/fe-001-n.js", "Previous statement should have thrown a ReferenceError", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/FunExpr/fe-002.js", "Inner function statement should not have been called.", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/LexicalConventions/7.9.1.js", "Automatic Semicolon insertion in postfix expressions: expr\n++", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/LexicalConventions/7.9.1.js", "Automatic Semicolon insertion in postfix expressions: expr\n--", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/LexicalConventions/7.9.1.js", "Automatic Semicolon insertion in postfix expressions: (x\n)-- y", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/LexicalConventions/7.9.1.js", "Automatic Semicolon insertion in postfix expressions: (x)-- y", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Number/15.7.4.2-01.js", "3.3.toString.length should be 1", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Number/15.7.4.6-1.js", "Section A of test: no error intended!", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Number/15.7.4.7-2.js", "num.toPrecision(undefined) should equal num.toString()", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Object/8.6.1-01.js", "In strict mode, setting a read-only property should generate a warning: Throw if STRICT and WERROR is enabled", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Object/8.6.2.6-001.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Operators/order-01.js", "operator evaluation order: 11.8.2 >", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Operators/order-01.js", "operator evaluation order: 11.8.4 >=", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 5 of test - \nregexp = /(aa|aabaac|ba|b|c)*/\nstring = 'aabaac'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"aaba\", \"ba\"]\nActual: [\"aabaac\", \"aabaac\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 6 of test - \nregexp = /^(a+)\\1*,\\1+$/\nstring = 'aaaaaaaaaa,aaaaaaaaaaaaaaa'\nERROR !!! regexp FAILED to match anything !!!\nExpect: aaaaaaaaaa,aaaaaaaaaaaaaaa,aaaaa\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 7 of test - \nregexp = /(z)((a+)?(b+)?(c))*/\nstring = 'zaacbbbcac'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"zaacbbbcac\", \"z\", \"ac\", \"a\", , \"c\"]\nActual: [\"zaacbbbcac\", \"z\", \"ac\", \"a\", \"\", \"c\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 8 of test - \nregexp = /(a*)*/\nstring = 'b'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\", , ]\nActual: [\"\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 10 of test - \nregexp = /(?=(a+))/\nstring = 'baaabac'\nERROR !!! match arrays have different lengths:\nExpect: [\"\", \"aaa\"]\nActual: [\"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 11 of test - \nregexp = /(?=(a+))a*b\\1/\nstring = 'baaabac'\nERROR !!! match arrays have different lengths:\nExpect: [\"aba\", \"a\"]\nActual: [\"aaab\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 12 of test - \nregexp = /(.*?)a(?!(a+)b\\2c)\\2(.*)/\nstring = 'baaabaac'\nERROR !!! regexp FAILED to match anything !!!\nExpect: baaabaac,ba,,abaac\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2-1.js", "Section 13 of test - \nregexp = /(?=(a+))/\nstring = 'baaabac'\nERROR !!! match arrays have different lengths:\nExpect: [\"\", \"aaa\"]\nActual: [\"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/15.10.2.12.js", "15.10.2.12 - CharacterClassEscape d", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 34 of test - \nregexp = /a]/\nstring = 'a]'\nERROR !!! regexp FAILED to match anything !!!\nExpect: a]\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 66 of test - \nregexp = /a.+?c/\nstring = 'abcabc'\nERROR !!! regexp FAILED to match anything !!!\nExpect: abc\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 94 of test - \nregexp = /^a(bc+|b[eh])g|.h$/\nstring = 'abh'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"bh\", , ]\nActual: [\"bh\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 95 of test - \nregexp = /(bc+d$|ef*g.|h?i(j|k))/\nstring = 'effgz'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"effgz\", \"effgz\", , ]\nActual: [\"effgz\", \"effgz\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 97 of test - \nregexp = /(bc+d$|ef*g.|h?i(j|k))/\nstring = 'reffgz'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"effgz\", \"effgz\", , ]\nActual: [\"effgz\", \"effgz\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 109 of test - \nregexp = /(([a-c])b*?\\2)*/\nstring = 'ababbbcbc'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ababb,bb,b\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 110 of test - \nregexp = /(([a-c])b*?\\2){3}/\nstring = 'ababbbcbc'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ababbbcbc,cbc,c\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 119 of test - \nregexp = /ab*?bc/i\nstring = 'ABBBBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBBBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 120 of test - \nregexp = /ab{0,}?bc/i\nstring = 'ABBBBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBBBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 121 of test - \nregexp = /ab+?bc/i\nstring = 'ABBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 123 of test - \nregexp = /ab{1,}?bc/i\nstring = 'ABBBBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBBBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 124 of test - \nregexp = /ab{1,3}?bc/i\nstring = 'ABBBBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBBBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 125 of test - \nregexp = /ab{3,4}?bc/i\nstring = 'ABBBBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBBBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 126 of test - \nregexp = /ab??bc/i\nstring = 'ABBC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABBC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 127 of test - \nregexp = /ab??bc/i\nstring = 'ABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 128 of test - \nregexp = /ab{0,1}?bc/i\nstring = 'ABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 129 of test - \nregexp = /ab??c/i\nstring = 'ABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 130 of test - \nregexp = /ab{0,1}?c/i\nstring = 'ABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 138 of test - \nregexp = /a.*?c/i\nstring = 'AXYZC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: AXYZC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 144 of test - \nregexp = /a]/i\nstring = 'A]'\nERROR !!! regexp FAILED to match anything !!!\nExpect: A]\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 160 of test - \nregexp = /a.+?c/i\nstring = 'ABCABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 161 of test - \nregexp = /a.*?c/i\nstring = 'ABCABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 162 of test - \nregexp = /a.{0,5}?c/i\nstring = 'ABCABC'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ABC\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 169 of test - \nregexp = /(a+|b){0,1}?/i\nstring = 'AB'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ,\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 191 of test - \nregexp = /^a(bc+|b[eh])g|.h$/i\nstring = 'ABH'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"BH\", , ]\nActual: [\"BH\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 192 of test - \nregexp = /(bc+d$|ef*g.|h?i(j|k))/i\nstring = 'EFFGZ'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"EFFGZ\", \"EFFGZ\", , ]\nActual: [\"EFFGZ\", \"EFFGZ\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 194 of test - \nregexp = /(bc+d$|ef*g.|h?i(j|k))/i\nstring = 'REFFGZ'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"EFFGZ\", \"EFFGZ\", , ]\nActual: [\"EFFGZ\", \"EFFGZ\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 212 of test - \nregexp = /a(?:b|c|d)+?(.)/\nstring = 'ace'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ace,e\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 213 of test - \nregexp = /a(?:b|c|d)+?(.)/\nstring = 'acdbcdbe'\nERROR !!! regexp FAILED to match anything !!!\nExpect: acd,d\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 217 of test - \nregexp = /a(?:b|c|d){4,5}?(.)/\nstring = 'acdbcdbe'\nERROR !!! regexp FAILED to match anything !!!\nExpect: acdbcd,d\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 218 of test - \nregexp = /((foo)|(bar))*/\nstring = 'foobar'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"foobar\", \"bar\", , \"bar\"]\nActual: [\"foobar\", \"bar\", \"\", \"bar\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 220 of test - \nregexp = /a(?:b|c|d){6,7}?(.)/\nstring = 'acdbcdbe'\nERROR !!! regexp FAILED to match anything !!!\nExpect: acdbcdbe,e\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 222 of test - \nregexp = /a(?:b|c|d){5,6}?(.)/\nstring = 'acdbcdbe'\nERROR !!! regexp FAILED to match anything !!!\nExpect: acdbcdb,b\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 224 of test - \nregexp = /a(?:b|c|d){5,7}?(.)/\nstring = 'acdbcdbe'\nERROR !!! regexp FAILED to match anything !!!\nExpect: acdbcdb,b\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 225 of test - \nregexp = /a(?:b|(c|e){1,2}?|d)+?(.)/\nstring = 'ace'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ace,c,e\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 227 of test - \nregexp = /^([^a-z])|(\\^)$/\nstring = '.'\nERROR !!! regexp failed to give expected match array:\nExpect: [\".\", \".\", , ]\nActual: [\".\", \".\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 234 of test - \nregexp = /(?:(f)(o)(o)|(b)(a)(r))*/\nstring = 'foobar'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"foobar\", , , , \"b\", \"a\", \"r\"]\nActual: [\"foobar\", \"\", \"\", \"\", \"b\", \"a\", \"r\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 240 of test - \nregexp = /(?:..)*?a/\nstring = 'aba'\nERROR !!! regexp FAILED to match anything !!!\nExpect: a\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 241 of test - \nregexp = /^(?:b|a(?=(.)))*\\1/\nstring = 'abc'\nERROR !!! match arrays have different lengths:\nExpect: [\"ab\", , ]\nActual: [\"ab\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 246 of test - \nregexp = /(a|x)*ab/\nstring = 'cab'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"ab\", , ]\nActual: [\"ab\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 247 of test - \nregexp = /(a)*ab/\nstring = 'cab'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"ab\", , ]\nActual: [\"ab\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 300 of test - \nregexp = /(?=(a+?))(\\1ab)/\nstring = 'aaab'\nERROR !!! regexp FAILED to match anything !!!\nExpect: aab,a,aab\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 303 of test - \nregexp = /(?=(a+?))(\\1ab)/\nstring = 'aaab'\nERROR !!! regexp FAILED to match anything !!!\nExpect: aab,a,aab\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 304 of test - \nregexp = /([\\w:]+::)?(\\w+)$/\nstring = 'abcd'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"abcd\", , \"abcd\"]\nActual: [\"abcd\", \"\", \"abcd\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 308 of test - \nregexp = /([\\w:]+::)?(\\w+)$/\nstring = 'abcd'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"abcd\", , \"abcd\"]\nActual: [\"abcd\", \"\", \"abcd\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 342 of test - \nregexp = /a$/m\nstring = 'a\\nb\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: a\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 344 of test - \nregexp = /a$/m\nstring = 'b\\na\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: a\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 353 of test - \nregexp = /aa$/m\nstring = 'aa\\nb\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: aa\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 355 of test - \nregexp = /aa$/m\nstring = 'b\\naa\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: aa\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 364 of test - \nregexp = /ab$/m\nstring = 'ab\\nb\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ab\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 366 of test - \nregexp = /ab$/m\nstring = 'b\\nab\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: ab\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 375 of test - \nregexp = /abb$/m\nstring = 'abb\\nb\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: abb\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 377 of test - \nregexp = /abb$/m\nstring = 'b\\nabb\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: abb\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 383 of test - \nregexp = /^d[x][x][x]/m\nstring = 'abcd\\ndxxx'\nERROR !!! regexp FAILED to match anything !!!\nExpect: dxxx\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 391 of test - \nregexp = /\\.c(pp|xx|c)?$/i\nstring = 'IO.c'\nERROR !!! regexp failed to give expected match array:\nExpect: [\".c\", , ]\nActual: [\".c\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 392 of test - \nregexp = /(\\.c(pp|xx|c)?$)/i\nstring = 'IO.c'\nERROR !!! regexp failed to give expected match array:\nExpect: [\".c\", \".c\", , ]\nActual: [\".c\", \".c\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 394 of test - \nregexp = /^([ab]*?)(b)?(c)$/\nstring = 'abac'\nERROR !!! regexp FAILED to match anything !!!\nExpect: abac,aba,,c\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 412 of test - \nregexp = /^(a(b)?)+$/\nstring = 'aba'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"aba\", \"a\", , ]\nActual: [\"aba\", \"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 413 of test - \nregexp = /^(aa(bb)?)+$/\nstring = 'aabbaa'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"aabbaa\", \"aa\", , ]\nActual: [\"aabbaa\", \"aa\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 414 of test - \nregexp = /^.{9}abc.*\\n/m\nstring = '123\\nabcabcabcabc\\n'\nERROR !!! regexp FAILED to match anything !!!\nExpect: abcabcabcabc\n\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 415 of test - \nregexp = /^(a)?a$/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", , ]\nActual: [\"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 416 of test - \nregexp = /^(a\\1?)(a\\1?)(a\\2?)(a\\3?)$/\nstring = 'aaaaaa'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"aaaaaa\", \"a\", \"aa\", \"a\", \"aa\"]\nActual: [\"aaaaaa\", \"aa\", \"a\", \"aa\", \"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 418 of test - \nregexp = /^(0+)?(?:x(1))?/\nstring = 'x1'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"x1\", , \"1\"]\nActual: [\"x1\", \"\", \"1\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 419 of test - \nregexp = /^([0-9a-fA-F]+)(?:x([0-9a-fA-F]+)?)(?:x([0-9a-fA-F]+))?/\nstring = '012cxx0190'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"012cxx0190\", \"012c\", , \"0190\"]\nActual: [\"012cxx0190\", \"012c\", \"\", \"0190\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 420 of test - \nregexp = /^(b+?|a){1,2}c/\nstring = 'bbbac'\nERROR !!! regexp FAILED to match anything !!!\nExpect: bbbac,a\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-001.js", "Section 421 of test - \nregexp = /^(b+?|a){1,2}c/\nstring = 'bbbbac'\nERROR !!! regexp FAILED to match anything !!!\nExpect: bbbbac,a\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/perlstress-002.js", "Section 40 of test - \nregexp = /(a)|\\1/\nstring = 'x'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\", , ]\nActual: [\"\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-105972.js", "Section 1 of test - \nregexp = /^.*?$/\nstring = 'Hello World'\nERROR !!! regexp FAILED to match anything !!!\nExpect: Hello World\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-105972.js", "Section 2 of test - \nregexp = /^.*?/\nstring = 'Hello World'\nERROR !!! regexp FAILED to match anything !!!\nExpect: \nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-105972.js", "Section 3 of test - \nregexp = /^.*?(:|$)/\nstring = 'Hello: World'\nERROR !!! regexp FAILED to match anything !!!\nExpect: Hello:,:\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-123437.js", "Section 1 of test - \nregexp = /(a)?a/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", , ]\nActual: [\"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-123437.js", "Section 2 of test - \nregexp = /a|(b)/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", , ]\nActual: [\"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-123437.js", "Section 3 of test - \nregexp = /(a)?(a)/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", , \"a\"]\nActual: [\"a\", \"\", \"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-165353.js", "Section 1 of test - \nregexp = /^([a-z]+)*[a-z]$/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", , ]\nActual: [\"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-169497.js", "Section 1 of test - \nregexp = /<body.*>((.*\\n?)*?)<\\/body>/i\nstring = '<html>\\n<body onXXX=\"alert(event.type);\">\\n<p>Kibology for all</p>\\n<p>All for Kibology</p>\\n</body>\\n</html>'\nERROR !!! regexp FAILED to match anything !!!\nExpect: <body onXXX=\"alert(event.type);\">\n<p>Kibology for all</p>\n<p>All for Kibology</p>\n</body>,\n<p>Kibology for all</p>\n<p>All for Kibology</p>\n,<p>All for Kibology</p>\n\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-187133.js", "Section 5 of test - \nregexp = /(?!a|b)|c/\nstring = 'bc'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\"]\nActual: [\"c\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 2 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 3 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 4 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 5 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 6 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 9 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 10 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 11 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 12 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 28 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 29 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 30 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 31 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 32 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 33 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 34 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 35 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-188206.js", "Section 36 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 1 of test - \nregexp = /(\\d|\\d\\s){2,}/\nstring = '12 3 45'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"12\", \"2\"]\nActual: [\"12 3 45\", \"5\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 3 of test - \nregexp = /(\\d|\\d\\s)+/\nstring = '12 3 45'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"12\", \"2\"]\nActual: [\"12 3 45\", \"5\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 8 of test - \nregexp = /(\\d|\\d\\s){2,}?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 12,2\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 9 of test - \nregexp = /(\\d|\\d\\s){4,}?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 12 3 4,4\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 10 of test - \nregexp = /(\\d|\\d\\s)+?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 1,1\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 11 of test - \nregexp = /(\\d\\s?){4,}?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 12 3 4,4\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 12 of test - \nregexp = /(\\d\\s|\\d){2,}?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 12 ,2 \nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 13 of test - \nregexp = /(\\d\\s|\\d){4,}?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 12 3 4,4\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-191479.js", "Section 14 of test - \nregexp = /(\\d\\s|\\d)+?/\nstring = '12 3 45'\nERROR !!! regexp FAILED to match anything !!!\nExpect: 1,1\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-202564.js", "Section 1 of test - \nregexp = /(?:(.+), )?(.+), (..) to (?:(.+), )?(.+), (..)/\nstring = 'Seattle, WA to Buckley, WA'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"Seattle, WA to Buckley, WA\", , \"Seattle\", \"WA\", , \"Buckley\", \"WA\"]\nActual: [\"Seattle, WA to Buckley, WA\", \"\", \"Seattle\", \"WA\", \"\", \"Buckley\", \"WA\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-209919.js", "Section 2 of test - \nregexp = /(a|b*){5,}/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", \"\"]\nActual: [\"a\", \"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-209919.js", "Section 3 of test - \nregexp = /(b*)*/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\", , ]\nActual: [\"\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-209919.js", "Section 5 of test - \nregexp = /^\\-?(\\d{1,}|\\.{0,})*(\\,\\d{1,})?$/\nstring = '100.00'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"100.00\", \"00\", , ]\nActual: [\"100.00\", \"00\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-216591.js", "Section 1 of test - \nregexp = /\\{(([a-z0-9\\-_]+?\\.)+?)([a-z0-9\\-_]+?)\\}/i\nstring = 'a {result.data.DATA} b'\nERROR !!! regexp FAILED to match anything !!!\nExpect: {result.data.DATA},result.data.,data.,DATA\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-216591.js", "Section 2 of test - \nregexp = /\\{(([a-z0-9\\-_]+?\\.)+?)([a-z0-9\\-_]+?)\\}/gi\nstring = 'a {result.data.DATA} b'\nERROR !!! match arrays have different lengths:\nExpect: [\"{result.data.DATA}\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-220367-001.js", "Section 1 of test - \nregexp = /(a)|(b)/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", \"a\", , ]\nActual: [\"a\", \"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-220367-001.js", "Section 2 of test - \nregexp = /(a)|(b)/\nstring = 'b'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"b\", , \"b\"]\nActual: [\"b\", \"\", \"b\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 2 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 3 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 4 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 11 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223273.js", "Section 12 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223535.js", "Section 2 of test - \nregexp = /|a/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\"]\nActual: [\"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223535.js", "Section 6 of test - \nregexp = /(|a)/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\", \"\"]\nActual: [\"a\", \"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-223535.js", "Section 7 of test - \nregexp = /(|a|)/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"\", \"\"]\nActual: [\"a\", \"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-224676.js", "Section 17 of test - \nregexp = /[x]b|(a)/\nstring = 'ZZZxbZZZ'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"xb\", , ]\nActual: [\"xb\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-224676.js", "Section 18 of test - \nregexp = /[x]b|()a/\nstring = 'ZZZxbZZZ'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"xb\", , ]\nActual: [\"xb\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-225289.js", "Section 7 of test - \nregexp = /(a)|([^a])/\nstring = 'a'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"a\", \"a\", , ]\nActual: [\"a\", \"a\", \"\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-225289.js", "Section 9 of test - \nregexp = /(a)|([^a])/\nstring = '()'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"(\", , \"(\"]\nActual: [\"(\", \"\", \"(\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-225289.js", "Section 10 of test - \nregexp = /((?:a|[^a])*)/g\nstring = 'a'\nERROR !!! match arrays have different lengths:\nExpect: [\"a\", \"\"]\nActual: [\"a\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-225289.js", "Section 11 of test - \nregexp = /((?:a|[^a])*)/g\nstring = ''\nERROR !!! match arrays have different lengths:\nExpect: [\"\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-225289.js", "Section 12 of test - \nregexp = /((?:a|[^a])*)/g\nstring = '()'\nERROR !!! match arrays have different lengths:\nExpect: [\"()\", \"\"]\nActual: [\"()\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-309840.js", "Treat / in a literal regexp class as valid", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-31316.js", "Section 1 of test - \nregexp = /<([^\\/<>][^<>]*[^\\/])>|<([^\\/<>])>/\nstring = '<p>Some<br />test</p>'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"<p>\", , \"p\"]\nActual: [\"<p>\", \"\", \"p\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-330684.js", "Do not hang on RegExp", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-375711.js", "Do not assert with /[Q-b]/i.exec(\"\"): /[q-b]/.exec(\"\")", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-375711.js", "Do not assert with /[Q-b]/i.exec(\"\"): /[q-b]/i.exec(\"\")", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(?)'and flag 'i'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(?)'and flag 'g'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(?)'and flag 'm'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(?)'and flag 'undefined'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(a'and flag 'i'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(a'and flag 'g'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(a'and flag 'm'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '(a'and flag 'undefined'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '( ]'and flag 'i'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '( ]'and flag 'g'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '( ]'and flag 'm'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-57631.js", "Testing for error creating illegal RegExp object on pattern '( ]'and flag 'undefined'", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-78156.js", "Section 1 of test - \nregexp = /^\\d/gm\nstring = 'aaa\\n789\\r\\nccc\\r\\n345'\nERROR !!! match arrays have different lengths:\nExpect: [\"7\", \"3\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-78156.js", "Section 2 of test - \nregexp = /\\d$/gm\nstring = 'aaa\\n789\\r\\nccc\\r\\n345'\nERROR !!! match arrays have different lengths:\nExpect: [\"9\", \"5\"]\nActual: [\"5\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-78156.js", "Section 3 of test - \nregexp = /^\\d/gm\nstring = 'aaa\\n789\\r\\nccc\\r\\nddd'\nERROR !!! match arrays have different lengths:\nExpect: [\"7\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-78156.js", "Section 4 of test - \nregexp = /\\d$/gm\nstring = 'aaa\\n789\\r\\nccc\\r\\nddd'\nERROR !!! match arrays have different lengths:\nExpect: [\"9\"]\nActual: []\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-85721.js", "Section 2 of test - \nregexp = /<sql:connection id=\"([^\\r\\n]*?)\">\\s*<sql:url>\\s*([^\\r\\n]*?)\\s*<\\/sql:url>\\s*<sql:driver>\\s*([^\\r\\n]*?)\\s*<\\/sql:driver>\\s*(\\s*<sql:userId>\\s*([^\\r\\n]*?)\\s*<\\/sql:userId>\\s*)?\\s*(\\s*<sql:password>\\s*([^\\r\\n]*?)\\s*<\\/sql:password>\\s*)?\\s*<\\/sql:connection>/\nstring = '<sql:connection id=\"conn1\"> <sql:url>www.m.com</sql:url> <sql:driver>drive.class</sql:driver>\\n<sql:userId>foo</sql:userId> <sql:password>goo</sql:password> </sql:connection>'\nERROR !!! regexp FAILED to match anything !!!\nExpect: <sql:connection id=\"conn1\"> <sql:url>www.m.com</sql:url> <sql:driver>drive.class</sql:driver>\n<sql:userId>foo</sql:userId> <sql:password>goo</sql:password> </sql:connection>,conn1,www.m.com,drive.class,<sql:userId>foo</sql:userId> ,foo,<sql:password>goo</sql:password> ,goo\nActual: null\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-87231.js", "Section 3 of test - \nregexp = /^(A)?(A.*)$/\nstring = 'A'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"A\", , \"A\"]\nActual: [\"A\", \"\", \"A\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/RegExp/regress-87231.js", "Section 6 of test - \nregexp = /(A)?(A.*)/\nstring = 'zxcasd;fl\\  ^AaaAAaaaf;lrlrzs'\nERROR !!! regexp failed to give expected match array:\nExpect: [\"AaaAAaaaf;lrlrzs\", , \"AaaAAaaaf;lrlrzs\"]\nActual: [\"AaaAAaaaf;lrlrzs\", \"\", \"AaaAAaaaf;lrlrzs\"]\n", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Statements/regress-226517.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 14", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 20", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 24", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 27", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 28", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 29", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 30", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 31", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 32", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 33", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 34", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 35", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 36", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 37", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 39", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 56", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 58", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 59", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 60", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 62", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 64", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 65", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 66", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 67", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 68", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.11.js", "Section 69", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/15.5.4.14.js", "15.5.4.14 - String.prototype.split(/()/)", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-104375.js", "Section 1 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-104375.js", "Section 2 of test - ", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-304376.js", "String.prototype should be readonly and permanent", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-392378.js", "Regular Expression Non-participating Capture Groups are inaccurate in edge cases: \"y\".split(/(x)?y/)", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-392378.js", "Regular Expression Non-participating Capture Groups are inaccurate in edge cases: \"y\".split(/(x)?y/)", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-392378.js", "Regular Expression Non-participating Capture Groups are inaccurate in edge cases: \"y\".replace(/(x)?\\1y/, function($0, $1){ return String($1); })", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-392378.js", "Regular Expression Non-participating Capture Groups are inaccurate in edge cases: \"y\".replace(/(x)?y/, function($0, $1){ return String($1); })", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/String/regress-392378.js", "Regular Expression Non-participating Capture Groups are inaccurate in edge cases: \"y\".replace(/(x)?y/, function($0, $1){ return $1; })", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Unicode/regress-352044-01.js", "issues with Unicode escape sequences in JavaScript source code", willFixInNextReleaseMessage);
    addExpectedFailure("ecma_3/Unicode/uc-001.js", "Unicode format-control character test (Category Cf.)", willFixInNextReleaseMessage);

    addFileExclusion(".+/15\\.9\\.5\\..+", "too slooow");
    addFileExclusion("regress-130451.js", "asserts");
    addFileExclusion("regress-322135-01.js", "asserts");
    addFileExclusion("regress-322135-02.js", "asserts");
    addFileExclusion("regress-322135-03.js", "takes forever");
    addFileExclusion("regress-322135-04.js", "takes forever");
    addFileExclusion("ecma_3/RegExp/regress-375715-04.js", "bug");

    static const char klass[] = "tst_QScriptJsTestSuite";

    QVector<uint> *data = qt_meta_data_tst_Suite();
    // content:
    *data << 1 // revision
          << 0 // classname
          << 0 << 0 // classinfo
          << 0 << 10 // methods (backpatched later)
          << 0 << 0 // properties
          << 0 << 0 // enums/sets
        ;

    QVector<char> *stringdata = qt_meta_stringdata_tst_Suite();
    appendCString(stringdata, klass);
    appendCString(stringdata, "");

// don't execute any tests on slow machines
#if !defined(Q_OS_IRIX)
    // do all the test suites
    QFileInfoList testSuiteDirInfos;
    if (testsFound)
        testSuiteDirInfos = testsDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo tsdi, testSuiteDirInfos) {
        QDir testSuiteDir(tsdi.absoluteFilePath());
        // do all the dirs in the test suite
        QFileInfoList subSuiteDirInfos = testSuiteDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
        foreach (QFileInfo ssdi, subSuiteDirInfos) {
            subSuitePaths.append(ssdi.absoluteFilePath());
            // slot: signature, parameters, type, tag, flags
            QString data_slot = QString::fromLatin1("%0/%1_data()")
                           .arg(testSuiteDir.dirName()).arg(ssdi.fileName());
            static const int nullbyte = sizeof(klass);
            *data << stringdata->size() << nullbyte << nullbyte << nullbyte << 0x08;
            appendCString(stringdata, data_slot.toLatin1());
            QString slot = QString::fromLatin1("%0/%1()")
                           .arg(testSuiteDir.dirName()).arg(ssdi.fileName());
            *data << stringdata->size() << nullbyte << nullbyte << nullbyte << 0x08;
            appendCString(stringdata, slot.toLatin1());
        }
    }
#endif

    (*data)[4] = subSuitePaths.size() * 2;

    *data << 0; // eod

    // initialize staticMetaObject
    staticMetaObject.d.superdata = &QObject::staticMetaObject;
    staticMetaObject.d.stringdata = stringdata->constData();
    staticMetaObject.d.data = data->constData();
    staticMetaObject.d.extradata = 0;
}

tst_Suite::~tst_Suite()
{
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
    if (!generatedAddExpectedFailureCode.isEmpty()) {
        QFile file("addexpectedfailures.cpp");
        file.open(QFile::WriteOnly);
        QTextStream ts(&file);
        ts << generatedAddExpectedFailureCode;
    }
#endif
}

void tst_Suite::addExpectedFailure(const QRegExp &path, const QString &description, const QString &message)
{
    expectedFailures.append(FailureItem(FailureItem::ExpectFail, path, description, message));
}

void tst_Suite::addExpectedFailure(const QString &fileName, const QString &description, const QString &message)
{
    expectedFailures.append(FailureItem(FailureItem::ExpectFail, QRegExp(fileName), description, message));
}

void tst_Suite::addSkip(const QRegExp &path, const QString &description, const QString &message)
{
    expectedFailures.append(FailureItem(FailureItem::Skip, path, description, message));
}

void tst_Suite::addSkip(const QString &fileName, const QString &description, const QString &message)
{
    expectedFailures.append(FailureItem(FailureItem::Skip, QRegExp(fileName), description, message));
}

bool tst_Suite::isExpectedFailure(const QString &fileName, const QString &description,
                                  QString *message, FailureItem::Action *action) const
{
    for (int i = 0; i < expectedFailures.size(); ++i) {
        if (expectedFailures.at(i).pathRegExp.indexIn(fileName) != -1) {
            if (description == expectedFailures.at(i).description) {
                if (message)
                    *message = expectedFailures.at(i).message;
                if (action)
                    *action = expectedFailures.at(i).action;
                return true;
            }
        }
    }
    return false;
}

void tst_Suite::addFileExclusion(const QString &fileName, const QString &message)
{
    fileExclusions.append(qMakePair(QRegExp(fileName), message));
}

void tst_Suite::addFileExclusion(const QRegExp &rx, const QString &message)
{
    fileExclusions.append(qMakePair(rx, message));
}

bool tst_Suite::isExcludedFile(const QString &fileName, QString *message) const
{
    for (int i = 0; i < fileExclusions.size(); ++i) {
        if (fileExclusions.at(i).first.indexIn(fileName) != -1) {
            if (message)
                *message = fileExclusions.at(i).second;
            return true;
        }
    }
    return false;
}

QTEST_MAIN(tst_Suite)
