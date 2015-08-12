/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2015

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
 *  Created on: March 24, 2015
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "gtest/gtest.h"

#include "../src/Various/mixt_Constants.h"
#include "../src/Mixture/Ordinal/mixt_BOSNode.h"
#include "../src/Mixture/Ordinal/mixt_BOSPath.h"
#include "../src/Statistic/mixt_MultinomialStatistic.h"
#include "../src/Statistic/mixt_UniformStatistic.h"
#include "UTestFunction.h"

using namespace mixt;

