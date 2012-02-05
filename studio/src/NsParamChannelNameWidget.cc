// -----------------------------------------------------------------------------
//
// NsParamChannelNameWidget.cc
//
// Naiad Studio param <string | channel-name>, source file.
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

#include "NsParamChannelNameWidget.h"
#include "NsParamWidgetArgs.h"
#include "NsParamWidgetFactory.h"
#include "NsValueBaseObject.h"
#include "NsValueWidgetLayout.h"
#include "NsValueLabel.h"
#include "NsValueLineEdit.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include "NsBodyOutputPlugObject.h"

#include <NiNb.h>

// -----------------------------------------------------------------------------

//!< Key [static].
const NsValueWidgetKey 
NsParamChannelNameWidget::key(fromNbStr(Nb::Value1s::staticTypeName()), 
                              "channel-name");

// -----------------------------------------------------------------------------

// NsParamChannelNameWidget
// ------------------------
//! CTOR.

NsParamChannelNameWidget::NsParamChannelNameWidget(
        const NsParamWidgetArgs &args)
    : NsParamBaseWidget(args)
    , _vle(new NsValueLineEdit(*this))                        // Child.
{
    // TODO: validator?

    //initMetaMode(*_vle);
    _vle->onValueChanged();

    connect(this, SIGNAL(valueChanged()),
            _vle, SLOT(onValueChanged()));

    connect(_vle, SIGNAL(valueEdited(QString,int)),
            this, SLOT(onValueEdited(QString,int)));

    valueWidgetLayout()->addWidget(_vle, 1);
}


void
NsParamChannelNameWidget::addContextActions(QMenu &menu, const int comp)  const
{
    typedef NsBodyObject::ShapeInfo ShapeInfoType;
    typedef ShapeInfoType::ChannelInfo  ChannelInfoType;

    // Build sub-menu.

    menu.addSeparator();
    QMenu *channelNameMenu(menu.addMenu("Channel Names"));

    const QStringList upstreamOpNames(queryUpstreamOpNames(op()->longName()));

    QSet<QString> channelNameSet;

    const NtString subTypeName(fromQStr(valueBaseObject()->subTypeName()));
    const QString subTypeShape(fromNbStr(subTypeName.parent(":")));

    foreach (const QString &opInstance, upstreamOpNames) {
        const NsOpObject *op(NsOpStore::instance()->queryConstOp(opInstance));

        if (0 != op) {
            const QList<const NsBodyObject*> opBodies(op->constCachedBodies());
            foreach (const NsBodyObject *body, opBodies) {
                foreach (const ShapeInfoType &si, body->shapes()) {
                    if (subTypeShape.isEmpty() || subTypeShape == si.name()) {
                        foreach (const ChannelInfoType &ci, si.channels()) {
                            channelNameSet.insert(
                                queryChannelLongName(si.name(), ci.name()));
                        }
                    }
                }
            }

            foreach (const NsBodyOutputPlugObject *bopo,
                     op->constBodyOutputs()) {
                const QList<const NsBodyObject*> liveBodies(
                        bopo->constLiveBodies());
                foreach (const NsBodyObject *body, liveBodies) {
                    foreach (const ShapeInfoType &si, body->shapes()) {
                        if (subTypeShape.isEmpty() ||
                            subTypeShape == si.name()) {
                            foreach (const ChannelInfoType &ci, si.channels()) {
                                channelNameSet.insert(
                                    queryChannelLongName(si.name(), ci.name()));
                            }
                        }
                    }
                }
            }
        }
    }

    foreach (const QString &channelName, channelNameSet) {
        _addChannelNameAction(channelName, *channelNameMenu);
    }
}


// -----------------------------------------------------------------------------

// onChannelNameActionTriggered
// ----------------------------
//! [slot]

void
NsParamChannelNameWidget::onChannelNameActionTriggered()
{
    QAction *action(qobject_cast<QAction *>(sender()));

    if (0 != action) {
        onValueEdited(action->text());   // Replace.
    }
}

// -----------------------------------------------------------------------------

void
NsParamChannelNameWidget::_addChannelNameAction(const QString &text,
                                                QMenu         &menu) const
{
    QAction *action(new QAction(text, &menu));
    menu.addAction(action);
    connect(action, SIGNAL(triggered()),
            this,   SLOT(onChannelNameActionTriggered()));
}


// -----------------------------------------------------------------------------

//! Static creator instance.
const NsParamChannelNameWidget::_Creator
    NsParamChannelNameWidget::_creator(*NsParamWidgetFactory::instance());
