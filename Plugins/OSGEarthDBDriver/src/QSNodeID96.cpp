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

#ifndef USE_SIMDIS_SDK
#include "swapbytes.h"
#else // used in SIMDIS SDK
#include "swapbytes.h"
using namespace simVis_db;
#endif

#include "QSNodeID96.h"

using namespace std;

//===========================================================================
QSNodeID96::QSNodeID96()
  : one_(0),
    two_(0),
    three_(0)
{
}

//---------------------------------------------------------------------------
QSNodeID96::QSNodeID96(const uint32_t& value)
  : one_(value),
    two_(0),
    three_(0)
{
}

//---------------------------------------------------------------------------
QSNodeID96::~QSNodeID96()
{
}

//---------------------------------------------------------------------------
bool QSNodeID96::operator==(const QSNodeID96& value) const
{
  return ((one_ == value.one_) && (two_ == value.two_) && (three_ == value.three_));
}

//---------------------------------------------------------------------------
bool QSNodeID96::operator<(const QSNodeID96& value) const
{
  if (one_ != value.one_)
    return one_ < value.one_;
  if (two_ != value.two_)
    return two_ < value.two_;
  return three_ < value.three_;
}

//---------------------------------------------------------------------------
QSNodeID96& QSNodeID96::operator=(const QSNodeID96& value)
{
  one_ = value.one_;
  two_ = value.two_;
  three_ = value.three_;
  return *this;
}

//---------------------------------------------------------------------------
QSNodeID96 QSNodeID96::operator~() const
{
  QSNodeID96 returnValue = (*this);
  returnValue.one_ = ~one_;
  returnValue.two_ = ~two_;
  returnValue.three_ = ~three_;
  return returnValue;
}

//---------------------------------------------------------------------------
QSNodeID96& QSNodeID96::operator|=(const QSNodeID96& value)
{
  one_ |= value.one_;
  two_ |= value.two_;
  three_ |= value.three_;
  return *this;
}

//---------------------------------------------------------------------------
QSNodeID96& QSNodeID96::operator&=(const QSNodeID96& value)
{
  one_ &= value.one_;
  two_ &= value.two_;
  three_ &= value.three_;
  return *this;
}

//---------------------------------------------------------------------------
QSNodeID96 QSNodeID96::operator>>(int numBitsToShift) const
{
  QSNodeID96 returnValue;
  if (numBitsToShift >= 0)
  {
    if (numBitsToShift == 32)
    {
      returnValue.one_ = two_;
      returnValue.two_ = three_;
      returnValue.three_ = 0;
    }
    else if (numBitsToShift == 0)
    {
      returnValue.one_ = one_;
      returnValue.two_ = two_;
      returnValue.three_ = three_;
    }
    else if (numBitsToShift == 64)
    {
      returnValue.one_ = three_;
      returnValue.two_ = 0;
      returnValue.three_ = 0;
    }
    else if (numBitsToShift < 32)
    {
      returnValue.three_ = (three_ >> numBitsToShift);
      returnValue.two_ = (two_ >> numBitsToShift) | (three_ << (32 - numBitsToShift));
      returnValue.one_ = (one_ >> numBitsToShift) | (two_ << (32 - numBitsToShift));
    }
    else if (numBitsToShift < 64)
    {
      returnValue.three_ = 0;
      returnValue.two_ = three_ >> (numBitsToShift-32);
      returnValue.one_ = (two_ >> (numBitsToShift-32)) | (three_ << (64 - numBitsToShift));
    }
    else if (numBitsToShift < 96)
    {
      returnValue.three_ = 0;
      returnValue.two_ = 0;
      returnValue.one_ = three_ >> (numBitsToShift-64);
    }
    else
    {
      returnValue.one_ = 0;
      returnValue.two_ = 0;
      returnValue.three_ = 0;
    }
  }
  else
    returnValue.operator<<(-numBitsToShift);

  return returnValue;
}

