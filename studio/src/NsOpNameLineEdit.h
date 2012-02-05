// -----------------------------------------------------------------------------
//
// NsOpNameLineEdit.h
//
// Naiad Studio Op name line edit, header file.
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

#ifndef NS_OP_NAME_LINE_EDIT_H
#define NS_OP_NAME_LINE_EDIT_H

#include "NsLineEdit.h"

// -----------------------------------------------------------------------------

// NsOpNameLineEdit
// ----------------
//! Simple class for line edits containing Op names. Note that Op names are
//! not values, which is why it is inappropriate to use NsValueLineEdit.

class NsOpNameLineEdit : public NsLineEdit
{
    Q_OBJECT

public:

    //! CTOR.
    explicit
    NsOpNameLineEdit(const QString &text, QWidget *parent = 0)
        : NsLineEdit(text, parent)
    { 
        connect(this, 
                SIGNAL(editingFinished()), 
                SLOT(onEditingFinished())); 
    }

    //! DTOR.
    virtual 
    ~NsOpNameLineEdit() 
    {}

protected slots:

    void 
    onOpNameChanged(const QString &oldOpInstance,
                    const QString &newOpInstance)
    { 
        Q_UNUSED(oldOpInstance);
        setText(newOpInstance);
    }

    void 
    onEditingFinished()
    { emit opNameEdited(text()); }

signals:

    void 
    opNameEdited(const QString &newOpInstance);
};

#endif  // NS_OP_NAME_LINE_EDIT_H
