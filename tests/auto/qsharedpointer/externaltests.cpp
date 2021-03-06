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


#include "externaltests.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QProcess>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QDateTime>

#ifndef DEFAULT_MAKESPEC
# error DEFAULT_MAKESPEC not defined
#endif

static QString makespec()
{
    static const char default_makespec[] = DEFAULT_MAKESPEC;
    const char *p;
    for (p = default_makespec + sizeof(default_makespec); p >= default_makespec; --p)
        if (*p == '/' || *p == '\\')
            break;

    return QString::fromLatin1(p + 1);
}

static bool removeRecursive(const QString &pathname)
{
    QFileInfo fi(pathname);
    if (!fi.exists())
        return true;

    if (fi.isFile())
        return QFile::remove(pathname);

    if (!fi.isDir()) {
        //  not a file or directory. How do I remove it?
        return false;
    }

    // not empty -- we must empty it first
    QDirIterator di(pathname, QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot);
    while (di.hasNext()) {
        di.next();
        if (!di.fileInfo().exists() && !di.fileInfo().isSymLink())
            continue;
        bool ok;
        if (di.fileInfo().isFile() || di.fileInfo().isSymLink())
            ok = QFile::remove(di.filePath());
        else
            ok = removeRecursive(di.filePath());
        if (!ok) {
            return false;
        }
    }

    QDir dir(pathname);
    QString dirname = dir.dirName();
    dir.cdUp();
    return dir.rmdir(dirname);
}

QT_BEGIN_NAMESPACE
namespace QTest {
    class QExternalTestPrivate
    {
    public:
        QExternalTestPrivate()
            : qtModules(QExternalTest::QtCore | QExternalTest::QtGui | QExternalTest::QtTest),
              appType(QExternalTest::AutoApplication),
              debugMode(true),
              exitCode(-1)
        {
        }
        ~QExternalTestPrivate()
        {
            clear();
        }

        enum Target { Compile, Link, Run };

        QList<QByteArray> qmakeLines;
        QByteArray programHeader;
        QExternalTest::QtModules qtModules;
        QExternalTest::ApplicationType appType;
        bool debugMode;

        QString temporaryDir;
        QByteArray sourceCode;
        QByteArray std_out;
        QByteArray std_err;
        int exitCode;
        QExternalTest::Stage failedStage;

        void clear();
        bool tryCompile(const QByteArray &body);
        bool tryLink(const QByteArray &body);
        bool tryRun(const QByteArray &body);

    private:
        void removeTemporaryDirectory();
        bool createTemporaryDirectory();
        bool prepareSourceCode(const QByteArray &body);
        bool createProjectFile();
        bool runQmake();
        bool runMake(Target target);
        bool commonSetup(const QByteArray &body);
    };

    QExternalTest::QExternalTest()
        : d(new QExternalTestPrivate)
    {
    }

    QExternalTest::~QExternalTest()
    {
        delete d;
    }

    bool QExternalTest::isDebugMode() const
    {
        return d->debugMode;
    }

    void QExternalTest::setDebugMode(bool enable)
    {
        d->debugMode = enable;
    }

    QList<QByteArray> QExternalTest::qmakeSettings() const
    {
        return d->qmakeLines;
    }

    void QExternalTest::setQmakeSettings(const QList<QByteArray> &settings)
    {
        d->qmakeLines = settings;
    }

    QExternalTest::QtModules QExternalTest::qtModules() const
    {
        return d->qtModules;
    }

    void QExternalTest::setQtModules(QtModules modules)
    {
        d->qtModules = modules;
    }

    QExternalTest::ApplicationType QExternalTest::applicationType() const
    {
        return d->appType;
    }

    void QExternalTest::setApplicationType(ApplicationType type)
    {
        d->appType = type;
    }

    QByteArray QExternalTest::programHeader() const
    {
        return d->programHeader;
    }

    void QExternalTest::setProgramHeader(const QByteArray &header)
    {
        d->programHeader = header;
    }

    bool QExternalTest::tryCompile(const QByteArray &body)
    {
        return d->tryCompile(body) && d->exitCode == 0;
    }

    bool QExternalTest::tryLink(const QByteArray &body)
    {
        return d->tryLink(body) && d->exitCode == 0;
    }

    bool QExternalTest::tryRun(const QByteArray &body)
    {
        return d->tryRun(body) && d->exitCode == 0;
    }

    bool QExternalTest::tryCompileFail(const QByteArray &body)
    {
        return d->tryCompile(body) && d->exitCode != 0;
    }

    bool QExternalTest::tryLinkFail(const QByteArray &body)
    {
        return d->tryLink(body) && d->exitCode != 0;
    }

