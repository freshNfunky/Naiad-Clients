// -----------------------------------------------------------------------------
//
// NsQuery.h
//
// Naiad Studio wrapper functions for Ni-queries, header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
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

#ifndef NS_QUERY_H
#define NS_QUERY_H

#include <NiTypes.h>
#include <QString>
#include <QStringList>
#include <QSet>

namespace Nb { class Body; }

// -----------------------------------------------------------------------------

/*!
A set of wrapper functions for Ni-queries. Wherever
possible exposure of Nt-types has been avoided. Naming of functions and
arguments match the corresponding Ni-functions.
NB: It is assumed that these functions never throw.
*/

// -----------------------------------------------------------------------------

// Parameter queries.

QStringList 
queryParamNames(const QString &name, const QString &sectionName);

QString 
queryParamTypeName(const QString &name, const QString &paramName);

QString 
queryParamSubTypeName(const QString &name, const QString &paramName);

QString 
queryParamLongName(const QString &opInstance, const QString &paramName);

QString 
queryParamEnumGroupName(const QString &queryType,
                        const QString &name,
                        const QString &paramName);

QStringList 
queryParamEnumItemNames(const QString &queryType,
                        const QString &name,
                        const QString &enumGroupName);
QString 
queryParam(const QString &paramLongName,
           int            comp = 0);

QString
queryParam(const QString &opInstance,
           const QString &paramName,
           int            comp = 0);

bool 
queryParamExists(const QString &paramLongName);

bool 
queryParamReadOnly(const QString &paramLongName);

bool 
queryParamHidden(const QString &paramLongName);

bool 
queryParamHidden(const QString &opInstance,
                 const QString &paramName);

int 
queryParamLimit1i(const QString &paramLongName, const QString &limitType);

float 
queryParamLimit1f(const QString &paramLongName, const QString &limitType);

float 
queryParamLimit3f(const QString &paramLongName,
                  const QString &limitType,
                  int            comp);

int 
queryParamHardMin1i(const QString &paramLongName);

int 
queryParamHardMax1i(const QString &paramLongName);

int 
queryParamSoftMin1i(const QString &paramLongName);

int 
queryParamSoftMax1i(const QString &paramLongName);

float 
queryParamHardMin1f(const QString &paramLongName);

float 
queryParamHardMax1f(const QString &paramLongName);

float 
queryParamSoftMin1f(const QString &paramLongName);

float 
queryParamSoftMax1f(const QString &paramLongName);

// -----------------------------------------------------------------------------

// Parameter evaluation.

float
evalParam1f(const QString &paramLongName, 
            const NtTimeBundle& tb = NiZeroTimeBundle);

float
evalParam3f(const QString &paramLongName, 
            int comp, 
            const NtTimeBundle& tb = NiZeroTimeBundle);

// TODO
//NtVec3f
//evalParam3f(const QString &paramLongName, double time = 0.);

int
evalParam1i(const QString &paramLongName, 
            const NtTimeBundle& tb = NiZeroTimeBundle);

QString
evalParam1s(const QString &paramLongName, 
            const NtTimeBundle& tb = NiZeroTimeBundle);

QString
evalParam1e(const QString &paramLongName, 
            const NtTimeBundle& tb = NiZeroTimeBundle);


bool
queryNumericConstantParam(const QString     &paramLongName,
                          const NtComponent  component=NI_X);

// -----------------------------------------------------------------------------

double
queryFrameTime(int frame);

NtTimeBundle
queryFrameTimeBundle(int frame);

NtTimeBundle
queryCurrentVisibleFrameTimeBundle();

NtTimeBundle
queryTimeBundle();

bool
queryIsFrameLive(const NtTimeBundle &ftb);

bool
queryIsFrameLive(int frame);

bool
queryStepped();

bool
queryStepping();


// -----------------------------------------------------------------------------

// Op queries.

QStringList
queryOpNames(const QString &queryType, 
             const QString &opClass, 
             bool includeHidden = true);

QStringList
queryOpTypeNames(bool includeHidden = true);

QStringList
queryBodyOpTypeNames(bool includeHidden = true);

QStringList
queryFieldOpTypeNames(bool includeHidden = true);

