// -----------------------------------------------------------------------------
//
// NsQuery.cc
//
// Naiad Studio wrapper functions for Ni-queries, source file.
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

#include "NsQuery.h"
#include "NsStringUtils.h"
#include <QDebug>
#include <Ni.h>     // TODO: NiQuery.h?
#include <NiNb.h>
#include <NbBody.h>

// -----------------------------------------------------------------------------

namespace {
//! Separator character in body long names.
const QString _bodyLongNameSeparator("~");

//! Separator character in plug long names.
const QString _plugLongNameSeparator(":");
}   // Namespace: anonymous.

// -----------------------------------------------------------------------------

// Parameter queries.

// queryParamNames
// ----------------
//! Provides a list of Op parameter names for a section.

QStringList
queryParamNames(const QString &name, const QString &sectionName)
{
    // Assume successful query.

    return fromNbStrList(
        NiQueryParamNames(
            NI_INSTANCE,            // Hard-coded!
            fromQStr(name),
            fromQStr(sectionName)));
}


// queryParamTypeName
// ------------------
//! Get the parameter type name.

QString
queryParamTypeName(const QString &name, const QString &paramName)
{
    // Assume successful query.

    return fromNbStr(
        NiQueryParamTypeName(
            NI_INSTANCE,            // Hard-coded!
            fromQStr(name),
            fromQStr(paramName)));
}


// queryParamSubTypeName
// ---------------------
//! Get the parameter sub-type name.

QString
queryParamSubTypeName(const QString &name, const QString &paramName)
{
    // Assume successful query.

    return fromNbStr(
        NiQueryParamSubTypeName(
            NI_INSTANCE,            // Hard-coded!
            fromQStr(name),
            fromQStr(paramName)));
}


// queryParamLongName
// ------------------
//! Get parameter long name.

QString
queryParamLongName(const QString &opInstance, const QString &paramName)
{
    // Assume successful query.

    return fromNbStr(
        NiQueryParamLongName(
            fromQStr(opInstance),
            fromQStr(paramName)));
}


// queryParamEnumGroupName
// -----------------------
//! Get enum group name.

QString
queryParamEnumGroupName(const QString &queryType,
                        const QString &name,
                        const QString &paramName)
{
    // Assume successful query.

    return fromNbStr(
        NiQueryParamEnumGroupName(
            fromQStr(queryType), //NI_INSTANCE,            // Hard-coded!
            fromQStr(name),
            fromQStr(paramName)));
}


// queryEnumItemNames
// ------------------
//! Get enum item names.

QStringList
queryParamEnumItemNames(const QString &queryType,
                        const QString &name,
                        const QString &enumGroupName)
{
    // Assume successful query.

    return fromNbStrList(
        NiQueryEnumItemNames(
            fromQStr(queryType), //NI_INSTANCE,            // Hard-coded!
            fromQStr(name),
            fromQStr(enumGroupName)));
}


// queryParam
// ----------
//! Get the current value of a parameter on the server.

QString
queryParam(const QString     &paramLongName,
           const NtComponent  component)
{
    // Assume successful query.

    return fromNbStr(NiQueryParam(fromQStr(paramLongName), component));
}


// queryParam
// ----------
//! Convenience.

QString
queryParam(const QString &opInstance,
           const QString &paramName,
           const int      comp)
{
    // Assume successful query.

    return fromNbStr(
        NiQueryParam(
            fromQStr(queryParamLongName(opInstance, paramName)), 
            comp));
}


// queryParamExists
// ----------------
//! True if the parameter exists.

bool
queryParamExists(const QString &paramLongName)
{
    return (NI_TRUE == NiQueryParamExists(fromQStr(paramLongName)));
}


// queryParamReadOnly
// ------------------
//! True if the parameter is read-only.

bool
queryParamReadOnly(const QString &paramLongName)
{
    // Assume successful query.

    return (NI_TRUE == NiQueryParamReadOnly(fromQStr(paramLongName)));
}


// queryParamHidden
// ----------------
//! True if the parameter is hidden.

bool
queryParamHidden(const QString &paramLongName)
{
    // Assume successful query.

    return (NI_TRUE == NiQueryParamHidden(fromQStr(paramLongName)));
}


// queryParamHidden
// ----------------
//! Convenience.

bool 
queryParamHidden(const QString &opInstance,
                 const QString &paramName)
{
    return queryParamHidden(queryParamLongName(opInstance, paramName));
}


