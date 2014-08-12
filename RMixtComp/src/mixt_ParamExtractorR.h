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
 *  Created on: August 07, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#ifndef MIXT_PARAMEXTRACTOR_H
#define MIXT_PARAMEXTRACTOR_H

#include "Arrays/include/STK_Array2D.h"
#include "Rcpp.h"

namespace mixt
{

class ParamExtractorR
{
  public:
    ParamExtractorR();
    ~ParamExtractorR();
    void exportParam(std::string idName,
                     const STK::Array2D<STK::Real>* p_params);
    Rcpp::List rcppReturnParam() const;
  private:
    Rcpp::List param_;
};

} // namespace mixt

#endif // MIXT_DATAEXTRACTOR_H