    bool QExternalTest::tryRunFail(const QByteArray &body)
    {
        return d->tryRun(body) && d->exitCode != 0;
    }

    QExternalTest::Stage QExternalTest::failedStage() const
    {
        return d->failedStage;
    }

    int QExternalTest::exitCode() const
    {
        return d->exitCode;
    }

    QByteArray QExternalTest::fullProgramSource() const
    {
        return d->sourceCode;
    }

    QByteArray QExternalTest::standardOutput() const
    {
        return d->std_out;
    }

    QByteArray QExternalTest::standardError() const
    {
        return d->std_err;
    }

    QString QExternalTest::errorReport() const
    {
        const char *stage = 0;
        switch (d->failedStage) {
        case FileStage:
            stage = "creating files";
            break;
        case QmakeStage:
            stage = "executing qmake";
            break;
        case CompilationStage:
            stage = "during compilation";
            break;
        case LinkStage:
            stage = "during linking";
            break;
        case RunStage:
            stage = "executing program";
            break;
        }

        QString report = QString::fromLatin1(
            "External test failed %1 with exit code %4\n"
            "==== standard error: ====\n"
            "%2\n"
            "==== standard output: ====\n"
            "%3\n"
            "==== ====\n");
        return report.arg(QString::fromLatin1(stage),
                          QString::fromLocal8Bit(d->std_err),
                          QString::fromLocal8Bit(d->std_out))
            .arg(d->exitCode);
    }

    // actual execution code
    void QExternalTestPrivate::clear()
    {
        if (!temporaryDir.isEmpty())
            removeTemporaryDirectory();

        sourceCode.clear();
        std_out.clear();
        std_err.clear();
        exitCode = -1;
        failedStage = QExternalTest::FileStage;
    }

    void QExternalTestPrivate::removeTemporaryDirectory()
    {
        Q_ASSERT(!temporaryDir.isEmpty());
        removeRecursive(temporaryDir);
        temporaryDir.clear();
    }

    bool QExternalTestPrivate::prepareSourceCode(const QByteArray &body)
    {
        sourceCode.clear();
        sourceCode.reserve(8192);

        sourceCode += programHeader;
        sourceCode += '\n';

        // Add Qt header includes
        if (qtModules & QExternalTest::QtCore)
            sourceCode += "#include <QtCore/QtCore>\n";
        if (qtModules & QExternalTest::QtGui)
            sourceCode += "#include <QtGui/QtGui>\n";
        if (qtModules & QExternalTest::QtNetwork)
            sourceCode += "#include <QtNetwork/QtNetwork>\n";
        if (qtModules & QExternalTest::QtXml)
            sourceCode += "#include <QtXml/QtXml>\n";
        if (qtModules & QExternalTest::QtXmlPatterns)
            sourceCode += "#include <QtXmlPatterns/QtXmlPatterns>\n";
        if (qtModules & QExternalTest::QtOpenGL)
            sourceCode += "#include <QtOpenGL/QtOpenGL>\n";
        if (qtModules & QExternalTest::QtSql)
            sourceCode += "#include <QtSql/QtSql>\n";
        if (qtModules & QExternalTest::Qt3Support)
            sourceCode += "#include <Qt3Support/Qt3Support>\n";
        if (qtModules & QExternalTest::QtSvg)
            sourceCode += "#include <QtSvg/QtSvg>\n";
        if (qtModules & QExternalTest::QtScript)
            sourceCode += "#include <QtScript/QtScript>\n";
        if (qtModules & QExternalTest::QtTest)
            sourceCode += "#include <QtTest/QtTest>\n";
        if (qtModules & QExternalTest::QtDBus)
            sourceCode += "#include <QtDBus/QtDBus>\n";
        if (qtModules & QExternalTest::QtWebKit)
            sourceCode += "#include <QtWebKit/QtWebKit>\n";
        if (qtModules & QExternalTest::Phonon)
            sourceCode += "#include <Phonon/Phonon>\n";
        sourceCode +=
            "#include <stdlib.h>\n"
            "#include <stddef.h>\n";

        sourceCode +=
            "\n"
            "void q_external_test_user_code()\n"
            "{\n"
            "    // HERE STARTS THE USER CODE\n";
        sourceCode += body;
        sourceCode +=
            "\n"
            "    // HERE ENDS THE USER CODE\n"
            "}\n"
            "\n"
            "#ifdef Q_OS_WIN\n"
            "#include <windows.h>\n"
            "static void q_test_setup()\n"
            "{\n"
            "    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);\n"
            "}\n"
            "#else\n"
            "static void q_test_setup() { }\n"
            "#endif\n"
            "int main(int argc, char **argv)\n"
            "{\n";

        switch (appType) {
        applicationless:
        case QExternalTest::Applicationless:
            sourceCode +=
                "    (void)argc; (void)argv;\n";
            break;

        coreapplication:
        case QExternalTest::QCoreApplication:
            sourceCode +=
                "    QCoreApplication app(argc, argv);\n";
            break;

        case QExternalTest::QApplicationTty:
            sourceCode +=
                "    QApplication app(argc, argv, QApplication::Tty);\n";
            break;

        guiapplication:
        case QExternalTest::QApplicationGuiClient:
            sourceCode +=
                "    QApplication app(argc, argv, QApplication::GuiClient);\n";
            break;

        case QExternalTest::QApplicationGuiServer:
            sourceCode +=
                "    QApplication app(argc, argv, QApplication::GuiServer);\n";
            break;

        case QExternalTest::AutoApplication:
            if (qtModules & QExternalTest::QtGui)
                goto guiapplication;
            if (qtModules == 0)
                goto applicationless;
            goto coreapplication;
        }

        sourceCode +=
            "    q_test_setup();\n"
            "    q_external_test_user_code();\n"
            "    return 0;\n"
            "}\n";

        QFile sourceFile(temporaryDir + QLatin1String("/project.cpp"));
        if (!sourceFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            std_err = sourceFile.errorString().toLocal8Bit();
            return false;
        }

        sourceFile.write(sourceCode);
        return true;
    }

