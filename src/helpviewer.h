/***************************************************************************
  helpviewer.h
  -------------------
  General purpose help file viewer
  -------------------
  Copyright 2007-2008 David Johnson
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

#ifndef HELPVIEWER_H
#define HELPVIEWER_H

#include <QMainWindow>

#include "ui_helpviewer.h"

class QPrinter;
class QTextBrowser;

class HelpViewer : public QMainWindow
{
    Q_OBJECT
public:
    // constructor
    HelpViewer(const QString& home, QWidget* parent=0);
    // destructor
    ~HelpViewer();
    
private slots:
    // if text has changed
    void textChanged();
    // print contents of browser
    void print();
    // hypertext link clicked
    void anchorClicked(const QUrl &link);

private:
    Ui::HelpViewer ui;
    QTextBrowser* browser_;
    QPrinter* printer_;
};

#endif // HELPVIEWER_H
