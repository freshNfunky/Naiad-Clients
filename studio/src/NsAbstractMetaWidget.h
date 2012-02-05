// -----------------------------------------------------------------------------
//
// NsAbstractMetaWidget.h
//
// Naiad Studio abstract meta widget, header file.
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

#ifndef NS_ABSTRACT_META_WIDGET_H
#define NS_ABSTRACT_META_WIDGET_H

#include "NsValueWidgetLayout.h"
#include "NsValueWidgetArgs.h"
#include "NsValueObject.h"
#include "NsValueBaseObject.h"
#include "NsUndoStack.h"
#include "NsCmdSetMeta.h"
#include "NsOpStore.h"
#include "NsQuery.h"
#include "NsMetaLabel.h"
#include "NsMetaCheckBox.h"
#include "NsMetaLineEdit.h"
#include <QWidget>
#include <QDebug>

// -----------------------------------------------------------------------------

class NsMetaWidgetArgs : public NsValueWidgetArgs
{
public:

    //! DTOR.
    virtual
    ~NsMetaWidgetArgs()
    {}

public:

    explicit
    NsMetaWidgetArgs(NsValueObject     *vo,
                     NsValueBaseObject *vbo,
                     const QString     &valueType,
                     NsUndoStack    *undoStack,
                     QWidget           *parent = 0)
        : NsValueWidgetArgs(vo, vbo, parent)
        , _valueType(valueType)
        , _undoStack(undoStack)
    {}


    const QString&
    valueType() const
    { return _valueType; }


    NsUndoStack*
    undoStack() const
    { return _undoStack; }

private:    // Member variables.

    QString                  _valueType;
    NsUndoStack          *_undoStack; //!< Undo stack used with commands.
    QWidget                 *_parent;
};

// -----------------------------------------------------------------------------

// NsAbstractMetaWidget
// --------------------
//! Abstract base for widgets representing meta values visually
//! in the value editor.

class NsAbstractMetaWidget : public QWidget
{
    Q_OBJECT

public:

    virtual
    ~NsAbstractMetaWidget() = 0;     // Force class to be abstract.

    const NsValueBaseObject*
    valueBase() const
    { return _vbo; }

    //! May return null.
    const NsValueObject&
    valueObject() const
    { return *_vo; }

    const QString &
    valueType() const
    { return _valueType; }

public slots:

    void
    onMetaChanged(const QString &longName,
                  const QString &valueType,
                  const QString &value,
                  const bool     success)
    {
        qDebug() << "NsAbstractMetaWidget::onMetaChanged";

        if (success &&
            this->valueType() == valueType &&
            this->longName() == longName) {
            emit valueChanged(value);
        }
    }

signals:

    void valueChanged(const QString &newValue);

protected:

    explicit
    NsAbstractMetaWidget(const NsMetaWidgetArgs &args)
        : QWidget(args.parent())
        , _vo(args.valueObject())
        , _vbo(args.valueBaseObject()) // May be null.
        , _valueType(args.valueType())
        , _undoStack(args.undoStack())
        , _vwl(new NsValueWidgetLayout)
    {
        QString x = _valueType;
        if(x=="Visible in 3D")
            x="Visible";
        NsMetaLabel *ml(new NsMetaLabel(x, this)); // Child.
        _vwl->addWidget(ml, 0, Qt::AlignRight);
        setLayout(_vwl);
    }

    NsUndoStack*
    undoStack()
    { return _undoStack; }

    NsValueWidgetLayout*
    valueWidgetLayout() const
    { return _vwl; }

    QString
    longName() const
    {
        return (0 != valueBase()) ? valueBase()->longName() :
                                    valueObject().longName();
    }

    bool
    value(QString *val) const
    {
        if (0 != valueBase()) {
            return valueBase()->metaValue(valueType(), val);
        }

        return valueObject().metaValue(valueType(), val);
    }

protected slots:

    virtual void
    onValueEdited(const QString &newValue)
    {
        if (0 != undoStack()) {
            NsCmdSetMeta::exec(longName(),
                               valueType(),
                               newValue,
                               *undoStack());
        }
        else {
            NsCmdSetMeta::exec(longName(),
                               valueType(),
                               newValue);
        }
    }

private:    // Member variables.

    const NsValueObject     *_vo;        //!< Guaranteed non-null.
    const NsValueBaseObject *_vbo;       //!< May be null.
    QString                  _valueType;
    NsUndoStack          *_undoStack; //!< Undo stack used with commands.
    NsValueWidgetLayout     *_vwl;       //!< Layout.

private:

    NsAbstractMetaWidget(const NsAbstractMetaWidget&);           //!< Disabled.
    NsAbstractMetaWidget& operator=(const NsAbstractMetaWidget&);//!< Disabled.
};

// -----------------------------------------------------------------------------

class NsMetaCheckBoxWidget : public NsAbstractMetaWidget
{
    Q_OBJECT

public:

    explicit
    NsMetaCheckBoxWidget(const NsMetaWidgetArgs &args)
        : NsAbstractMetaWidget(args)
    {
        QString val;
        value(&val);    // Ignore success.
        NsMetaCheckBox *mcb(new NsMetaCheckBox(val, "On", "Off", this));// Child

        connect(mcb,  SIGNAL(valueEdited(QString)),
                this, SLOT(onValueEdited(QString)));
        connect(this, SIGNAL(valueChanged(QString)),
                mcb,  SLOT(onValueChanged(QString)));

        valueWidgetLayout()->addWidget(mcb, 1);
    }

    //! DTOR.
    virtual
    ~NsMetaCheckBoxWidget()
    {}
};


class NsMetaLineEditWidget : public NsAbstractMetaWidget
{
    Q_OBJECT

public:

    explicit
    NsMetaLineEditWidget(const NsMetaWidgetArgs &args)
        : NsAbstractMetaWidget(args)
    {
        QString val;
        value(&val);    // Ignore success.
        NsMetaLineEdit *mle(new NsMetaLineEdit(val, this)); // Child.

        connect(mle,  SIGNAL(valueEdited(QString)),
                this, SLOT(onValueEdited(QString)));
        connect(this, SIGNAL(valueChanged(QString)),
                mle,  SLOT(onValueChanged(QString)));

        valueWidgetLayout()->addWidget(mle, 1);
    }

    //! DTOR.
    virtual
    ~NsMetaLineEditWidget()
    {}
};

//class NsMetaLabelWidget : public NsAbstractMetaWidget
//{};

//class NsMetaComboBoxWidget : public NsAbstractMetaWidget
//{};

//class NsMetaButtonWidget : public NsAbstractMetaWidget
//{};

#endif // NS_ABSTRACT_META_WIDGET_H
