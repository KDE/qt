#!/usr/bin/env perl
#############################################################################
##
## Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/legal
##
## This file is part of the test suite of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Digia.  For licensing terms and
## conditions see http://qt.digia.com/licensing.  For further information
## use the contact form at http://qt.digia.com/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Digia gives you certain additional
## rights.  These rights are described in the Digia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
##
## $QT_END_LICENSE$
##
#############################################################################

sub makeit2($$$)
{
    for (my $i = 0; $i < (1 << $_[0]); $i++) {
        print OUTFILE "\n";
        print OUTFILE "$_[2]\n" unless $3 eq "";
        print OUTFILE "msgid \"singular $_[1] $i\"\n";
        print OUTFILE "msgid_plural \"plural $_[1] $i\"\n";
        for (my $j = 0; $j < $_[0]; $j++) {
            my $tr;
            if (($i & (1 << $j)) == 0) {
                $tr = "translated $_[1] $i $j";
            }
            print OUTFILE "msgstr[$j] \"$tr\"\n";
        }
    }
}

sub makeit($$)
{
    open OUTFILE, ">${OUTDIR}plural-$_[0].po" || die "cannot write file in $OUTDIR";
    print OUTFILE <<EOF;
msgid ""
msgstr ""
"X-FooBar: yup\\n"
"X-Language: $_[1]\\n"
EOF
    makeit2($_[0], "one", "");
    makeit2($_[0], "two", "#, fuzzy
#| msgid \"old untranslated one\"");
    makeit2($_[0], "three", "#, fuzzy
#| msgid \"old untranslated two\"
#| msgid_plural \"old untranslated plural two\"");
    makeit2($_[0], "four", "#, fuzzy
#| msgid_plural \"old untranslated only plural three\"");
}

$OUTDIR = $ARGV[0];
makeit(1, "zh_CN");
makeit(2, "de_DE");
makeit(3, "pl_PL");
