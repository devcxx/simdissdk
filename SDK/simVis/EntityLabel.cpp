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
#include "osg/AlphaFunc"
#include "osg/Depth"
#include "simVis/Constants.h"
#include "simVis/Registry.h"
#include "simVis/Utils.h"
#include "simVis/OverheadMode.h"

#include "EntityLabel.h"

namespace simVis
{

/** Reject pixels with an alpha equal or less than this value.  Useful for blending text correctly against SilverLining. */
static const float ALPHA_THRESHOLD = 0.05f;

EntityLabelNode::EntityLabelNode(osg::Group* root)
  : root_(root),
    hasLastPrefs_(false)
{
  // Note that labels are not flattened (by default) in overhead mode

  // Set various states in order to make rendering text look better against SilverLining
  osg::StateSet* stateSet = root_->getOrCreateStateSet();

  // Always write to the depth buffer, overriding the osgEarth internal settings
  stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 0, 1, true), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
  // Set the alpha function to reject pixels of ALPHA_THRESHOLD or less, fixing SilverLining issues
  stateSet->setAttributeAndModes(new osg::AlphaFunc(osg::AlphaFunc::GREATER, ALPHA_THRESHOLD));
  // Turn on the alpha test so that the <=ALPHA_THRESHOLD rejection happens
  stateSet->setMode(GL_ALPHA_TEST, osg::StateAttribute::ON);
}

EntityLabelNode::~EntityLabelNode()
{
}

/// Update the label with the given preferences and text
void EntityLabelNode::update(const simData::CommonPrefs& commonPrefs, const std::string& text, float zOffset)
{
  const simData::LabelPrefs& lastLabelPrefs = lastCommonPrefs_.labelprefs();
  const simData::LabelPrefs& labelPrefs = commonPrefs.labelprefs();

  // whether to draw the label at all:
  const bool draw = commonPrefs.draw() && labelPrefs.draw();

  // Only make the label when need it
  if (!draw && !label_.valid())
    return;

  bool forceStyle = false;

  if (!label_.valid())
  {
    // create a label node for the first time
    osgEarth::Symbology::Style style;
    style.getOrCreate<osgEarth::Symbology::TextSymbol>()->alignment() = static_cast<osgEarth::Symbology::TextSymbol::Alignment>(labelPrefs.alignment());
    style.getOrCreate<osgEarth::Symbology::TextSymbol>()->pixelOffset() = osg::Vec2s(labelPrefs.offsetx(), labelPrefs.offsety());

    label_ = new osgEarth::Annotation::LabelNode("", style);
    label_->setDynamic(true);
    label_->setNodeMask(simVis::DISPLAY_MASK_LABEL);
    label_->setHorizonCulling(false);
    label_->setOcclusionCulling(false);

    // Note: no need to clamp the label's geo transform in overhead mode, since the Locator
    // will take care of that for us. -gw

    root_->addChild(label_.get());
    forceStyle = true;
  }

  // Detect label changes in the last update and apply those changes
  if (label_.valid())
  {
    // check for an update:
    const bool textChanged = (text != lastText_);

    label_->setNodeMask(draw ? DISPLAY_MASK_LABEL : 0);

    // For priority pref, 0 is least likely to show, higher values more likely to show, negative values are always shown
    // FLT_MAX means always show to osgEarth
    const float actualPriority = (labelPrefs.priority() >= 0.0) ? labelPrefs.priority() : std::numeric_limits<float>::max();
    // Change the priority to user value, or FLT_MAX if always-show
    if (!simCore::areEqual(label_->getPriority(), actualPriority))
      label_->setPriority(actualPriority);

    // Preferences that change how the text is displayed and not the content of the text
    const bool labelStylePrefsChanged =
      !hasLastPrefs_ ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, color) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, offsetx) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, offsety) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, outlinecolor) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, overlayfontname) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, overlayfontpointsize) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, textoutline) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, backdroptype) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, alignment) ||
      PB_FIELD_CHANGED(&lastLabelPrefs, &labelPrefs, backdropimplementation);
    if (textChanged)
    {
      // update the text label.
      label_->setText(text);
    }

    // update the style:
    if (labelStylePrefsChanged || forceStyle)
    {
      osgEarth::Symbology::Style style;
      osgEarth::Symbology::TextSymbol* ts = style.getOrCreate<osgEarth::Symbology::TextSymbol>();
      ts->alignment() = static_cast<osgEarth::Symbology::TextSymbol::Alignment>(labelPrefs.alignment());
      ts->pixelOffset() = osg::Vec2s(labelPrefs.offsetx(), labelPrefs.offsety());

      // text color:
      osg::Vec4 color = ColorUtils::RgbaToVec4(labelPrefs.color());
      ts->fill() = osgEarth::Symbology::Fill(color.r(), color.g(), color.b(), color.a());

      // outline:
      if (labelPrefs.textoutline() != simData::TO_NONE && labelPrefs.backdroptype() != simData::BDT_NONE && color.a() != 0)
      {
        ts->halo()->color() = ColorUtils::RgbaToVec4(labelPrefs.outlinecolor());
        ts->haloOffset() = simVis::outlineThickness(labelPrefs.textoutline());
        ts->haloBackdropType() = backdropType(labelPrefs.backdroptype());
        ts->haloImplementation() = backdropImplementation(labelPrefs.backdropimplementation());
      }
      else
      {
        ts->halo()->color() = osg::Vec4();
        ts->haloOffset() = 0.f;
        ts->haloBackdropType() = osgText::Text::NONE;
      }

      // font:
      if (!labelPrefs.overlayfontname().empty())
      {
        std::string fileFullPath = simVis::Registry::instance()->findFontFile(labelPrefs.overlayfontname());
        if (!fileFullPath.empty()) // only set if font file found, uses default OS font otherwise
          ts->font() = fileFullPath;
      }

      ts->size() = simVis::osgFontSize(static_cast<float>(labelPrefs.overlayfontpointsize()));
      ts->content()->setLiteral(!commonPrefs.name().empty() ? commonPrefs.name() : "unnamed");

      label_->setStyle(style);
    }

    // apply the local altitude offset passed in
    label_->setLocalOffset(osg::Vec3f(0.0f, 0.0f, zOffset));
  }

  lastCommonPrefs_ = commonPrefs;
  hasLastPrefs_ = true;
  lastText_ = text;
}

void EntityLabelNode::addCullCallback(osg::Callback* callback)
{
  root_->addCullCallback(callback);
}

}

