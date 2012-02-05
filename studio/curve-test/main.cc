// -----------------------------------------------------------------------------
//
// main.cc
//
// Main.
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
#include "NsCurveGrid.h"
#include "NsCurveKeyframe.h"
#include "NsCurveItem.h"
#include "NsCurveScene.h"
#include "NsCurveViewer.h"
//#include "NsCurveCamera.h"

#include <QApplication>
#include <QtDebug>
//#include <QGLWidget>
//#include <QtOpenGL>
#include <cstdlib>  // std::abort

//#include <iostream>
#include <vector>
#include <string>
#include <limits>

// TMP: For testing.
#include "em_curve_factory.h"
#include "em_time_value.h"
#include "em_curve.h"



void
curveParseTest()
{   // Testing a couple of invalid curve strings, exceptions
    // should be thrown (and caught).
    const std::string crvStr[] =
    {
        std::string("~1[0:0 -1:0.2 3:-7.4 2:1.4]"), // Valid
        std::string("~5[0:0 1:0 2:0]"),             // Invalid order
        std::string("1[0:0 1:0 2:0]"),              // Missing '~'
        std::string("~10:0 1:0 2:0]"),              // Missing '['
        std::string("~1[0:0 1:0 2:0"),              // Missing ']'
        std::string("~1[]"),                        // Empty
        std::string("~1[0:0 1fg:89]"),              // Test stringstream
        std::string("~1[0:0 1:0 2:0 1:2.3]"),       // Duplicate keys
        std::string("~1[0:0 7:-5 2:0 1:2.3] ] ]"),  // Strange syntax
        std::string("~1[0:0 7,-5 2.0 1.2:3.4]")     // Invalid keyframes
    };

    for(int i = 0; i < 10; ++i)
    {
        try
        {
            qDebug() << "[Input]: \"" << crvStr[i].c_str() << "\"";
            em::time_value* tv
                = em::crv_fact::instance().create(crvStr[i]);
            qDebug() << "[Output]: \"" << tv->str().c_str() << "\"\n";
            delete tv;
        }
        catch( std::exception& ex )
        {
            qDebug() << ex.what() << "\n";
        }
    }
}



void
parseCLArgs(int argc, char* argv[], std::vector<std::string>* strVec)
{
    if( 1 < argc && 0 != strVec )
    {
        for(int i = 1; i < argc; ++i)
        {
            qDebug() << "Arg[" << i << "]: \"" << argv[i] << "\"";
            const std::string argStr(argv[i]);
            const std::string::size_type sepPos(argStr.find_first_of(':'));
            const std::string prmStr(argStr.substr(0, sepPos));
            const std::string valStr(argStr.substr(
                sepPos + 1, std::string::npos));

            //std::istringstream iss(val_str);
            if( std::string("curve") == prmStr )
            {
                strVec->push_back(valStr);
            }
        }
    }
}



int
main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    // Create a "dummy" parameter outside the CurveEditor.
    // In Naiad, the CurveEditor would update this parameter
    // through commands, which would be created through NsNaiad.

    std::string prmString("~3[-40:33.2 0:0 25:-7.4 82:12.4]");
    NsParameter param("Camera",
                      "PosX",
                      em::crv_fact::instance().create(prmString));

    NsCurveScene crvScene;
    QGraphicsItem *grid = new NsCurveGrid();
//    QGraphicsItem *dot =
//        new QGraphicsEllipseItem(25.0, 25.0, 10.0, 10.0, grid);
    crvScene.addItem(grid);

//    crvScene.addItem(new NsCurveKeyframe());
    crvScene.addItem(
            new NsCurveItem("",
                            "",
                            em::crv_fact::instance().create(prmString)));
    //crvScene.addText("Hello World!");
    //crvScene.addRect(0.0, 0.0, 100.0, 50.0);

    NsCurveViewer crvView(&crvScene);
    crvView.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    crvView.show();



    // Process command line args.
    std::vector<std::string> strVec;
    parseCLArgs(argc, argv, &strVec);

    curveParseTest();

    return QApplication::exec();

//    std::vector<NsParameter> prmVec(strVec.size());
//    for(std::vector<std::string>::size_type i = 0; i < strVec.size(); ++i)
//    {
//        try
//        {
//            prmVec[i].setValue(em::crv_fact::instance().create(strVec[i]));
//            //crvView.addParameter(prmVec[i]);
//        }
//        catch( std::exception& ex )
//        {
//            std::cerr << ex.what() << "\n";
//        }
//    }