// queryParamLimit1i
// -----------------
//! Returns an integer parameter limit depending on the limit type.

int
queryParamLimit1i(const QString &paramLongName, const QString &limitType)
{
    return NiQueryParamLimit1i(fromQStr(paramLongName), fromQStr(limitType));
}


// queryParamLimit1f
// -----------------
//! Returns a float parameter limit depending on the limit type.

float
queryParamLimit1f(const QString &paramLongName, const QString &limitType)
{
    return NiQueryParamLimit1f(fromQStr(paramLongName), fromQStr(limitType));
}


// queryParamLimit3f
// -----------------
//! Returns a float vector parameter limit depending on the limit type.

float
queryParamLimit3f(const QString &paramLongName,
                  const QString &limitType,
                  const int      comp)
{
    return NiQueryParamLimit3f(fromQStr(paramLongName),
                               fromQStr(limitType),
                               comp);
}


// queryParamHardMin1i
// -------------------
//! Convenience.

int
queryParamHardMin1i(const QString &paramLongName)
{
    return NiQueryParamLimit1i(fromQStr(paramLongName),NI_PARAM_RANGE_HARD_MIN);
}


// queryParamHardMax1i
// -------------------
//! Convenience.

int
queryParamHardMax1i(const QString &paramLongName)
{
    return NiQueryParamLimit1i(fromQStr(paramLongName),NI_PARAM_RANGE_HARD_MAX);
}


// queryParamSoftMin1i
// -------------------
//! Convenience.

int
queryParamSoftMin1i(const QString &paramLongName)
{
    return NiQueryParamLimit1i(fromQStr(paramLongName),NI_PARAM_RANGE_SOFT_MIN);
}


// queryParamSoftMax1i
// -------------------
//! Convenience.

int
queryParamSoftMax1i(const QString &paramLongName)
{
    return NiQueryParamLimit1i(fromQStr(paramLongName),NI_PARAM_RANGE_SOFT_MAX);
}


// queryParamHardMin1f
// -------------------
//! Convenience.

float
queryParamHardMin1f(const QString &paramLongName)
{
    return NiQueryParamLimit1f(fromQStr(paramLongName),NI_PARAM_RANGE_HARD_MIN);
}


// queryParamHardMax1f
// -------------------
//! Convenience.

float
queryParamHardMax1f(const QString &paramLongName)
{
    return NiQueryParamLimit1f(fromQStr(paramLongName),NI_PARAM_RANGE_HARD_MAX);
}


// queryParamSoftMin1f
// -------------------
//! Convenience.

float
queryParamSoftMin1f(const QString &paramLongName)
{
    return NiQueryParamLimit1f(fromQStr(paramLongName),NI_PARAM_RANGE_SOFT_MIN);
}


// queryParamSoftMax1f
// -------------------
//! Convenience.

float
queryParamSoftMax1f(const QString &paramLongName)
{
    return NiQueryParamLimit1f(fromQStr(paramLongName),NI_PARAM_RANGE_SOFT_MAX);
}

// -----------------------------------------------------------------------------

// Parameter evaluation.

// evalParam1f
// -----------
//! Evaluate a float parameter at the given time.

float
evalParam1f(const QString &paramLongName, const NtTimeBundle& tb)
{
    return NiEvalParam1f(fromQStr(paramLongName), tb);
}


// evalParam1f
// -----------
//! Evaluate a component of a float vector parameter at the given time.

float
evalParam3f(const QString&      paramLongName,
            const int           comp,
            const NtTimeBundle& tb)
{
    return NiEvalParam3f(fromQStr(paramLongName), comp, tb);
}


// evalParam1i
// -----------
//! Evaluate a integer parameter at the given time.

int
evalParam1i(const QString&      paramLongName, 
            const NtTimeBundle& tb)
{
    return NiEvalParam1i(fromQStr(paramLongName), tb);
}


// evalParam1s
// -----------
//! Evaluate a string parameter at the given time.

QString
evalParam1s(const QString&      paramLongName,
            const NtTimeBundle& tb)
{
    return fromNbStr(NiEvalParam1s(fromQStr(paramLongName), tb));
}


// evalParam1e
// -----------
//! Evaluate an enum parameter at the given time.

QString
evalParam1e(const QString &paramLongName, const NtTimeBundle& tb)
{
    return fromNbStr(NiEvalParam1e(fromQStr(paramLongName), tb));
}


