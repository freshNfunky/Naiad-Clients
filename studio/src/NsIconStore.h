// -----------------------------------------------------------------------------
//
// NsIconStore.h
//
// Naiad Studio icon store header file.
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

#ifndef NS_ICONSTORE_H
#define NS_ICONSTORE_H

#include <QIcon>


// NsIconStore
// -----------
//! The singleton NsIconStore class, which stores all icons used in the program.

class NsIconStore
{
public:

    //! Returns the Naiad icon.
    static QIcon naiadIcon() { return store()->_naiadIcon; }

    //! Returns the warning icon.
    static QIcon warningIcon() { return store()->_warningIcon; }

    //! Returns the error icon.
    static QIcon errorIcon() { return store()->_errorIcon; }

    //! Returns the internal error icon.
    static QIcon internalErrorIcon() { return store()->_internalErrorIcon; }

    //! Returns the empty icon.
    static QIcon emptyIcon() { return store()->_emptyIcon; }

    //! Returns the plus icon.
    static QIcon plusIcon() { return store()->_plusIcon; }

    //! Returns the minus icon.
    static QIcon minusIcon() { return store()->_minusIcon; }

    //! Returns the to beginning icon.
    static QIcon beginningIcon() { return store()->_beginningIcon; }

    //! Returns the to end icon.
    static QIcon endIcon() { return store()->_endIcon; }

    //! Returns the back icon.
    static QIcon backIcon() { return store()->_backIcon; }

    //! Returns the forward icon.
    static QIcon forwardIcon() { return store()->_forwardIcon; }

    //! Returns the play icon.
    static QIcon playIcon() { return store()->_playIcon; }

    //! Returns the play backwards icon.
    static QIcon playBackwardsIcon() { return store()->_playBackwardsIcon; }

    //! Returns the loop icon.
    static QIcon loopIcon() { return store()->_loopIcon; }

    //! Returns the solve from here icon.
    static QIcon solveFromCurrentFrameIcon() 
    { return store()->_solveFromCurrentFrameIcon; }

    //! Returns the solve to here icon.
    static QIcon restepIcon() 
    { return store()->_restepIcon; }

    //! Returns the solve frame icon.
    static QIcon solveFrameIcon() { return store()->_solveFrameIcon; }

    //! Returns the reset solver icon.
    static QIcon resetSolverIcon() { return store()->_resetSolverIcon; }

    //! Returns the lock icon.
    static QIcon lockIcon() { return store()->_lockIcon; }

    //! Returns the folder icon.
    static QIcon folderIcon() { return store()->_folderIcon; }

    //! Returns the curve icon.
    static QIcon curveIcon() { return store()->_curveIcon; }

private:

    NsIconStore();
    static NsIconStore *theIconStore;
    static NsIconStore *store();

    QIcon _naiadIcon;          //!< The Naiad icon.
    QIcon _warningIcon;        //!< The warning icon.
    QIcon _errorIcon;          //!< The error icon.
    QIcon _internalErrorIcon;  //!< The internal error icon.
    QIcon _emptyIcon;          //!< The empty icon.
    QIcon _plusIcon;           //!< The plus icon.
    QIcon _minusIcon;          //!< The minus icon.
    QIcon _beginningIcon;      //!< The beginning icon.
    QIcon _endIcon;            //!< The end icon.
    QIcon _backIcon;           //!< The back icon.
    QIcon _forwardIcon;        //!< The forward icon.
    QIcon _playIcon;           //!< The play icon.
    QIcon _playBackwardsIcon;  //!< The play backwards icon.
    QIcon _loopIcon;           //!< The loop icon.
    QIcon _solveFromCurrentFrameIcon;  //!< The solve from here icon.
    QIcon _restepIcon;         //!< The "re-step" icon.
    QIcon _solveFrameIcon;     //!< The solve frame icon.
    QIcon _resetSolverIcon;    //!< The reset solver icon.
    QIcon _lockIcon;           //!< The lock icon.
    QIcon _folderIcon;         //!< The folder icon.
    QIcon _curveIcon;          //!< The curve icon.
};

#endif // NS_ICONSTORE_H
