/* -*- mode: c++ -*- */
/****************************************************************************
 *****                                                                  *****
 *****                   Classification: UNCLASSIFIED                   *****
 *****                    Classified By:                                *****
 *****                    Declassify On:                                *****
 *****                                                                  *****
 ****************************************************************************
 *
 *
 * Developed by: Naval Research Laboratory, Tactical Electronic Warfare Div.
 *               EW Modeling & Simulation, Code 5773
 *               4555 Overlook Ave.
 *               Washington, D.C. 20375-5339
 *
 * License for source code at https://simdis.nrl.navy.mil/License.aspx
 *
 * The U.S. Government retains all rights to use, duplicate, distribute,
 * disclose, or release this software.
 *
 */
#ifndef SIMVIS_VELOCITY_VECTOR_H
#define SIMVIS_VELOCITY_VECTOR_H

#include <osg/Group>
#include <osg/LineWidth>
#include <osg/Vec4f>
#include "simCore/Common/Common.h"
#include "simData/DataTypes.h"
#include "simVis/Locator.h"

namespace osg { class Geometry; }

namespace simVis
{

/// Attachment node for a platform's velocity vector graphic.
class SDKVIS_EXPORT VelocityVector : public simVis::LocatorNode
{
public:
  /**
   * Construct a new velocity vector graphic.
   * @param hostLocator location of host platform
   * @param vectorColor color for velocity vector, in osg format: r,g,b,a values 0.0-1.0
   * @param lineWidth width of axis vector lines, in pixels
   */
  VelocityVector(Locator* hostLocator, const osg::Vec4f& vectorColor, float lineWidth = 2.0);

  /**
   * Sets new preferences for this object.
   * @param draw flag toggling the axis vectors visibility
   * @param prefs Preferences to apply
   * @param force Apply them even if the current settings already match
   */
  void setPrefs(bool draw, const simData::PlatformPrefs& prefs, bool force = false);

  /** Updates geometry to match given platform update */
  void update(const simData::PlatformUpdate& platformUpdate);

  /** Return the proper library name */
  virtual const char* libraryName() const { return "simVis"; }

  /** Return the class name */
  virtual const char* className() const { return "VelocityVector"; }

protected: // methods

  /// osg::Referenced-derived
  virtual ~VelocityVector();

private: // methods
  /// recreate the geometry
  void rebuild_(const simData::PlatformPrefs& prefs);

  /// create the velocity vector line
  void createVelocityVector_(const simData::PlatformPrefs& prefs, osg::Geode* geode) const;

  simData::PlatformPrefs               lastPrefs_;          ///< last prefs update
  bool                                 forceRebuild_;       ///< flag to force a rebuild
  osg::ref_ptr<osg::LineWidth>         lineWidth_;          ///< width of velocity vector lines
  osg::Vec4f                           vectorColor_;        ///< color for velocity vector, in osg format: r,g,b,a values 0.0-1.0
  simData::PlatformUpdate              lastUpdate_;         ///< Platform location and velocity
};

} // namespace simVis

#endif // SIMVIS_VELOCITY_VECTOR_H
