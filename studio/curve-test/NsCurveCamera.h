// -----------------------------------------------------------------------------
//
// NsCurveCamera.h
//
// Controls parameter drawing in terms of positioning and zooming.
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

#ifndef NS_CURVECAMERA_H
#define NS_CURVECAMERA_H

#include "em_vec.h"
#include "em_mat44.h"



class NsCurveCamera
{
private:
    //static const em::vec3d right_vec;
    //static const em::vec3d up_vec;
    //static const em::vec3d view_vec;

    //static const em::vec3d timeDir;
    //static const em::vec3d valueDir;
    //static const em::vec3d zDir;

public:
    NsCurveCamera();
//    explicit NsCurveCamera(const em::vec3d& wsx);
//    NsCurveCamera(
//		const double left, const double right,
//		const double bottom, const double top,
//		const double hither, const double yonder,
//        const em::vec3d& wsx);
    explicit
    NsCurveCamera(
        const em::vec3d& wsx,
        const double tRange,
        const double vRange);

    NsCurveCamera(const NsCurveCamera& rhs)
        : mv_(rhs.mv_),
          proj_(rhs.proj_),
          timeRange_(rhs.timeRange_),
          valueRange_(rhs.valueRange_),
          hither_(rhs.hither_),
          yonder_(rhs.yonder_) {}

    ~NsCurveCamera() throw() {}

    NsCurveCamera&
    operator=(const NsCurveCamera& rhs)
    {
        mv_ = rhs.mv_;
        proj_ = rhs.proj_;
        timeRange_ = rhs.timeRange_;
        valueRange_ = rhs.valueRange_;
        hither_ = rhs.hither_;
        yonder_ = rhs.yonder_;
        return *this;
    }
	
    const em::mat44d& modelview() const { return mv_; }
    const em::mat44d& projection() const { return proj_; }
    const double timeRange() const { return timeRange_; }
    const double valueRange() const { return valueRange_; }
    const double hither() const { return hither_; }
    const double yonder() const { return yonder_; }
//	const double left() const { return left_; }
//	const double right() const { return right_; }
//	const double bottom() const { return bottom_; }
//	const double top() const { return top_; }
//	const double zoom() const { return zoom_; }

    const em::vec3d wsxOrigin() const;  // Extracted from MV matrix.
    const double timeMin() const { return wsxOrigin()[0] - 0.5*timeRange_; }
    const double timeMax() const { return wsxOrigin()[0] + 0.5*timeRange_; }
    const double valueMin() const { return wsxOrigin()[1] - 0.5*valueRange_; }
    const double valueMax() const { return wsxOrigin()[1] + 0.5*valueRange_; }

    void zoom(const double factor);
    void pan(const em::vec2d& delta);


    //void zoom_in(const double dz);
    //void zoom_out(const double dz);
    //const bool unproject(const vec3d& win, vec3d* obj) const;

private:
    void updateModelview(const em::vec3d& wsx);
    void updateProjection();

private:
    em::mat44d mv_;
    em::mat44d proj_;
    double timeRange_;
    double valueRange_;
    double hither_;     // Near.
    double yonder_;     // Far.
};

#endif	// NS_CURVECAMERA_H
