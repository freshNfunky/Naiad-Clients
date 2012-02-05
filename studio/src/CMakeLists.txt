#
# CMAKE project for Exotic Matter's Naiad Studio (NAIAD_STUDIO)
#
# Copyright (c) 2010 Exotic Matter AB. All rights reserved.
#
# This file is part of Naiad Studio.
#
# Naiad Studio is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# Naiad Studio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# If you did not receive a copy of the GNU General Public License
# along with Naiad Studio, please see <http://www.gnu.org/licenses/>.
#

project (NAIAD_STUDIO)

find_package (Qt4 4.6 COMPONENTS QtCore QtGui QtOpenGL QtWebKit REQUIRED)

set (QT_USE_QTOPENGL TRUE)
set (QT_USE_QTWEBKIT TRUE)
include (${QT_USE_FILE})

include_directories (.
                     $ENV{DEV_ROOT_NCLIENTS}/studio/Ngl
                     $ENV{DEV_ROOT_NCLIENTS}/studio/em_gl
                     /usr/include/nvidia)

link_directories    (/usr/lib64/nvidia)

file(GLOB_RECURSE NSTUDIO_HDRS "*.h")
file(GLOB_RECURSE NSTUDIO_SRCS "*.cc")

#if ("$ENV{EM_COMPILER}" STREQUAL "MSVC")
source_group("Header Files\\Commands" 
             FILES 
             NsCmdBase.h
             NsCmdCentral.h
             NsCmdClearGraph.h
             NsCmdCopy.h
             NsCmdCreate.h
             NsCmdCreateAndSelect.h
             NsCmdCut.h
             NsCmdErase.h
             NsCmdFeed.h
             NsCmdFrameGraphView.h
             NsCmdPaste.h
             NsCmdRename.h
             NsCmdSelect.h
             NsCmdSelectAll.h
             NsCmdSelectAllBodies.h
             NsCmdSelectAllFeeds.h
             NsCmdSelectAllOps.h
             NsCmdSelectBody.h
             NsCmdSelectFeed.h
             NsCmdSelectOp.h
             NsCmdSetCurrentVisibleFrame.h
             NSCmdSetEnabled.h
             NsCmdSetFirstVisibleFrame.h
             NsCmdSetGroupPlug.h
             NsCmdSetLastVisibleFrame.h
             NsCmdSetMeta.h
             NsCmdSetOpPosition.h
             NsCmdSetOpState.h
             NsCmdSetParam.h
             NsCmdSetParam3.h
             NsCmdSmack.h
             NsCmdUnfeedAndErase.h
             NsCmdUnselectAndErase.h)
source_group("Source Files\\Commands" 
             FILES 
             NsCmdBase.cc
             NsCmdCentral.cc
             NsCmdClearGraph.cc
             NsCmdCopy.cc
             NsCmdCreate.cc
             NsCmdCreateAndSelect.cc
             NsCmdCut.cc
             NsCmdErase.cc
             NsCmdFeed.cc
             NsCmdFrameGraphView.cc
             NsCmdPaste.cc
             NsCmdRename.cc
             NsCmdSelectAll.cc
             NsCmdSelectAllBodies.cc
             NsCmdSelectAllFeeds.cc
             NsCmdSelectAllOps.cc
             NsCmdSelectBody.cc
             NsCmdSelectFeed.cc
             NsCmdSelectOp.cc
             NsCmdSetCurrentVisibleFrame.cc
             NSCmdSetEnabled.cc
             NsCmdSetFirstVisibleFrame.cc
             NsCmdSetGroupPlug.cc
             NsCmdSetLastVisibleFrame.cc
             NsCmdSetMeta.cc
             NsCmdSetOpPosition.cc
             NsCmdSetOpState.cc
             NsCmdSetParam.cc
             NsCmdSetParam3.cc
             NsCmdSmack.cc
             NsCmdUnfeedAndErase.cc
             NsCmdUnselectAndErase.cc)

