// -----------------------------------------------------------------------------
//
// NsStringUtils.cc
//
// Naiad Studio string utilities, source file.
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

#include "NsStringUtils.h"

// -----------------------------------------------------------------------------

// fromNbStr
// ---------
//! Convert a NtString to a QString.

QString
fromNbStr(const NtString &s)
{
    return QString(s.str().c_str());
}


// fromNbStrList
// -------------
//! Convert a NtStringList to a QStringList.

QStringList
fromNbStrList(const NtStringList &strList)
{
    QStringList qsl;

    typedef NtStringList::const_iterator IterType;

    const IterType iend(strList.end());
    for (IterType iter(strList.begin()); iter != iend; ++iter) {
        qsl += fromNbStr(*iter);
    }

    return qsl;
}


// fromNbStrSet
// ------------
//! Convert from NtStringSet to QStringSet.

QStringSet
fromNbStrSet(const NtStringSet &strSet)
{
    QStringSet qss;

    typedef NtStringSet::const_iterator IterType;

    const IterType iend(strSet.end());
    for (IterType iter(strSet.begin()); iter != iend; ++iter) {
        qss.insert(fromNbStr(*iter));
    }

    return qss;
}

// -----------------------------------------------------------------------------

// fromQStr
// --------
//! Convert a QString to a NtString.

NtString
fromQStr(const QString &s)
{
    return NtString(s.toAscii().data());
}


// fromQStrList
// ------------
//! Convert a QStringList to a NtStringList.

NtStringList
fromQStrList(const QStringList &strList)
{
    NtStringList ntsl;
    ntsl.reserve(strList.size());

    foreach (const QString &str, strList) {
        ntsl.push_back(fromQStr(str));
    }

    return ntsl;
}


// fromQStrSet
// -----------
//! Convert a QStringSet to a NtStringSet.

NtStringSet
fromQStrSet(const QStringSet &strSet)
{
    NtStringSet ntss;

    foreach (const QString &str, strSet) {
        ntss.insert(fromQStr(str));
    }

    return ntss;
}

// -----------------------------------------------------------------------------