bool
queryNumericConstantParam(const QString     &paramLongName,
                          const NtComponent  component)
{
    return (NI_TRUE == NiQueryNumericConstantParam(fromQStr(paramLongName),
                                                   component));
}

// -----------------------------------------------------------------------------

// queryFrameTime
// --------------
//! DOCS

double
queryFrameTime(const int frame)
{
    return queryFrameTimeBundle(frame).time;
}


// queryFrameTimeBundle
// --------------------
//! DOCS

NtTimeBundle
queryFrameTimeBundle(const int frame)
{
    NtTimeBundle tb(frame, 0, 0, 0, 0, 0, true);
    const double frame_dt = 1./NiEvalParam1i("Global.Fps", tb);
    tb.frame_dt=frame_dt;
    tb.dt=frame_dt;
    tb.time=frame*frame_dt;
    return tb;
}


// queryCurrentFrameTimeBundle
// ---------------------------
//! DOCS

NtTimeBundle
queryCurrentVisibleFrameTimeBundle()
{
    int cvf(1);
    queryCurrentVisibleFrame(&cvf); // Ignore success.
    return queryFrameTimeBundle(cvf);
}


// queryTimeBundle
// ---------------
//! Current server time.

NtTimeBundle
queryTimeBundle()
{
    return NiQueryTimeBundle();
}


// queryIsFrameLive
// ----------------
//! DOCS

bool
queryIsFrameLive(const NtTimeBundle &ftb)
{
    const NtTimeBundle cstb = NiQueryTimeBundle();   // Current server time.
    return (cstb.frame == ftb.frame && queryStepped());
}


// queryIsFrameLive
// ----------------
//! DOCS

bool
queryIsFrameLive(const int frame)
{
    return queryIsFrameLive(queryFrameTimeBundle(frame));
}


// queryStepped
// ------------
//! DOCS

bool
queryStepped()
{
    return (NI_TRUE == NiQueryStepped());
}


// queryStepping
// -------------
//! DOCS

bool
queryStepping()
{
    return (NI_TRUE == NiQueryStepping());
}

// -----------------------------------------------------------------------------

// Op queries.

// queryOpNames
// ------------
//! TODO.

QStringList
queryOpNames(const QString &queryType, 
             const QString &opClass, 
             const bool     includeHidden)
{
    return fromNbStrList(
        NiQueryOpNames(fromQStr(queryType), fromQStr(opClass), includeHidden));
}


// queryOpTypeNames
// ----------------
//! Convenience.

QStringList
queryOpTypeNames(const bool includeHidden)
{
    return queryOpNames(fromNbStr(NI_TYPE),
                        fromNbStr(NI_OP),
                        includeHidden);
}


// queryBodyOpTypeNames
// --------------------
//! Convenience.

QStringList
queryBodyOpTypeNames(const bool includeHidden)
{
    return queryOpNames(fromNbStr(NI_TYPE),
                        fromNbStr(NI_BODY_OP),
                        includeHidden);
}


// queryFieldOpTypeNames
// ---------------------
//! Convenience.

QStringList
queryFieldOpTypeNames(const bool includeHidden)
{
    return queryOpNames(fromNbStr(NI_TYPE),
                        fromNbStr(NI_FIELD_OP),
                        includeHidden);
}


// queryBodyScopeTypeNames
// -----------------------
//! Convenience.

QStringList
queryBodyScopeTypeNames(const bool includeHidden)
{
    return queryOpNames(fromNbStr(NI_TYPE),
                        fromNbStr("BODY_SCOPE"),
                        includeHidden);
}


// queryFieldScopeTypeNames
// ------------------------
//! Convenience.

QStringList
queryFieldScopeTypeNames(const bool includeHidden)
{
    return queryOpNames(fromNbStr(NI_TYPE),
                        fromNbStr("FIELD_SCOPE"),
                        includeHidden);
}


// queryOpMemberNames
// ------------------
//! DOCS
//! 'queryType' = NI_TYPE | NI_INSTANCE
//! 'name'      = Op name.
//! 'member'    = NI_OUTPUT | NI_INPUT

QStringList
queryOpMemberNames(const QString &queryType,
                   const QString &name,
                   const QString &member)
{
    // Assume successful query.

    return fromNbStrList(
        NiQueryOpMemberNames(fromQStr(queryType),
                             fromQStr(name),
                             fromQStr(member)));
}


// queryOpInstanceSectionNames
// ---------------------------
//! Convenience. Provides a list of sections for an Op instance.

