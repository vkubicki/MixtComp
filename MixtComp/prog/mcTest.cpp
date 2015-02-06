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
 *  Created on: Nov 14, 2014
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include <iostream>
#include <limits>
#include <algorithm>
#include "../src/LinAlg/mixt_LinAlg.h"

using namespace mixt;

int main()
{
  Matrix<Real> m(3, 3);
//  Eigen::MatrixXf m(3, 3);
  m << 1, 2, 3, 4, 5, 6, 7, 8, 9;

//  Matrix<Real>::iterator endIt = m.end();
//  for (Matrix<Real>::iterator it = m.begin();
//       it != endIt;
//       ++it)
//  {
//    std::cout << *it << std::endl;
//  }
  sort(m);

  std::cout << m << std::endl;
}