    bool QExternalTestPrivate::createTemporaryDirectory()
    {
        QDir temp = QDir::temp();
        QString subdir = QString::fromLatin1("qexternaltest-%1-%2-%3")
                        .arg(QDateTime::currentDateTime().toString(QLatin1String("yyyyMMddhhmmss")))
                        .arg(quintptr(this), 0, 16)
                        .arg(qrand());
        if (!temp.mkdir(subdir))
            return false;

        if (!temp.cd(subdir))
            return false;

        temporaryDir = temp.absolutePath();
        return true;
    }

    bool QExternalTestPrivate::createProjectFile()
    {
        Q_ASSERT(!temporaryDir.isEmpty());

        QFile projectFile(temporaryDir + QLatin1String("/project.pro"));
        if (!projectFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            std_err = projectFile.errorString().toLocal8Bit();
            return false;
        }

        projectFile.write(
            "TEMPLATE = app\n"
            "\n"
            "TARGET   = externaltest\n"
            "CONFIG   -= app_bundle\n"        // for the Mac
            "CONFIG   -= debug_and_release\n"
            "DESTDIR  = .\n"
            "OBJECTS_DIR = .\n"
            "UI_DIR   = .\n"
            "MOC_DIR  = .\n"
            "RCC_DIR  = .\n"
            "HEADERS  +=\n"
            "SOURCES  += project.cpp\n"
            "QT       -= core gui\n"
            "INCLUDEPATH += . ");
        projectFile.write(QFile::encodeName(QDir::currentPath()));

        if (debugMode)
            projectFile.write("\nCONFIG  += debug\n");
        else
            projectFile.write("\nCONFIG  += release\n");

        // Add Qt modules
        if (qtModules & QExternalTest::QtCore)
            projectFile.write("QT += core\n");
        if (qtModules & QExternalTest::QtGui)
            projectFile.write("QT += gui\n");
        if (qtModules & QExternalTest::QtNetwork)
            projectFile.write("QT += network\n");
        if (qtModules & QExternalTest::QtXml)
            projectFile.write("QT += xml\n");
        if (qtModules & QExternalTest::QtXmlPatterns)
            projectFile.write("QT += xmlpatterns\n");
        if (qtModules & QExternalTest::QtOpenGL)
            projectFile.write("QT += opengl\n");
        if (qtModules & QExternalTest::QtSql)
            projectFile.write("QT += sql\n");
        if (qtModules & QExternalTest::Qt3Support)
            projectFile.write("QT += qt3support\n");
        if (qtModules & QExternalTest::QtSvg)
            projectFile.write("QT += svg\n");
        if (qtModules & QExternalTest::QtScript)
            projectFile.write("QT += script\n");
        if (qtModules & QExternalTest::QtTest)
            projectFile.write("QT += testlib\n");
        if (qtModules & QExternalTest::QtDBus)
            projectFile.write("QT += dbus\n");
        if (qtModules & QExternalTest::QtWebKit)
            projectFile.write("QT += webkit\n");
        if (qtModules & QExternalTest::Phonon)
            projectFile.write("QT += phonon\n");

        projectFile.write("\n### User-specified settings start ###\n");
        foreach (QByteArray line, qmakeLines) {
            projectFile.write(line);
            projectFile.write("\n");
        }
        projectFile.write("\n### User-specified settings end ###\n");

        // Use qmake to just compile:
        projectFile.write(
            "\n"
            "test_compile.depends        += $(OBJECTS)\n"
            "QMAKE_EXTRA_TARGETS += test_compile\n");

        // Use qmake to run the app too:
        projectFile.write(
            "\n"
            "unix:test_run.commands     = ./$(QMAKE_TARGET)\n"
            "else:test_run.commands     = $(QMAKE_TARGET)\n"
            "embedded:test_run.commands += -qws\n"
            "QMAKE_EXTRA_TARGETS += test_run\n");

        return true;
    }