QStringList
queryOpInstanceSectionNames(const QString &opInstance)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_INSTANCE),
                              opInstance,
                              fromNbStr(NI_PARAM_SECTION));
}


// queryOpInstanceInputNames
// -------------------------
//! Convenience. Provides a list of inputs for an Op instance.

QStringList
queryOpInstanceInputNames(const QString &opInstance)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_INSTANCE),
                              opInstance,
                              fromNbStr(NI_INPUT));
}


// queryOpInstanceOutputNames
// --------------------------
//! Convenience. Provides a list of outputs for an Op instance.

QStringList
queryOpInstanceOutputNames(const QString &opInstance)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_INSTANCE),
                              opInstance,
                              fromNbStr(NI_OUTPUT));
}


// queryOpInstanceCategories
// -------------------------
//! Convenience. Returns the categories to which the given Op instance belongs.

QStringList
queryOpInstanceCategories(const QString &opInstance)
{
    return queryOpMemberNames(fromNbStr(NI_INSTANCE),
                              opInstance,
                              fromNbStr(NI_CATEGORY));
}


// queryOpTypeSectionNames
// -----------------------
//! Provides a list of sections for an Op type.

QStringList
queryOpTypeSectionNames(const QString &opType)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_TYPE),
                              opType,
                              fromNbStr(NI_PARAM_SECTION));
}


// queryOpTypeInputNames
// ---------------------
//! Convenience. Provides a list of inputs for an Op type.

QStringList
queryOpTypeInputNames(const QString &opType)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_TYPE),
                              opType,
                              fromNbStr(NI_INPUT));
}


// queryOpTypeOutputNames
// ----------------------
//! Convenience. Provides a list of outputs for an Op type.

QStringList
queryOpTypeOutputNames(const QString &opType)
{
    // Assume successful query.

    return queryOpMemberNames(fromNbStr(NI_TYPE),
                              opType,
                              fromNbStr(NI_OUTPUT));
}


// queryOpTypeCategories
// ---------------------
//! Returns a list of the categories to which the given Op type belongs.

QStringList
queryOpTypeCategories(const QString &opType)
{
    return queryOpMemberNames(fromNbStr(NI_TYPE),
                              opType,
                              fromNbStr(NI_CATEGORY));
}


// queryOpInstanceExists
// ---------------------
//! Returns true if the Op instance exists.

bool
queryOpInstanceExists(const QString &opInstance)
{
    return (NI_TRUE == NiQueryOpInstanceExists(fromQStr(opInstance)));
}


// queryOpTypeName
// ---------------
//! Query the type of an Op given the Op's name.

QString
queryOpTypeName(const QString &opInstance)
{
    // Assume successful query.

    return fromNbStr(NiQueryTypeName(fromQStr(opInstance)));
}


// queryOpClassName
// ----------------
//! Query the class of an Op given the Op's name.

QString
queryOpClassName(const QString &opInstance)
{
    // Assume successful query.

    return fromNbStr(NiQueryClassName(fromQStr(opInstance)));
}


// queryOpFamilyName
// -----------------
//! Query the family name of an Op given the type or name of the Op.

QString
queryOpFamilyName(const QString &queryType, const QString &name)
{
    // Assume successful query.

    return fromNbStr(NiQueryFamilyName(fromQStr(queryType), fromQStr(name)));
}


// queryOpInstanceFamilyName
// -------------------------
//! Query the family name of an Op instance.

QString
queryOpInstanceFamilyName(const QString &opInstance)
{
    return queryOpFamilyName(fromNbStr(NI_INSTANCE), opInstance);
}


// queryOpTypeFamilyName
// ---------------------
//! Query the family name of an Op type.

QString
queryOpTypeFamilyName(const QString &opType)
{
    return queryOpFamilyName(fromNbStr(NI_TYPE), opType);
}


// queryOpState
// ------------
//! Query the state of an Op.

QString
queryOpState(const QString &opInstance)
{
    // Assume successful query.

    return fromNbStr(NiQueryOpState(fromQStr(opInstance)));
}


// queryUpstreamOpNames
// --------------------
//! DOCS

QStringList
queryUpstreamOpNames(const QString &opInstance, const bool includeRoot)
{
    return fromNbStrList(
            NiQueryUpstreamOpNames(fromQStr(opInstance), includeRoot));
}

// -----------------------------------------------------------------------------

// Feed queries.

// queryValidFeed
// --------------
//! Check if a feed is valid.

