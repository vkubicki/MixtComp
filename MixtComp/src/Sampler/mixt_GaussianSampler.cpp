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
 *  Created on: Feb 18, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "mixt_GaussianSampler.h"
#include "STatistiK/include/STK_Law_Normal.h"
#include "STatistiK/include/STK_Law_Exponential.h"
#include "STatistiK/include/STK_Law_Uniform.h"

namespace mixt
{
GaussianSampler::GaussianSampler(AugmentedData<STK::Array2D<STK::Real> >* p_augData,
                                 const STK::Array2D<STK::Real>* p_param) :
    p_augData_(p_augData),
    p_param_(p_param),
    p_zi_(0)
{}

GaussianSampler::~GaussianSampler()
{}

void GaussianSampler::sampleIndividual(int i, int z_i)
{
#ifdef MC_DEBUG
  std::cout << "GaussianSampler::sampleIndividual" << std::endl;
  std::cout << "\ti: " << i << ", z_i: " << z_i << std::endl;
  std::cout << "\tp_param_->sizeRows()" << p_param_->sizeRows() << std::endl;
  std::cout << "\tp_param_->sizeCols()" << p_param_->sizeCols() << std::endl;
#endif

  if (p_augData_->misData_.find(i) == p_augData_->misData_.end())
  {
#ifdef MC_DEBUG
    std::cout << "\tempty iterator" << std::endl;
#endif
    return;
  }

  // loop on missing variables
  for (AugmentedData<STK::Array2D<STK::Real> >::ConstIt_MisVar itVar = p_augData_->getInd(i).begin(); // p_augData_->misData_.find(i)->(mapped element).(get iterator on variables)()
      itVar != p_augData_->getInd(i).end();
      ++itVar)
  {
    int j = itVar->first;
#ifdef MC_DEBUG
  std::cout << "\tj: " << j << std::endl;
#endif
    STK::Real z;
    STK::Real mean  = p_param_->elt(2 * z_i    , j);
    STK::Real sd    = p_param_->elt(2 * z_i + 1, j);

    switch(itVar->second.first) // (iterator on map)->(mapped element).(MisType)
    {
      case missing_:
      {
#ifdef MC_DEBUG
        std::cout << "\tmissing_" << std::endl;
#endif
        z = STK::Law::Normal::rand(0., 1.);
      }
      break;

      case missingIntervals_:
      {
        STK::Real infBound(itVar->second.second[0]); // (iterator on map)->(mapped element).(vector of parameters)[relevant parameter]
        STK::Real supBound(itVar->second.second[1]);

        STK::Real lower = (infBound - mean) / sd;
        STK::Real upper = (supBound - mean) / sd;
        STK::Real alpha = (lower + sqrt(pow(lower, 2) + 4.))/2.;

#ifdef MC_DEBUG
        std::cout << "\tmissingIntervals_" << std::endl;
        std::cout << "\tinfBound " << infBound << ", supBound: " << supBound << std::endl;
        std::cout << "\tlower " << lower << ", upper: " << upper << std::endl;
        std::cout << "\talpha " << alpha << std::endl;
#endif

        if (alpha*exp(alpha * lower / 2.) / sqrt(exp(1)) > exp(pow(lower, 2) / 2) / (upper - lower))
        {
          do
          {
            z = lbSampler(lower);
          }
          while(upper < z);
        }
        else
        {
          z = lrbSampler(lower, upper);
        }
      }
      break;

      case missingLUIntervals_: // missingLUIntervals
      {
#ifdef MC_DEBUG
        std::cout << "\tmissingLUIntervals_" << std::endl;
#endif
        STK::Real supBound(itVar->second.second[0]); // (iterator on map)->(mapped element).(vector of parameters)[relevant parameter]
        STK::Real upper = (supBound - mean) / sd;
        z = -lbSampler(-upper);
      }
      break;

      case missingRUIntervals_: // missingRUIntervals
      {
#ifdef MC_DEBUG
        std::cout << "\tmissingRUIntervals_" << std::endl;
#endif
        STK::Real infBound(itVar->second.second[0]); // (iterator on map)->(mapped element).(vector of parameters)[relevant parameter]
        STK::Real lower = (infBound - mean) / sd;
        z = lbSampler(lower);
      }
      break;
    }
    p_augData_->data_(i, j) = z * sd + mean;
  }
}

STK::Real GaussianSampler::lbSampler(STK::Real lower) const
{
  STK::Real alpha = (lower + sqrt(pow(lower, 2) + 4.))/2.;
  STK::Real z, u, rho;
  if (lower < 0)
  {
    do
    {
      z = STK::Law::Normal::rand(0, 1);
    }
    while (z < lower);
  }
  else
  {
    do
    {
      z = STK::Law::Exponential::rand(1./alpha) + lower;
      rho = exp(-pow((z - alpha), 2) / 2.);
      u = STK::Law::Uniform::rand(0., 1.);
    }
    while (u > rho);
  }
  return z;
}

STK::Real GaussianSampler::lrbSampler(STK::Real lower, STK::Real upper) const
{
  STK::Real z, u, rho;

  do
  {
    z = STK::Law::Uniform::rand(lower, upper);

    if (lower < 0. && 0. < upper)
      rho = exp(-pow(z, 2));
    else if (upper < 0.)
      rho = exp((pow(upper, 2)-pow(z, 2))/2);
    else if (0. < lower)
      rho = exp((pow(lower, 2)-pow(z, 2))/2);

    u = STK::Law::Uniform::rand(0., 1.);
  }
  while(u > rho);

  return z;
}
} // namespace mixt
