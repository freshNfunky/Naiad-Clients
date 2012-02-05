// -----------------------------------------------------------------------------
//
// NsIconStore.cc
//
// Naiad Studio icon store source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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
// POSSIBILITY OF SUCH DAMAGE.//
// -----------------------------------------------------------------------------

#include "NsIconStore.h"


// NsIconStore
// -----------
//! Constructor, creates a new icon store.

NsIconStore::NsIconStore()
{
    _naiadIcon.addFile(":/images/naiad-icon-16.png", QSize(16, 16));
    _naiadIcon.addFile(":/images/naiad-icon-32.png", QSize(32, 32));
    _naiadIcon.addFile(":/images/naiad-icon-48.png", QSize(48, 48));

    _warningIcon.addFile(":/images/warning-16.png", QSize(16, 16));
    _errorIcon.addFile(":/images/error-16.png", QSize(16, 16));
    _internalErrorIcon.addFile(":/images/internal-error-16.png", QSize(16, 16));
    _emptyIcon.addFile(":/images/empty-16.png", QSize(16, 16));

    _plusIcon.addFile(":/images/plus-18.png", QSize(18, 18));
    _minusIcon.addFile(":/images/minus-18.png", QSize(18, 18));

    _beginningIcon.addFile(":/images/beginning-18.png", QSize(18, 18));
    _endIcon.addFile(":/images/end-18.png", QSize(18, 18));
    _backIcon.addFile(":/images/back-18.png", QSize(18, 18));
    _forwardIcon.addFile(":/images/forward-18.png", QSize(18, 18));
    _playIcon.addFile(":/images/play-18.png", QSize(18, 18),
                      QIcon::Normal, QIcon::Off);
    _playIcon.addFile(":/images/pause-18.png", QSize(18, 18),
                      QIcon::Normal, QIcon::On);
    _playBackwardsIcon.addFile(":/images/play-backwards-18.png", QSize(18, 18),
                               QIcon::Normal, QIcon::Off);
    _playBackwardsIcon.addFile(":/images/pause-18.png", QSize(18, 18),
                               QIcon::Normal, QIcon::On);
    _loopIcon.addFile(":/images/loop-18.png", QSize(18, 18));
    _solveFromCurrentFrameIcon.addFile(
        ":/images/solve-from-18.png", QSize(18, 18),
        QIcon::Normal, QIcon::Off
        );
    _solveFromCurrentFrameIcon.addFile(":/images/stop-18.png", QSize(18, 18),
                               QIcon::Normal, QIcon::On);
    _restepIcon.addFile(":/images/restep-18.png", QSize(18, 18),
                        QIcon::Normal, QIcon::Off);
    _solveFrameIcon.addFile(":/images/solve-18.png", QSize(18, 18),
                            QIcon::Normal, QIcon::Off);
    _solveFrameIcon.addFile(":/images/stop-18.png", QSize(18, 18),
                            QIcon::Normal, QIcon::On);
    _resetSolverIcon.addFile(":/images/reset-solver-18.png", QSize(18, 18));

    _lockIcon.addFile(":/images/lock-16.png", QSize(16, 16));
    _folderIcon.addFile(":/images/folder-16.png", QSize(16, 16));
    _curveIcon.addFile(":/images/curve-16.png", QSize(16, 16));
}


// theIconStore
// ------------
//! The singleton IconStore object.

NsIconStore *NsIconStore::theIconStore = 0;


// store
// -----
//! Creates, if necessary, and returns the singleton NsIconStore object.

NsIconStore *
NsIconStore::store()
{
    if (!theIconStore)
        theIconStore = new NsIconStore();

    return theIconStore;
}
