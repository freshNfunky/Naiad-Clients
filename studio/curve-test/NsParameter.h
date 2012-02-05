// -----------------------------------------------------------------------------
//
// NsParameter.h
//
// Owns a time value and can be drawn if provided a camera.
//
// Copyright (c) 2009 Exotic Matter AB.  All rights reserved.
//
// This material contains the confidential and proprietary information of
// Exotic Matter AB and may not be disclosed, copied or duplicated in
// any form, electronic or hardcopy, in whole or in part, without the
// express prior written consent of Exotic Matter AB. This copyright notice
// does not imply publication.
//
// -----------------------------------------------------------------------------

#ifndef NS_PARAMETER_H
#define NS_PARAMETER_H

#include "em_time_value.h"
#include <string>
//#include <boost/shared_ptr.hpp>



class NsCurveCamera;    // Fwd declaration.



class NsParameter
{
public:
    explicit NsParameter(
            const std::string& nodeName,
            const std::string& prmName,
            em::time_value* timeValue)
        : _nodeName(nodeName)
        , _prmName(prmName)
        , _timeValue(timeValue)
    {}

    NsParameter(const NsParameter& rhs)
        : _nodeName(rhs._nodeName)
        , _prmName(rhs._prmName)
    {
        // Deep copy.

        delete _timeValue;
        _timeValue = rhs._timeValue->clone();
    }

    ~NsParameter() throw()
    {
        delete _timeValue;
    }

    NsParameter&
    operator=(const NsParameter& rhs)
    {
        _nodeName = rhs._nodeName;
        _prmName = rhs._prmName;

        // Deep copy.

        delete _timeValue;
        _timeValue = rhs._timeValue->clone();

        return *this;
    }

    void draw(const NsCurveCamera& cam) const;

    const em::time_value*
    timeValue() const
    {
        return _timeValue;
    }

    void
    setTimeValue(em::time_value* timeValue)
    {
        // Owns given time_value.

        delete _timeValue;
        _timeValue = timeValue;
    }

private:
    std::string _nodeName;
    std::string _prmName;
    em::time_value* _timeValue;
};

#endif  // NS_PARAMETER_H
