// -----------------------------------------------------------------------------
//
// NsHelpView.cc
//
// Naiad Studio help view, source file.
//
// Copyright (c) 2011 Exotic Matter AB. All rights reserved.
//
// This file is part of Open Naiad Studio.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------

#include "NsHelpView.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsOpStore.h"
#include "NsOpObject.h"

#include <NgStore.h>
#include <NbPath.h>
#include <NbpValue.h>
#include <NgpOpTree.h>

#include <QDebug>
#include <QtGui>

#if 0
#include <boost/xpressive/xpressive.hpp>

static std::string const&
envlookup(boost::xpressive::smatch const& what)
{
    static std::string envval="";
    const std::string& whatstr = what[1].str();
    envval = std::getenv(whatstr.c_str());
    return envval;
}
#endif

// -----------------------------------------------------------------------------

// createOpHtmlCache
// -----------------
//! Create HTML help pages for each registered Ops. [static]

bool
NsHelpView::createOpHtmlCache()
{
    QDir d;
    if (!d.mkpath(_opHtmlPath())) {

        QMessageBox::information(
            0,
            tr("Path Creation Error"),
            tr("Unable to create the path: ") + _opHtmlPath());

        return false;
    }

#if 0
    // Regenerate HTML for certain shared files ...

    const QString spectrumFileName = 
        fromNbStr(Nb::naiadPath()) + 
        "/docs/html/NOT/spectrum.html";
    QFile spectrumFile(spectrumFileName);
    if (spectrumFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString specHtml = spectrumFile.readAll();
        spectrumFile.close();

        // expand $${ENVVAR}$$ with the $ENVVAR environment variable
        std::string x = fromQStr(specHtml);
        boost::xpressive::sregex re = 
            "$${" >> (boost::xpressive::s1 = +boost::xpressive::_w) >> "}$$";
        std::string cx = boost::xpressive::regex_replace(x, re, envlookup);
        QString expandedSpecHtml(cx.c_str());

        // write the expanded HTML...
        if(!spectrumFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::information(
                0,
                tr("File Creation Error"),
                tr("Unable to create the file: ") +
                spectrumFile.fileName() +": " + spectrumFile.errorString());            
        } else {
            spectrumFile.write(expandedSpecHtml.toAscii());
            spectrumFile.close();
        }
    }

#endif

    // Generate HTML help docs for each Op type.

    const QStringList typeNames(queryOpTypeNames());

    foreach (const QString &type, typeNames) {
        // HTML header...

        QFile helpFile(_opTypeHtmlPath(type));
        if (!helpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::information(
                0,
                tr("File Creation Error"),
                tr("Unable to create the file: ") +
                helpFile.fileName() +": " + helpFile.errorString());
            continue;
        }

        QString html = "<html><head>";
        html += "<meta http-equiv=\"Content-Type\" ";
        html += "content=\"text/html; charset=ISO-8859-1\">";
        html += "<link rel=\"stylesheet\" ";
        html += "href=\"";
        html += fromNbStr(Nb::naiadPath());
        html += "/docs/html/naiad-docs.css\" ";
        html += "type=\"text/css\"></head><body>\n";

        Ngp::OpTree* npt = Ng::Store::opTree(fromQStr(type));

        // Generate HTML for Op class, type and description

        html += "<h2>";
        html += type;
        if (type == "Global") {
            html += " Operator";
        }
        else {
            html += " (" + fromNbStr(npt->familyName()) + ")";
        }
        html += "</h2>\n";
        html += "<p>";
        html += fromNbStr(npt->description());
        html += "</p>\n";

        // Generate HTML for parameter sections

        // TODO: use iterators!
        const std::vector<Nbp::ValueSectionDecl *> sections(npt->sections());
        for (unsigned int i = 0; i < sections.size(); ++i) {
            html += "<h3><u>";
            html += fromNbStr(sections[i]->name()) + " Parameters";
            html += "</u></h3>\n";
            for (int p = 0; p < sections[i]->size(); ++p) {
                if(!sections[i]->value(p)->hidden()) {
                    html += "<dt><b>";
                    html += fromNbStr(sections[i]->value(p)->name());
                    html += "</b></dt>\n";
                    html += "<dd>";
                    html += fromNbStr(sections[i]->value(p)->description());
                    html += "</dd>\n";
                }
            }
            html += "<dl>\n";
        }

        // Generate HTML for inputs

        const std::vector<Ngp::PlugDecl *> inputs(npt->plugs("INPUT"));
        if (inputs.size() > 0) {
            html += "<h3><u>Inputs</u></h3>\n";
            html += "<dl>\n";
            for(unsigned int i = 0; i < inputs.size(); ++i) {
                html += "<dt><b>";
                html += fromNbStr(inputs[i]->name());
                html += "</b></dt>\n";
                html += "<dd>";
                html += fromNbStr(inputs[i]->description());
                html += "</dd>\n";
            }
            html += "<dl>\n";
        }

        // Generate HTML for outputs

        const std::vector<Ngp::PlugDecl *> outputs(npt->plugs("OUTPUT"));
        if (outputs.size() > 0) {
            html += "<h3><u>Outputs</u></h3>\n";
            html += "<dl>\n";
            for(unsigned int i = 0; i < outputs.size(); ++i) {
                html += "<dt><b>";
                html += fromNbStr(outputs[i]->name());
                html += "</b></dt>\n";
                html += "<dd>";
                html += fromNbStr(outputs[i]->description());
                html += "</dd>\n";
            }
            html += "<dl>\n";
        }

        helpFile.write(html.toAscii());
        helpFile.close();
    }

    return true;
}


