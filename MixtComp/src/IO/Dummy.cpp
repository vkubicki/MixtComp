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
 *  Created on: November 14, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "Dummy.h"

namespace mixt
{

std::string DataHandlerDummy::getData(std::string const& idName,
                                      Vector<std::string>& dataStr,
                                      int& nbInd,
                                      std::string& paramStr) const {
  return std::string();
}

/** Export function for Rank model */
void DataExtractorDummy::exportVals(std::string idName,
                                    const Vector<RankIndividual>& data) const {

}

void ParamExtractorDummy::exportParam(const std::string& idName,
                                      const std::string& paramName,
                                      const Matrix<Real>& paramStat,
                                      const Matrix<Real>& paramsLog,
                                      const std::vector<std::string>& paramNames,
                                      const Real confidenceLevel) const
{}

void ParamExtractorDummy::exportParam(const std::string& idName,
                                      const std::string& paramName,
                                      const std::vector<RankParamStat>& paramStat,
                                      const std::vector<std::string>& paramNames,
                                      Real confidenceLevel) const {
}

ParamSetterDummy::ParamSetterDummy()
{}

ParamSetterDummy::~ParamSetterDummy()
{}

void ParamSetterDummy::getParam(const std::string& idName,
                                const std::string& paramName,
                                Vector<Real>& params) const
{}

void  ParamSetterDummy::getParam(const std::string& idName,
                                 const std::string& paramName,
                                 Vector<RankVal>& param) const
{}

} // namespace mixt