source_group("Header Files\\3D View" 
             FILES 
             Ns3DBBox.h
             Ns3DBody.h
             Ns3DBodyScope.h
             Ns3DCacheObject.h
             Ns3DCameraScope.h
             Ns3DConstructionGridItem.h
             Ns3DField.h
             Ns3DFieldIsoScope.h
             Ns3DFieldScalarScope.h
             Ns3DFieldScope.h
             Ns3DFieldStreamLineScope.h
             Ns3DFieldTileScope.h
             Ns3DGhostScope.h
             Ns3DGhostScopeUtils.h
             Ns3DGraphicsItem.h
             Ns3DIsoScope.h
             Ns3DLocator.h
             Ns3DManipulator.h
             Ns3DManipulatorComponent.h
             Ns3DManipulatorPivot.h
             Ns3DMeshScope.h
             Ns3DOpAxisItem.h
             Ns3DOpBoxItem.h
             Ns3DOpClipBoxItem.h	
             Ns3DOpFrustumItem.h
             Ns3DOpImportTransformItem.h
             Ns3DOpItem.h
             Ns3DOpPlaneItem.h
             Ns3DOpSphereItem.h
             Ns3DOpVectorItem.h
             Ns3DOrthoCameraScope.h
             Ns3DParticleScope.h
             Ns3DPerspectiveCameraScope.h
             Ns3DResourceObject.h
             Ns3DRManipulator.h
             Ns3DScene.h
             Ns3DScopeUtils.h
             Ns3DSelectionManager.h
             Ns3DSliceScopeUtils.h
             Ns3DSManipulator.h
             Ns3DStreamlineScope.h
             Ns3DStreamlineScopeUtils.h
             Ns3DTileScope.h
             Ns3DTileScopeUtils.h
             Ns3DTManipulator.h
             Ns3DView.h) 
source_group("Source Files\\3D View" 
             FILES 
             Ns3DBody.cc
             Ns3DConstructionGridItem.cc
             Ns3DField.cc
             Ns3DLocator.cc
             Ns3DManipulator.cc	
             Ns3DOpAxisItem.cc
             Ns3DOpBoxItem.cc
             Ns3DOpClipBoxItem.cc
             Ns3DOpFrustumItem.cc
             Ns3DOpItem.cc
             Ns3DOpPlaneItem.cc
             Ns3DOpSphereItem.cc
             Ns3DOpVectorItem.cc
             Ns3DResourceObject.cc
             Ns3DRManipulator.cc
             Ns3DScene.cc
             Ns3DSelectionManager.cc
             Ns3DSManipulator.cc
             Ns3DTManipulator.cc
             Ns3DView.cc)

source_group("Header Files\\Message View"
             FILES
             NsMessageCallback.h
             NsMessageFilterAction.h
             NsMessageItem.h
             NsMessageWidget.h)
source_group("Source Files\\Message View"
             FILES
             NsMessageCallback.cc
             NsMessageFilterAction.cc
             NsMessageItem.cc
             NsMessageWidget.cc)

source_group("Header Files\\Help View"
             FILES
             NsHelpView.h)
source_group("Source Files\\Help View"
             FILES
             NsHelpView.cc)

source_group("Header Files\\Time Line"
             FILES
             NsTimeToolBar.h
             NsTimeSlider.h
             NsTimeLineEdit.h
             NsTimeAction.h)
source_group("Source Files\\Time Line"
             FILES
             NsTimeToolBar.cc
             NsTimeSlider.cc
             NsTimeLineEdit.cc
             NsTimeAction.cc)

