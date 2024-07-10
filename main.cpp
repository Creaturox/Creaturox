/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
// #include <QDesktopWidget>
#include <QScreen>
#include <QSurfaceFormat>

#include "mainwindow1.h"


// hellogl2 is our main BoxMan application
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow1 mainWindow;
    // mainWindow.setFixedSize(2000, 1300);
    mainWindow.show();
    // mainWindow.showMaximized();
    return app.exec();
}


// INCLUDEPATH += "C:/Users/Ismail/PhD/Software/glm"
// INCLUDEPATH += "D:/backup_labPC_09-10-2015/Ismail Kazmi/Bournemouth University/PhD/Software/glm"
// INCLUDEPATH +=D:/boost/boost_1_59_0
// LIBS +=-LD:/boost/boost_1_59_0/stage/lib/
// LIBS+=-llibboost_system-mgw49-1_59 -lws2_32

/* this space is for book keeping purposes
 *
 *
 * from manahil to Rehan bhai: I hope you had a good day. Is phupo's new car okay? Are you having fun with your new car? What did you
 *                             eat today? Bye Bye!! :-)
 *
 *
 *
 * */