    bool QExternalTestPrivate::runQmake()
    {
        Q_ASSERT(!temporaryDir.isEmpty());
        if (!createProjectFile())
            return false;

        failedStage = QExternalTest::QmakeStage;
        QProcess qmake;
        QStringList args;
        args << QLatin1String("-makefile")
             << QLatin1String("-spec")
             << makespec()
             << QLatin1String("project.pro");
        qmake.setWorkingDirectory(temporaryDir);
        qmake.start(QLatin1String("qmake"), args);

        std_out += "### --- stdout from qmake --- ###\n";
        std_err += "### --- stderr from qmake --- ###\n";
        bool ok = qmake.waitForStarted();
        if (!ok) {
            exitCode = 255;
            std_err += "qmake: ";
            std_err += qmake.errorString().toLocal8Bit();
        } else {
            ok = qmake.waitForFinished();
            exitCode = qmake.exitCode();

            std_out += qmake.readAllStandardOutput();
            std_err += qmake.readAllStandardError();
        }

        return ok && exitCode == 0;
    }

    bool QExternalTestPrivate::runMake(Target target)
    {
        Q_ASSERT(!temporaryDir.isEmpty());

        QProcess make;
        make.setWorkingDirectory(temporaryDir);

        QStringList environment = QProcess::systemEnvironment();
        environment += QLatin1String("LC_ALL=C");
        make.setEnvironment(environment);

        QStringList args;
        if (target == Compile)
            args << QLatin1String("test_compile");
        else if (target == Run)
            args << QLatin1String("test_run");

#if defined(Q_OS_WIN) && !defined(Q_CC_MINGW)
        make.start(QLatin1String("nmake.exe"), args);
        make.waitForStarted();
#else
        static const char makes[] =
# ifdef Q_CC_MINGW
            "mingw32-make.exe\0"
# endif
            "gmake\0"
            "make\0";
        for (const char *p = makes; *p; p += strlen(p) + 1) {
            make.start(QLatin1String(p), args);
            if (make.waitForStarted())
                break;
        }
#endif

        if (make.state() != QProcess::Running) {
            exitCode = 255;
            std_err += "make: ";
            std_err += make.errorString().toLocal8Bit();
            return false;
        }

        bool ok = make.waitForFinished();
        exitCode = make.exitCode();
        std_out += make.readAllStandardOutput();
        std_err += make.readAllStandardError();

        return ok;
    }

    bool QExternalTestPrivate::commonSetup(const QByteArray &body)
    {
        clear();

        if (!createTemporaryDirectory())
            return false;
        if (!createProjectFile())
            return false;
        if (!prepareSourceCode(body))
            return false;
        if (!runQmake())
            return false;
        return true;
    }

    bool QExternalTestPrivate::tryCompile(const QByteArray &body)
    {
        if (!commonSetup(body))
            return false;

        // compile
        failedStage = QExternalTest::CompilationStage;
        std_out += "\n### --- stdout from make (compilation) --- ###\n";
        std_err += "\n### --- stderr from make (compilation) --- ###\n";
        return runMake(Compile);
    }

    bool QExternalTestPrivate::tryLink(const QByteArray &body)
    {
        if (!tryCompile(body) || exitCode != 0)
            return false;

        // link
        failedStage = QExternalTest::LinkStage;
        std_out += "\n### --- stdout from make (linking) --- ###\n";
        std_err += "\n### --- stderr from make (linking) --- ###\n";
        return runMake(Link);
    }

    bool QExternalTestPrivate::tryRun(const QByteArray &body)
    {
        if (!tryLink(body) || exitCode != 0)
            return false;

        // run
        failedStage = QExternalTest::RunStage;
        std_out += "\n### --- stdout from process --- ###\n";
        std_err += "\n### --- stderr from process --- ###\n";
        return runMake(Run);
    }
}
QT_END_NAMESPACE