source_group("Header Files\\Graph View"
             FILES
             NsGraphBodyInputPlugItem.h
             NsGraphBodyItem.h
             NsGraphBodyOpItem.h
             NsGraphBodyOutputPlugItem.h
             NsGraphBodyScopeOpitem.h
             NsGraphCameraScopeOpItem.h
             NsGraphFeedItem.h
             NsGraphFieldInputPlugItem.h
             NsGraphFieldOpItem.h
             NsGraphFieldOutputPlugItem.h
             NsGraphFieldScopeOpItem.h
             NsGraphGlobalOpItem.h
             NsGraphInputPlugItem.h
             NsGraphItem.h
             NsGraphOpItem.h
             NsGraphOpItemArgs.h
             NsGraphOpItemCreator.h
             NsGraphOpItemFactory.h
             NsGraphOpItemKey.h
             NsGraphOpNameItem.h
             NsGraphOpStateItem.h
             NsGraphOutputPlugItem.h
             NsGraphPlugItem.h
             NsGraphScene.h
             NsGraphView.h)
source_group("Source Files\\Graph View"
             FILES
             NsGraphBodyItem.cc
             NsGraphBodyOpItem.cc
             NsGraphBodyScopeOpitem.cc
             NsGraphCameraScopeOpItem.cc
             NsGraphFeedItem.cc
             NsGraphFieldOpItem.cc
             NsGraphFieldScopeOpItem.cc
             NsGraphGlobalOpItem.cc
             NsGraphInputPlugItem.cc
             NsGraphItem.cc
             NsGraphOpItem.cc
             NsGraphOpItemKey.cc
             NsGraphOpNameItem.cc
             NsGraphOpStateItem.cc
             NsGraphOutputPlugItem.cc
             NsGraphPlugItem.cc
             NsGraphScene.cc
             NsGraphView.cc)

source_group("Header Files\\Value Editor"
             FILES
             NsValueEditorWidget.h
             NsValueObjectBox.h
             NsOpObjectBox.h
             NsBodyObjectBox.h
             NsValueSectionBox.h
             NsOpSectionBox.h
             NsBodySectionBox.h
             NsValueHeaderBox.h
             NsOpHeaderBox.h
             NsBodyHeaderBox.h
             NsValueMetaBox.h
             NsOpMetaBox.h
             NsValueBaseWidget.h
             NsValueWidgetArgs.h
             NsValueWidgetKey.h
             NsValueWidgetLayout.h
             NsParamBaseWidget.h    # Parameters
             NsParamWidgetCreator.h
             NsParamWidgetFactory.h
             NsParamWidgetArgs.h
             NsParam1fWidget.h
             NsParam1i64Widget.h
             NsParam1i8Widget.h
             NsParam1iWidget.h
             NsParam1sWidget.h
             NsParam2iWidget.h
             NsParam3fWidget.h
             NsParam3iWidget.h
             NsParam4iWidget.h
             NsParamBodyNameListWidget.h
             NsParamBodyNameWidget.h
             NsParamCachedBodyNameListWidget.h
             NsParamCachedBodyNameWidget.h
             NsParamChannelNameListWidget.h
             NsParamChannelNameWidget.h
             NsParamEnumWidget.h
             NsParamFileNameWidget.h
             NsParamPathNameWidget.h
             NsParamToggleWidget.h
             NsPropBaseWidget.h     # Properties
             NsPropWidgetCreator.h
             NsPropWidgetFactory.h
             NsPropWidgetArgs.h
             NsProp1fWidget.h
             NsProp1i64Widget.h
             NsProp1i8Widget.h
             NsProp1iWidget.h
             NsProp1sWidget.h
             NsProp2iWidget.h
             NsProp3fWidget.h
             NsProp3iWidget.h
             NsProp4iWidget.h
             NsPropBodyNameListWidget.h
             NsPropBodyNameWidget.h
             NsPropChannelNameListWidget.h
             NsPropChannelNameWidget.h
             NsPropEnumWidget.h
             NsPropFileNameWidget.h
             NsPropPathNameWidget.h
             NsPropToggleWidget.h
             NsOpNameLineEdit.h

NsAbstractMetaWidget.h
NsMetaLabel.h
NsMetaLineEdit.h
NsMetaCheckBox.h

)

