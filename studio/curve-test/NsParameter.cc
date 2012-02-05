// -----------------------------------------------------------------------------
//
// NsParameter.cc
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

#include "NsParameter.h"
#include "NsCurveCamera.h"  // Fwd declared.
#include <QtOpenGL>

#include "em_curve.h"       // TMP!!

void
NsParameter::draw(const NsCurveCamera& cam) const
{
    const int steps(640);
    const double dt(cam.timeRange()/steps);

    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    double t(cam.timeMin());
    double vPrev(_timeValue->eval(t));
    for(int i = 1; i < steps; ++i)
    {
        glVertex2d(t, vPrev);
        t += dt;
        vPrev = _timeValue->eval(t);
        glVertex2d(t, vPrev);
    }
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    const int n_kf(static_cast<const em::curve*>(_timeValue)->size());
    for(int kf = 0; kf < n_kf; ++kf)
    {
        glVertex2d(
            (*static_cast<const em::curve*>(_timeValue))[kf].t(),
            (*static_cast<const em::curve*>(_timeValue))[kf].v());
    }
    glEnd();

    //if( !curve_.empty() )
    {
//        if( cam.timeMax() < curve_.first()->time() )
//        {   // Window right bound is left of curve,
//            // no splines or keyframes shown.
//            //
//            glColor3f(1.0f, 0.0f, 0.0f);
//            glPointSize(10.0f);
//            glBegin(GL_LINES);
//            glVertex2d(cam.timeMin(), curve_.first()->value());
//            glVertex2d(cam.timeMax(), curve_.first()->value());
//            glEnd();
//        }
//        else if( cam.timeMin() > curve_.last()->time() )
//        {   // Window left bound is right of curve,
//            // no splines or keyframes shown.
//            //
//            glColor3f(1.0f, 0.0f, 0.0f);
//            glPointSize(10.0f);
//            glBegin(GL_LINES);
//            glVertex2d(cam.timeMin(), curve_.last()->value());
//            glVertex2d(cam.timeMax(), curve_.last()->value());
//            glEnd();
//        }
//        else
//        {   // Splines and/or keyframes drawn.
//            //
//            const int kf_min(curve_.find_left_keyframe(cam.timeMin()));
//            const int kf_max(curve_.find_left_keyframe(cam.timeMax()));
//            //std::cerr << "KF min: " << kf_min << " | " << kf_max << "\n";
//
//            glColor3f(0.0f, 1.0f, 0.0f);
//            glBegin(GL_LINES);
//
//            if( kf_min == kf_max )
//            {   // Draw only part of spline from kf_min to (kf_min + 1),
//                // no keyframes.
//
//            }
//            else
//            {   // More than one spline, possibly with line segments
//                // outside curve.
//                if( cam.timeMin() < curve_.first()->time() )
//                {   // Draw line to first keyframe, no need for a spline.
//                    glVertex2d(cam.timeMin(), curve_.first()->value());
//                    glVertex2d(curve_.first()->time(), curve_.first()->value());
//                }
//                else
//                {   // draw spline from left edge of window to (kf_min + 1)
//
//                }
//
//                const int segs(5);   // TODO: User parameter. Segments per spline.
//
//                for(int kf = kf_min+1; kf < kf_max; ++kf)
//                {   // Draw internal splines.
//                    em::curve::time_type t = curve_[kf].time();
//                    em::curve::time_type dt
//                            = (curve_[kf+1].time() - t)/(segs);
//
//                    // First seg, start at keyframe.
//                    glVertex2d(t, curve_[kf].value());
//                    t += dt;
//                    glVertex2d(t, curve_(t));
//
//                    for(int s = 1; s < segs - 1; ++s)
//                    {
//                        glVertex2d(t, curve_(t));
//                        t += dt;
//                        glVertex2d(t, curve_(t));
//                    }
//                    glVertex2d(t, curve_(t));
//                    glVertex2d(curve_[kf+1].time(), curve_[kf+1].value());
//                }
//
//                if( cam.timeMax() > curve_.last()->time() )
//                {   // Draw line from last keyframe to right edge of
//                    // view, no need for a spline.
//                    glVertex2d(curve_[kf_max].time(), curve_[kf_max].value());
//                    glVertex2d(cam.timeMax(), curve_[kf_max].value());
//                }
//                else
//                {   // draw spline from kf_max to right edge of window
//                }
//            }
//            glEnd();
//        }
    }
}