//    double min_time(std::numeric_limits<double>::max());
//    double max_time(-std::numeric_limits<double>::max());
//    double min_val(std::numeric_limits<double>::max());
//    double max_val(-std::numeric_limits<double>::max());
//
//    for(std::vector<std::string>::size_type i = 0; i < strVec.size(); ++i)
//    {
//        std::cerr << "Curve[" << i << "]: \"" << str_vec[i] << "\" [Input]\n";
//        em::linear_curve* lin_crv();
//
//        NsParameter prm;
//
//        if( !prm.curve().parse(str_vec[i]) )
//        {
//            std::abort();
//        }
//
//        if( 0 != prm.curve().first() && 0 != prm.curve().last() )
//        {
//            min_time = std::min<double>(min_time, prm.curve().first()->time());
//            max_time = std::max<double>(max_time, prm.curve().last()->time());
//            min_val = std::min<double>(min_val, prm.curve().first()->value());
//            max_val = std::max<double>(max_val, prm.curve().last()->value());
//        }
//
//        std::cerr << "Curve[" << i << "]: " << prm.curve() << " [Internal]\n";
//        std::cerr << "Curve[" << i << "]: \""
//                << prm.curve().str() << "\" [Output]\n";
//
//        prm_vec.push_back(prm);
//
//        crvView.addParameter(&prm_vec.back());
//    }
//
//    std::cerr << "Time: [" << min_time << ", " << max_time << "]\n";
//    std::cerr << "Value: [" << min_val << ", " << max_val << "]\n";

//    return QApplication::exec();
}




//scalar::matrix4d mvp(cam.modelview()*cam.projection());

//std::cerr << mvp;
//std::cerr << "Det: " << determinant(mvp) << "\n\n";
////m1 = scalar::matrix4d::identity;
//scalar::invert(&mvp);
//std::cerr << "Inverse:\n" << mvp << "\n\n";

//scalar::matrix2d m2(
//	1.0, 2.9,
//	1.0, 2.0);
//std::cerr << m2 << "\n";
//scalar::invert(&m2);
//std::cerr << m2 << "\n";



    //core::timer total_timer;

    // Open log file.
    //
    //core::log::instance().buf().setf(std::ios::fixed, std::ios::floatfield);
    //core::log::instance().buf().precision(config::default_io_precision);

//	if( !core::log::instance().open(config::default_log_fname) )
//	{	// Failure opening log file.
//		//
//		std::abort();
//	}

    // Window with OpenGL context.
    //
//	gl_window win;
//	gl_window::parameters win_p;	// Use default values.
//	if( !win.open(win_p) ) { std::abort(); }	// Failure opening window.

    // Viewport.
    //
//	gl_viewport vp(
//		scalar::vec2<int>(0, 0),
//		scalar::vec2<int>(win.params().width, win.params().height));

    // Camera, positioned such that z = 0 is in the middle of
    // near- and far planes.
    //
//	scalar::vec3d cam_pos(scalar::vec3d::origin);
//	cam_pos.z = 0.5*(config::default_cam_near + config::default_cam_far);
//	gl_ortho_camera cam(
//		-1.0, 1.0,
//		-1.0, 1.0,
//		config::default_cam_near, config::default_cam_far,
//		cam_pos);

//	vp.set_cam(&cam);		// Attach camera to viewport.
//	win.add_viewport(vp);	// Attach viewport to window.

    // Renderer, created after the OpenGL context.
    //
//	gl_renderer ren;
//	if( !ren.init_gl() ) { std::abort(); }	// Initializing OpenGL.

    // Read curve from file.
    //


//	while( win.is_open() )
//	{	// Main loop.
//		//
//		ren.pre_render();
//		ren.set_cam(cam);
//		ren.render_grid();
//		//ren.render_curve();
//
//		ren.post_render();
//		ren.swap_buffers();
//
//		win.check_events();
//	}

//	core::log::instance().buf()
//		<< "Total time: " << total_timer.elapsed_ms() << " [ms]\n";
//	core::log::instance().flush();

    // Shutting down...
    //
//	win.close();
//	core::log::instance().close();