bool
queryValidFeed(const QString &inputLongName, const QString &plugLongName)
{
    return (NI_TRUE == NiQueryValidFeed(fromQStr(inputLongName),
                                        fromQStr(plugLongName)));
}


// queryFeeds
// ----------
//! DOCS.

void
queryFeeds(const QString &opInstance,
           QSet<QString> &feeds,
           const bool     includeDummies)
{
    NtStringSet ntStrSet;
    NiQueryFeeds(fromQStr(opInstance), ntStrSet, includeDummies);
    feeds += fromNbStrSet(ntStrSet);
}


// queryDummyFeedCount
// -------------------
//! DOCS

int
queryDummyFeedCount(const QString &plugLongName,
                    const bool     includeDownstreamInputs)
{
    return NiQueryDummyFeedCount(fromQStr(plugLongName),
                                 includeDownstreamInputs);
}


// queryFeedingBodyOutput
// ----------------------
//! DOCS

QString
queryFeedingBodyOutput(const QString &plugLongName)
{
    return fromNbStr(NiQueryFeedingBodyOutput(fromQStr(plugLongName)));
}

// -----------------------------------------------------------------------------

// Plug queries.

// queryIsGroupPlug
// ----------------
//! True if plug is group.

bool
queryIsGroupPlug(const QString &queryType, 
                 const QString &name, 
                 const QString &plugName)
{
    return (NI_TRUE == NiQueryIsGroupPlug(fromQStr(queryType), 
                                          fromQStr(name), 
                                          fromQStr(plugName)));
}

bool
queryOpInstanceIsGroupPlug(const QString &opInstance, const QString &plugName)
{
    return queryIsGroupPlug(fromNbStr(NI_INSTANCE), opInstance, plugName);
}

bool
queryOpTypeIsGroupPlug(const QString &opType, const QString &plugName)
{
    return queryIsGroupPlug(fromNbStr(NI_TYPE), opType, plugName);
}


// queryPlugLongName
// -----------------
//! Returns the plug long name.

QString
queryPlugLongName(const QString &opInstance, const QString &plugName)
{
    return fromNbStr(NiQueryPlugLongName(fromQStr(opInstance),
                                         fromQStr(plugName)));
}


// queryPlugSignatureName
// ----------------------
//! Returns the plug signature name.

QString
queryPlugSignatureName(const QString &opType, const QString &plugName)
{
    return fromNbStr(NiQueryPlugSignatureName(fromQStr(opType), 
                                              fromQStr(plugName)));
}


// queryPlugLongName
// -----------------
//! Extracts the plug long name from a body long name.

QString
queryPlugLongName(const QString &bodyLongName, QString *bodyName)
{
    const int index(bodyLongName.indexOf(_bodyLongNameSeparator));

    QString plugLongName("");

    if (-1 != index) {
        // Separator found. There is at least one separator.

        plugLongName = bodyLongName.left(index);

        if (0 != bodyName) {
            *bodyName = bodyLongName.right(bodyLongName.length() - index - 1);
        }
    }

    return plugLongName;
}


// queryPlugOpInstance
// -------------------
//! Extracts the Op name from a plug long name. Returns an empty string
//! the provided plug long name is not valid.

QString
queryPlugOpInstance(const QString &plugLongName, QString *plugName)
{
    const int index = plugLongName.indexOf(_plugLongNameSeparator);
    QString opInstance;
    if (index != -1) {
        // Separator found. There is at least one separator.

        opInstance = plugLongName.left(index);
        if (0 != plugName) {
            *plugName = plugLongName.right(plugLongName.length() - index - 1);
        }
    }
    return opInstance;
}

// -----------------------------------------------------------------------------

// Version queries.

// queryVersion
// ------------
//! Get the Naiad version string.

QString
queryVersion()
{
    return fromNbStr(NiQueryVersion());
}


QString
queryVersionString()
{
    return fromNbStr(NIVERSION_STR);
}

// -----------------------------------------------------------------------------

// queryMeta
// ---------
//! Get meta value.

QString
queryMeta(const QString &longName,
          const QString &valueType)
{
    // Assume successful query.

    return fromNbStr(NiQueryMeta(fromQStr(longName), fromQStr(valueType)));
}


// queryMeta
// ---------
//! Convenience. Get meta value.

QString
queryMeta(const QString &opInstance, 
          const QString &paramName, 
          const QString &valueType)
{
    // Assume successful query.

    return queryMeta(queryParamLongName(opInstance, paramName), valueType);
}


// queryProjectPath
// ----------------
//! DOCS