//---------------------------------------------------------------------------
QSNodeID96 QSNodeID96::operator<<(int numBitsToShift) const
{
  QSNodeID96 returnValue;
  if (numBitsToShift >= 0)
  {
    if (numBitsToShift == 32)
    {
      returnValue.one_ = 0;
      returnValue.two_ = one_;
      returnValue.three_ = two_;
    }
    else if (numBitsToShift == 0)
    {
      returnValue.one_ = one_;
      returnValue.two_ = two_;
      returnValue.three_ = three_;
    }
    else if (numBitsToShift == 64)
    {
      returnValue.one_ = 0;
      returnValue.two_ = 0;
      returnValue.three_ = one_;
    }
    else if (numBitsToShift < 32)
    {
      returnValue.one_ = one_ << numBitsToShift;
      returnValue.two_ = (two_ << numBitsToShift) | (one_ >> (32 - numBitsToShift));
      returnValue.three_ = (three_ << numBitsToShift) | (two_ >> (32 - numBitsToShift));
    }
    else if (numBitsToShift < 64)
    {
      returnValue.one_ = 0;
      returnValue.two_ = one_ << (numBitsToShift-32);
      returnValue.three_ = (two_ << (numBitsToShift-32)) | (one_ >> (64 - numBitsToShift));
    }
    else if (numBitsToShift < 96)
    {
      returnValue.one_ = 0;
      returnValue.two_ = 0;
      returnValue.three_ = one_ << (numBitsToShift-64);
    }
    else
    {
      returnValue.three_ = 0;
      returnValue.two_ = 0;
      returnValue.one_ = 0;
    }
  }
  else
    returnValue.operator>>(-numBitsToShift);

  return returnValue;
}

//---------------------------------------------------------------------------
QSNodeID96 QSNodeID96::operator&(const QSNodeID96& value) const
{
  QSNodeID96 returnValue;
  returnValue.one_ = one_ & value.one_;
  returnValue.two_ = two_ & value.two_;
  returnValue.three_ = three_ & value.three_;
  return returnValue;
}

//---------------------------------------------------------------------------
void QSNodeID96::Pack(uint8_t* buffer) const
{
  if (buffer == NULL)
    return;
  bewrite(buffer, &three_);
  bewrite(buffer + sizeof(three_), &two_);
  bewrite(buffer + sizeof(three_) + sizeof(two_), &one_);
}

//---------------------------------------------------------------------------
void QSNodeID96::UnPack(const uint8_t* buffer)
{
  if (buffer == NULL)
    return;
  beread(buffer, &three_);
  beread(buffer + sizeof(three_), &two_);
  beread(buffer + sizeof(three_) + sizeof(two_), &one_);
}

//---------------------------------------------------------------------------
string QSNodeID96::FormatAsHex(bool bLeadingZeros) const
{
  string returnValue;

  char sTemp[64];

  if (bLeadingZeros)
  {
    sprintf(sTemp, "%08x", three_);
    returnValue += sTemp;
  }
  else
  {
    if (three_)
    {
      sprintf(sTemp, "%x", three_);
      returnValue += sTemp;
    }
  }

  if (bLeadingZeros)
  {
    sprintf(sTemp, "%08x", two_);
    returnValue += sTemp;
  }
  else
  {
    if (two_)
    {
      if (three_)
        sprintf(sTemp, "%08x", two_);
      else
        sprintf(sTemp, "%x", two_);
      returnValue += sTemp;
    }
  }

  if (bLeadingZeros)
  {
    sprintf(sTemp, "%08x", one_);
    returnValue += sTemp;
  }
  else
  {
    if (one_)
    {
      if (three_ || two_)
        sprintf(sTemp, "%08x", one_);
      else
        sprintf(sTemp, "%x", one_);
      returnValue += sTemp;
    }
  }

  if (returnValue.empty())
    returnValue = "0";

  return returnValue;
}