// _opHtmlPath
// -----------
//! DOCS [static]

QString
NsHelpView::_opHtmlPath()
{
    return QString(qgetenv("HOME")) + QString("/.naiad");
}


// _opTypeHtmlPath
// ---------------
//! DOCS [static]

QString
NsHelpView::_opTypeHtmlPath(const QString &opType)
{
    return QString(_opHtmlPath() + "/ophelp." + opType + ".html");
}


// _defaultPath
// ------------
//! DOCS [static]

QString
NsHelpView::_defaultPath()
{
    return QString(fromNbStr(Nb::naiadPath()) + "/docs/html/index.html");
}

// -----------------------------------------------------------------------------

// NsHelpView
// ----------
//! CTOR.

NsHelpView::NsHelpView(QWidget *parent)
    : QWebView(parent)
    , _opPage(false)
{
    _loadDefault();
}

// -----------------------------------------------------------------------------

// onOpSelectionChanged
// --------------------
//! DOCS [slot]

void
NsHelpView::onOpSelectionChanged(const QStringList &opInstances,
                                 const bool         success)
{
    if (success && !opInstances.isEmpty()) {
        // Check if any Ops where selected.

        foreach (const QString &opInstance, opInstances) {
            const NsOpObject *op = 
                NsOpStore::instance()->queryConstOp(opInstance);

            if (0 != op) {
                if (op->isSelected()) {
                    // Found a newly selected Op. Show it!

                    _loadOpType(op->typeName());
                    break;  // Done!
                }
                else {
                    // Try removing Op in case the Op 
                    // being shown was unselected.

                    if (_loadedOpType == op->typeName()) {
                        _loadDefault();
                    }
                }
            }
        }        

        if (_loadedOpType.isEmpty()) {
            // Any other selected Ops?

            const QList<const NsOpObject*> selOps = 
                NsOpStore::instance()->constSelectedOps();
            if (!selOps.isEmpty()) {
                _loadOpType(selOps.first()->typeName());
            }
        }
    }
}


// onGraphCleared
// --------------
//! DOCS [slot]

void
NsHelpView::onGraphCleared(const bool success)
{
    qDebug() << "NsHelpView::onGraphCleared";
    if (success) {
        _loadDefault();
    }
}

// -----------------------------------------------------------------------------

// _createUrl
// ----------
//! DOCS [static]

QUrl
NsHelpView::_createUrl(const QString &path, const QString &scheme)
{
    QUrl url = QUrl::fromLocalFile(path);
    url.setScheme(scheme);
    return url;
}

void
NsHelpView::_loadOpType(const QString &typeName)
{
    _load(_createUrl(_opTypeHtmlPath(typeName)));
    _loadedOpType = typeName;
}

void
NsHelpView::_loadDefault()
{
    _load(_createUrl(_defaultPath()));
    _loadedOpType = QString();
}

void
NsHelpView::_load(const QUrl &url)
{
    //qDebug() << "Load URL: " << url.path();
    load(url);

#if 0
    qDebug() << "Path: " << url.path();
    qDebug() << "Scheme: " << url.scheme();
    qDebug() << "Valid URL: " << url.isValid();
    qDebug() << "Error: " << url.errorString();
#endif
}
