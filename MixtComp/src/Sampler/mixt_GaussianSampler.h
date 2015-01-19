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
 *  Created on: 14 feb. 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 *              iovleff, serge.iovleff@stkpp.org
 **/

#ifndef MIXT_GAUSSIANSAMPLER_H
#define MIXT_GAUSSIANSAMPLER_H

#include "../LinAlg/mixt_LinAlg.h"
#include "../Data/mixt_AugmentedData.h"
#include "../Statistic/mixt_UniformStatistic.h"
#include "../Statistic/mixt_NormalStatistic.h"
#include "../Statistic/mixt_ExponentialStatistic.h"

namespace mixt
{

class GaussianSampler
{
  public:
    GaussianSampler(AugmentedData<STK::Array2D<Real> >* p_augData,
                    const STK::Array2DVector<Real>* p_param,
                    int nbClass);
    ~GaussianSampler();
    /** Sample new values for the missing variables of the given individual */
    void sampleIndividual(int i, int z_i);
  private:
    AugmentedData<STK::Array2D<Real> >* p_augData_;
    const STK::Array2DVector<Real>* p_param_;

    UniformStatistic uniform_;
    NormalStatistic normal_;
    ExponentialStatistic exponential_;
};

} // namespace mixt

#endif /* MIXT_GAUSSIANSAMPLER_H */
