// -----------------------------------------------------------------------------
//
// NsCurveCamera.cc
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


#include "NsCurveCamera.h"

// -----------------------------------------------------------------------------

namespace config {

// -----------------------------------------------------------------------------

const double tRange(2.0);
const double vRange(2.0);
const double hither(1.0);
const double yonder(3.0);
const em::vec3d wsxOrigin(0.0, 0.0, 0.5*(hither + yonder));

const em::vec3d tAxis(1.0, 0.0, 0.0);   // x-axis
const em::vec3d vAxis(0.0, 1.0, 0.0);   // y-axis
const em::vec3d zAxis(0.0, 0.0, 1.0);   // z-axis

// -----------------------------------------------------------------------------

}   // namespace config

// -----------------------------------------------------------------------------

NsCurveCamera::NsCurveCamera()
    : mv_(1.0),     // Identity
      proj_(1.0),   // Identity
      timeRange_(config::tRange),
      valueRange_(config::vRange),
      hither_(config::hither),
      yonder_(config::yonder)
{	// Set transformations.
	//
    updateModelview(config::wsxOrigin);
    updateProjection();
}

// -----------------------------------------------------------------------------

NsCurveCamera::NsCurveCamera(
        const em::vec3d& wsx,
        const double tRange,
        const double vRange)
    : mv_(1.0),     // Identity
      proj_(1.0),   // Identity
      timeRange_(std::max<double>(0.1, tRange)),     // TODO: Hard-coded bounds!
      valueRange_(std::max<double>(0.1, tRange)),
      hither_(config::hither),
      yonder_(config::yonder)
{   // Set transformations.
    //
    updateModelview(wsx);
    updateProjection();
}

// -----------------------------------------------------------------------------

const em::vec3d
NsCurveCamera::wsxOrigin() const
{
    return em::vec3d(-mv_[3][0], -mv_[3][1], -mv_[3][2]);
}

// -----------------------------------------------------------------------------

void
NsCurveCamera::zoom(const double factor)
{
    if( 0.0 > factor )
    {   // Ranges cannot be negative.
        //
        timeRange_ *= factor;
        valueRange_ *= factor;
        updateProjection();
    }
}

// -----------------------------------------------------------------------------

void
NsCurveCamera::pan(const em::vec2d& delta)
{
    em::vec3d wsx(wsxOrigin());
    wsx[0] += delta[0];
    wsx[1] += delta[1];
    updateModelview(wsx);
}

// -----------------------------------------------------------------------------

void
NsCurveCamera::updateModelview(const em::vec3d& wsx)
{   // TODO: Pass orientation vectors as arguments?
    //
    mv_[0][0] = config::tAxis[0];
    mv_[0][1] = config::vAxis[0];
    mv_[0][2] = config::zAxis[0];
    //mv_[3]  = 0.0;
    mv_[1][0] = config::tAxis[1];
    mv_[1][1] = config::vAxis[1];
    mv_[1][2] = config::zAxis[1];
    //mv_[7]  = 0.0;
    mv_[2][0] = config::tAxis[2];
    mv_[2][1] = config::vAxis[2];
    mv_[2][2] = config::zAxis[2];
    //mv_[11] = 0.0;
    mv_[3][0] = -wsx[0];
    mv_[3][1] = -wsx[1];
    mv_[3][2] = -wsx[2];
    //mv_[15] = 1.0;
}

// -----------------------------------------------------------------------------

void
NsCurveCamera::updateProjection()
{
    // Update relevant elements.
    //
    const double yhInv(1.0/(yonder_ - hither_));
    proj_[0][0] = 2.0/timeRange_;
    proj_[1][1] = 2.0/valueRange_;
    proj_[2][2] = -2.0*yhInv;
    proj_[3][2] = -(yonder_ + hither_)*yhInv;
}

// -----------------------------------------------------------------------------

//void
//curve_camera::viewport(const gl_viewport* vp)
//{
//	if( !core::is_null(vp) )
//	{
//		vp_ = vp;
//	}
//}

// -----------------------------------------------------------------------------

//const bool
//curve_camera::unproject(const vec3d& win, vec3d* obj) const
//{	// Project window coords into object space.
//	//
//    mat44d mvp(mv_*proj_);
//	vec4d in(win.x, win.y, win.z, 1.0);
//	vec4d out;
//
//	scalar::invert(&mvp);
//
//	if( !core::is_null(vp_) )
//	{	// Map x and y from window coordinates
//		//
//		in.x = (in.x - static_cast<double>(vp_->origin().x))/vp_->dim().x;
//		in.y = (in.y - static_cast<double>(vp_->origin().y))/vp_->dim().y;
//	}
//
//	// Map to range -1 to 1
//	//
//	in.x = 2.0*in.x - 1.0;
//	in.y = 2.0*in.y - 1.0;
//	in.z = 2.0*in.z - 1.0;
//
//	out = mvp*in;
//
//	if( !scalar::is_zero(out.w, 0.001) )
//	{
//		const double inv_w(1.0/out.w);
//		//assert(obj != 0);
//		obj->x = out.x*inv_w;
//		obj->y = out.y*inv_w;
//		obj->z = out.z*inv_w;
//		return true;	// No failures.
//	}
//	return false;
//}

// -----------------------------------------------------------------------------
