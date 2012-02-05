// -----------------------------------------------------------------------------
//
// NsOutputPlugObject.h
//
// Naiad Studio class for representing a Ng::Output, header file.
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

#ifndef NS_OUTPUT_PLUG_OBJECT_H
#define NS_OUTPUT_PLUG_OBJECT_H

#include "NsPlugObject.h"
#include <NgOutput.h>   // Resource type.

class NsOpObject;       // Parent type.
namespace Ng { class Input; }

// -----------------------------------------------------------------------------

// NsOutputPlugObject
// ------------------
//! Abstract base class for objects representing output plugs.

class NsOutputPlugObject : public NsPlugObject
{
    Q_OBJECT

public:     // Plug object interface.

    virtual
    ~NsOutputPlugObject() = 0;    // Force abstract.

    //! Covariant return type.
    virtual const Ng::Output*
    constNgPlug() const
    { return _output; }

    //! Covariant return type.
    virtual Ng::Output*
    mutableNgPlug() 
    { return _output; }

public:     // Output.

    const Ng::Output*
    constOutput() const
    { return _output; }

    Ng::Output*
    mutableOutput()
    { return _output; }

public:     // Inputs.

    int
    downstreamInputCount() const
    { return _output->downstreamInputCount(); }

    Ng::Input*
    downstreamInput(const int i)
    { return _output->downstreamInput(i); }

    const Ng::Input*
    downstreamInput(const int i) const
    { return _output->downstreamInput(i); } 

    QList<Ng::Input*>
    downstreamInputs();

protected:

    explicit
    NsOutputPlugObject(Ng::Output &output, NsOpObject &parent);

protected:

    virtual void
    groupPlugChanged() = 0;

private:    // Member variables.

    Ng::Output *_output;  //!< Server resource.
};

// -----------------------------------------------------------------------------

#endif // NS_OUTPUT_PLUG_OBJECT_H