source_group("Source Files\\Value Editor"
             FILES             
             NsValueEditorWidget.cc
             NsValueObjectBox.cc
             NsOpObjectBox.cc
             NsBodyObjectBox.cc
             NsValueSectionBox.cc
             NsOpSectionBox.cc
             NsBodySectionBox.cc
             NsValueHeaderBox.cc
             NsOpHeaderBox.cc
             NsBodyHeaderBox.cc
             NsValueMetaBox.cc
             NsOpMetaBox.cc
             NsValueBaseWidget.cc
             NsValueWidgetArgs.cc
             NsValueWidgetKey.cc
             NsValueWidgetLayout.cc
             NsParamBaseWidget.cc    # Parameters
             NsParamWidgetArgs.cc
             NsParam1fWidget.cc
             NsParam1i64Widget.cc
             NsParam1i8Widget.cc
             NsParam1iWidget.cc
             NsParam1sWidget.cc
             NsParam2iWidget.cc
             NsParam3fWidget.cc
             NsParam3iWidget.cc
             NsParam4iWidget.cc
             NsParamBodyNameListWidget.cc
             NsParamBodyNameWidget.cc
             NsParamCachedBodyNameListWidget.cc
             NsParamCachedBodyNameWidget.cc
             NsParamChannelNameListWidget.cc
             NsParamChannelNameWidget.cc
             NsParamEnumWidget.cc
             NsParamFileNameWidget.cc
             NsParamPathNameWidget.cc
             NsParamToggleWidget.cc
             NsPropBaseWidget.cc     # Properties
             NsPropWidgetArgs.cc
             NsProp1fWidget.cc
             NsProp1i64Widget.cc
             NsProp1i8Widget.cc
             NsProp1iWidget.cc
             NsProp1sWidget.cc
             NsProp2iWidget.cc
             NsProp3fWidget.cc
             NsProp3iWidget.cc
             NsProp4iWidget.cc
             NsPropBodyNameListWidget.cc
             NsPropBodyNameWidget.cc
             NsPropChannelNameListWidget.cc
             NsPropChannelNameWidget.cc
             NsPropEnumWidget.cc
             NsPropFileNameWidget.cc
             NsPropPathNameWidget.cc
             NsPropToggleWidget.cc)

#endif ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

qt4_wrap_cpp (NSTUDIO_MOC_SRCS ${NSTUDIO_HDRS})
source_group("moc" FILES ${NSTUDIO_MOC_SRCS})

qt4_add_resources (NSTUDIO_QRC_SRCS nstudio.qrc)

add_executable (nstudio-app 
                ${NSTUDIO_SRCS} 
                ${NSTUDIO_HDRS} 
                ${NSTUDIO_MOC_SRCS} 
                ${NSTUDIO_QRC_SRCS})

if ("$ENV{EM_COMPILER}" STREQUAL "intel")
target_link_libraries (nstudio-app -dynamic Ngl Ni -static-intel ${QT_LIBRARIES} ${OPENGL_LIBRARIES} z)
endif ("$ENV{EM_COMPILER}" STREQUAL "intel")

if ("$ENV{EM_COMPILER}" STREQUAL "gcc")
target_link_libraries (nstudio-app Ngl Ni ${QT_LIBRARIES} ${OPENGL_LIBRARIES} z)
endif ("$ENV{EM_COMPILER}" STREQUAL "gcc")

if ("$ENV{EM_COMPILER}" STREQUAL "MSVC")
target_link_libraries (nstudio-app Ngl
                               Ni${EM_D}
                               ${QT_LIBRARIES} 
                               ${OPENGL_LIBRARIES})

endif ("$ENV{EM_COMPILER}" STREQUAL "MSVC")

install (TARGETS nstudio-app DESTINATION clients/nstudio/bin)

