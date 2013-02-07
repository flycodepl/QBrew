/***************************************************************************
  helpviewer.cpp
  -------------------
  General purpose help file viewer
  -------------------
  Copyright 2007-2008 David Johnson
  Please see the header file for copyright and license information.
***************************************************************************/

#include <QDesktopServices>
#include <QFileInfo>
#include <QLocale>
#include <QPrintDialog>
#include <QPrinter>
#include <QTextBrowser>

#include "resource.h"
#include "helpviewer.h"

using namespace Resource;

//////////////////////////////////////////////////////////////////////////////
// Construction                                                             //
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// HelpViewer()
// ------------
// Constructor

HelpViewer::HelpViewer(const QString &home, QWidget *parent)
    : QMainWindow(parent), browser_(0), printer_(0)
{
    setUnifiedTitleAndToolBarOnMac(true);
    ui.setupUi(this);

    browser_ = new QTextBrowser(this);

    QFileInfo hinfo(home);
    QString helppath = hinfo.absolutePath();
    QString helpfile = hinfo.fileName();
    browser_->setSearchPaths(QStringList(helppath));
    if (!helpfile.isEmpty()) browser_->setSource(helpfile);


    textChanged();
    setCentralWidget(browser_);

    // setup actions
    connect(ui.actionback, SIGNAL(triggered()), browser_, SLOT(backward()));
    connect(ui.actionforward, SIGNAL(triggered()), browser_, SLOT(forward()));
    connect(ui.actiongohome, SIGNAL(triggered()), browser_, SLOT(home()));
    connect(ui.actionfileprint, SIGNAL(triggered()), this, SLOT(print()));
    connect(ui.actionquit, SIGNAL(triggered()), this, SLOT(close()));

    // setup connections
    connect(browser_, SIGNAL(backwardAvailable(bool)),
            ui.actionback, SLOT(setEnabled(bool)));
    connect(browser_, SIGNAL(forwardAvailable(bool)),
            ui.actionforward, SLOT(setEnabled(bool)));
    connect(browser_, SIGNAL(textChanged()),
            this, SLOT(textChanged()));
    connect(browser_, SIGNAL(highlighted(const QString&)),
            statusBar(), SLOT(showMessage(const QString&)));
    connect(browser_, SIGNAL(anchorClicked(const QUrl&)),
            this, SLOT(anchorClicked(const QUrl&)));

    browser_->setFocus();
}

//////////////////////////////////////////////////////////////////////////////
// ~HelpViewer()
// -------------
// Destructor

HelpViewer::~HelpViewer()
{
    if (browser_) delete browser_;
    if (printer_) delete printer_;
}

//////////////////////////////////////////////////////////////////////////////
// textChanged()
// -------------
// The displayed document has changed, so change caption

void HelpViewer::textChanged()
{
    QString title = browser_->documentTitle().simplified(); // cleanup
    setWindowTitle(tr("%1 Help - %2").arg(TITLE.constData()).arg(title));
}

//////////////////////////////////////////////////////////////////////////////
// print()
// -------
// Print the contents of the help window

void HelpViewer::print()
{
    // Note: we are not sharing printer with application
    if (!printer_) {
        printer_ = new QPrinter(QPrinter::HighResolution);
        printer_->setFullPage(true);
        // for convenience, default to US_Letter for US/Canada
        switch (QLocale::system().country()) {
          case QLocale::AnyCountry:
          case QLocale::Canada:
          case QLocale::UnitedStates:
          case QLocale::UnitedStatesMinorOutlyingIslands:
              printer_->setPageSize(QPrinter::Letter); break;
          default:
              printer_->setPageSize(QPrinter::A4); break;
        }
    }

    QPrintDialog *pdialog = new QPrintDialog(printer_, this);
    if (pdialog->exec() == QDialog::Accepted) {
        browser_->document()->print(printer_);
    }
    delete pdialog;
}

void HelpViewer::anchorClicked(const QUrl &link)
{
    if (link.scheme().isEmpty()) {
        browser_->setSource(link);
    } else {
        QDesktopServices::openUrl(link);
        browser_->setSource(browser_->source());
    }
}
