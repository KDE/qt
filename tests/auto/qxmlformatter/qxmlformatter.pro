load(qttest_p4)
SOURCES += tst_qxmlformatter.cpp

include (../xmlpatterns.pri)

wince*:{
   addFiles.sources = baselines input
   addFiles.path = .
   DEPLOYMENT += addFiles
}