QStringList
queryBodyScopeTypeNames(bool includeHidden = true);

QStringList
queryFieldScopeTypeNames(bool includeHidden = true);

QStringList
queryOpMemberNames(const QString &queryType,
                   const QString &name,
                   const QString &member);

QStringList
queryOpInstanceSectionNames(const QString &opInstance);

QStringList
queryOpInstanceInputNames(const QString &opInstance);

QStringList
queryOpInstanceOutputNames(const QString &opInstance);

QStringList
queryOpInstanceCategories(const QString &opInstance);

QStringList
queryOpTypeSectionNames(const QString &opType);

QStringList
queryOpTypeInputNames(const QString &opType);

QStringList
queryOpTypeOutputNames(const QString &opType);

QStringList
queryOpTypeCategories(const QString &opType);

bool
queryOpInstanceExists(const QString &opInstance);

QString
queryOpTypeName(const QString &opInstance);

QString
queryOpClassName(const QString &opInstance);

QString
queryOpFamilyName(const QString &queryType, const QString &name);

QString
queryOpInstanceFamilyName(const QString &opInstance);

QString
queryOpTypeFamilyName(const QString &opInstance);

QString
queryOpState(const QString &opInstance);

QStringList
queryUpstreamOpNames(const QString &opInstance, bool includeRoot = false);

// -----------------------------------------------------------------------------

// Feed queries.

bool
queryValidFeed(const QString &inputLongName,
               const QString &plugLongName);

void
queryFeeds(const QString &opInstance,
           QSet<QString> &feeds,
           bool           includeDummies = true);

int
queryDummyFeedCount(const QString &plugLongName,
                    bool          includeDownstreamInputs = true);

QString
queryFeedingBodyOutput(const QString& plugLongName);

// -----------------------------------------------------------------------------

// Plug queries.

//bool
//queryIsGroupPlug(const QString &opType, const QString &plugName);

bool
queryIsGroupPlug(const QString &queryType, 
                 const QString &name, 
                 const QString &plugName);

bool
queryOpInstanceIsGroupPlug(const QString &opInstance, const QString &plugName);

bool
queryOpTypeIsGroupPlug(const QString &opType, const QString &plugName);

QString
queryPlugLongName(const QString &opInstance, const QString &plugName);

QString
queryPlugSignatureName(const QString &opType, const QString &plugName);

// HACK: Fake query!
QString
queryPlugLongName(const QString &bodyLongName, QString *bodyName = 0);

// HACK: Fake query!
QString
queryPlugOpInstance(const QString &plugLongName, QString *plugName = 0);

// -----------------------------------------------------------------------------

// Version queries.

QString
queryVersion();

QString
queryVersionString();

// -----------------------------------------------------------------------------

// Meta queries.

QString
queryMeta(const QString &longName,
          const QString &valueType);

QString
queryMeta(const QString &opInstance,
          const QString &paramName,
          const QString &valueType);

// Global meta values.

QString
queryProjectPath();

// Visible frame range.

bool
queryFirstVisibleFrame(int *fvf);

bool
queryLastVisibleFrame(int *fvf);

bool
queryCurrentVisibleFrame(int *cvf);

// -----------------------------------------------------------------------------

// Body queries.

QList<Nb::Body*>
queryCachedBodies(const QString      &opInstance, 
                  const NtTimeBundle &tb, 
                  bool                applyBodyNamePattern);

QStringList
queryCachedBodyNames(const QString      &opInstance, 
                     const NtTimeBundle &tb, 
                     bool                applyBodyNamePattern = false);

bool
initCachedBodies(const QString      &opInstance,
                 const NtTimeBundle &tb,
                 bool                applyBodyNamePattern,
                 bool               *ok = 0);

//! HACK: Fake query.
QString 
queryBodyName(const QString &bodyLongName, QString *plugLongName = 0);

//! HACK: Fake query.
QString 
queryChannelLongName(const QString &shapeName,
                     const QString &channelName);

//QString queryBodyLongName(const QString &plugLongName,
//                          const QString &bodyName);

QString queryBodyLongName(const QString &opInstance,
                          const QString &plugName,
                          const QString &bodyName);

// -----------------------------------------------------------------------------

#endif // NS_QUERY_H
