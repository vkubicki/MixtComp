/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2014

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 *  Project:    MixtComp
 *  Created on: July 1, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "mixt_GaussianDataStat.h"

namespace mixt
{

GaussianDataStat::GaussianDataStat(AugmentedData<Vector<Real> >& augData,
                                   Real confidenceLevel) :
    augData_(augData),
    confidenceLevel_(confidenceLevel)
{}

GaussianDataStat::~GaussianDataStat() {};

void GaussianDataStat::sample(int ind,
                              int iteration)
{
  Real currVal = augData_.data_(ind,
                                      0);
  stat_[iteration] = currVal;
}

void GaussianDataStat::sampleVals(int ind,
                                  int iteration,
                                  int iterationMax)
{
#ifdef MC_DEBUG
  std::cout << "GaussianDataStat::sampleVals" << std::endl;
#endif
  if (augData_.misData_(ind).first != present_)
  {
    if (iteration == 0) // clear the temporary statistical object
    {
#ifdef MC_DEBUG
      std::cout << "GaussianDataStat::sampleVals, iteration 0" << std::endl;
#endif
      // initialize internal storage
      stat_.resize(iterationMax + 1);

  #ifdef MC_DEBUG
      std::cout << "p_dataStatStorage_.rows(): " << dataStatStorage_.rows() << ", p_dataStatStorage_.cols(): "<< dataStatStorage_.cols() << std::endl;
  #endif
      // clear current individual

      dataStatStorage_(ind) = RowVector<Real>(3);
      dataStatStorage_(ind) = 0.;

      // first sampling
      sample(ind, iteration);
    }
    else if (iteration == iterationMax) // export the statistics to the p_dataStatStorage object
    {
#ifdef MC_DEBUG
      std::cout << "GaussianDataStat::sampleVals, iterationMax" << std::endl;
#endif
      // last sampling
      sample(ind, iteration);

#ifdef MC_DEBUG
      std::cout << "GaussianDataStat::sampleVals, last iteration" << std::endl;
      std::cout << "p_dataStatStorage_.rows(): " << dataStatStorage_.rows() << std::endl;
      std::cout << "tempStat_.rows(): " << tempStat_.rows() << std::endl;
      std::cout << "tempStat_: " << std::endl;
      std::cout << tempStat_ << std::endl;
#endif
      sortContiguous(stat_);
      Real alpha = (1. - confidenceLevel_) / 2.;
      Real realIndLow = alpha * iterationMax;
      Real realIndHigh = (1. - alpha) * iterationMax;

      RowVector<Real> tempPoint(3);
      tempPoint[0] = stat_.mean();
      tempPoint[1] =  (1. - (realIndLow  - int(realIndLow ))) * stat_(realIndLow )
                    + (      realIndLow  - int(realIndLow ) ) * stat_(realIndLow + 1);
      tempPoint[2] =  (1. - (realIndHigh - int(realIndHigh))) * stat_(realIndHigh)
                    + (      realIndHigh - int(realIndHigh) ) * stat_(realIndHigh + 1);
      dataStatStorage_(ind) = tempPoint;
#ifdef MC_DEBUG
      std::cout << "confidenceLevel_: " << confidenceLevel_ << std::endl;
      std::cout << "alpha: " << alpha << std::endl;
      std::cout << "realIndLow: " << realIndLow << std::endl;
      std::cout << "realIndHigh: " << realIndHigh << std::endl;
      std::cout << "tempVec: " << tempVec << std::endl;
#endif
    }
    else
    {
      // standard sampling
      sample(ind, iteration);
    }
  }
}

void GaussianDataStat::imputeData(int ind)
{
  if (augData_.misData_(ind).first != present_)
  {
    augData_.data_(ind) = dataStatStorage_(ind)[0]; // imputation by the expectation
  }
}

} // namespace mixt