QString
queryProjectPath()
{
    return evalParam1s("Global.Project Path");
}


// queryFirstVisibleFrame
// ----------------------
//! DOCS

bool
queryFirstVisibleFrame(int *fvf)
{
    const QString mv(queryMeta("Global", "First Visible Frame"));

    bool ok(false);
    const int i(mv.toInt(&ok));

    if (ok && 0 != fvf) {
        *fvf = i;
    }

    return ok;
}


// queryLastVisibleFrame
// ---------------------
//! DOCS

bool
queryLastVisibleFrame(int *lvf)
{
    const QString mv(queryMeta("Global", "Last Visible Frame"));

    bool ok(false);
    const int i(mv.toInt(&ok));

    if (ok && 0 != lvf) {
        *lvf = i;
    }

    return ok;
}


// queryCurrentVisibleFrame
// ------------------------
//! DOCS

bool
queryCurrentVisibleFrame(int *cvf)
{
    const QString mv(queryMeta("Global", "Current Visible Frame"));

    bool ok(false);
    const int i(mv.toInt(&ok));

    if (ok && 0 != cvf) {
        *cvf = i;
    }

    return ok;
}

// -----------------------------------------------------------------------------

// Body queries.

// queryCachedBodies
// -----------------
//! Caller is responsible for deleting body resources!

QList<Nb::Body*>
queryCachedBodies(const QString      &opInstance, 
                  const NtTimeBundle &tb, 
                  bool                applyBodyNamePattern)
{
    typedef std::vector<Nb::Body*> BodyVectorType;
    typedef BodyVectorType::const_iterator BodyIterType;

    QList<Nb::Body*> bodyList;
    initCachedBodies(opInstance, tb, applyBodyNamePattern); // Must call!
    const BodyVectorType bodies(NiCachedBodies(fromQStr(opInstance), tb));
    const BodyIterType iend(bodies.end());
    for (BodyIterType iter(bodies.begin()); iter != iend; ++iter) {
        bodyList.append(*iter);
    }

    return bodyList;        
}


// queryCachedBodyNames
// --------------------
//! DOCS

QStringList
queryCachedBodyNames(const QString      &opInstance, 
                     const NtTimeBundle &tb, 
                     const bool          applyBodyNamePattern)
{
    return fromNbStrList(
        NiQueryCachedBodyNames(fromQStr(opInstance), applyBodyNamePattern, tb));

//    const QList<Nb::Body*> bodyList =
//        queryCachedBodies(opInstance, tb, applyBodyNamePattern);
//    QStringList bodyNameList;
//    foreach (const Nb::Body* nbBody, bodyList) {
//        bodyNameList.append(fromNbStr(nbBody->name()));
//        delete nbBody;
//    }
//    return bodyNameList;
}


// queryUpdateCachedBodies
// -----------------------
//! DOCS

bool
initCachedBodies(const QString      &opInstance,
                      const NtTimeBundle &tb,
                      const bool          applyBodyNamePattern,
                      bool               *ok)
{
    NtBool ntOk(false);
    const bool result(
        NiInitCachedBodies(fromQStr(opInstance),
                           tb,
                           applyBodyNamePattern,
                           &ntOk));

    if (0 != ok) {
        *ok = (ntOk == NI_TRUE);
    }

    return result;
}



// queryBodyName
// -------------
//! HACK: Fake query.
//! Extract body name from body long name.

QString
queryBodyName(const QString &bodyLongName, QString *plugLongName)
{
    QString bodyName("");
    const QString pln(queryPlugLongName(bodyLongName, &bodyName));

    if (0 != plugLongName) {
        *plugLongName = pln;
    }

    return bodyName;
}


// queryChannelLongName
// --------------------
//! HACK: Fake query.

QString
queryChannelLongName(const QString &shapeName,
                     const QString &channelName)
{
    return shapeName + "." + channelName;
}


// queryBodyLongName
// -----------------
//! HACK: Fake query.
//! Create a body long name from a plug long name and a body name.

QString
queryBodyLongName(const QString &plugLongName, const QString &bodyName)
{
    return (plugLongName + _bodyLongNameSeparator + bodyName);
}


// queryBodyLongName
// -----------------
//! HACK: Fake query.
//! Create a body long name.

QString
queryBodyLongName(const QString &opInstance,
                  const QString &plugName,
                  const QString &bodyName)
{
    return queryBodyLongName(queryPlugLongName(opInstance, plugName), bodyName);
}
