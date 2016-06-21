/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2016

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
 *  Created on: June 20, 2016
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include <boost/algorithm/string.hpp>

#include "Data/mixt_MisValParser.h"
#include "mixt_FunctionalParser.h"

namespace mixt {

std::string parseFunctionalStr(const Vector<std::string>& vecStr,
                               Index minMod,
                               Index& nbMod,
                               Vector<Function>& vecInd) {
  std::string warnLog;

  Index nInd = vecStr.size();
  vecInd.resize(nInd);

  MisValParser<Real> mvp(0.); // no need for offset as data is continuous

  std::vector<std::string> strs;
  for (Index i = 0; i < nInd; ++i) {
    boost::split(strs,
                 vecStr(i),
                 boost::is_any_of(rankPosSep));
  }

  return warnLog;
}

} // namespace mixt