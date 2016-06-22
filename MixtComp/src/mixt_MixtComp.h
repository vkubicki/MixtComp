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
 *  Created on: Apr 14, 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#ifndef MIXT_MIXTCOMP_H
#define MIXT_MIXTCOMP_H

#include <algorithm>
#include <iterator>

#include "boost/regex.hpp"

#include <nlopt.h>

#include "IO/Dummy.h"
#include "IO/mixt_IO.h"
#include "Data/mixt_MisValParser.h"
#include "LinAlg/mixt_Math.h"
#include "LinAlg/mixt_Typedef.h"
#include "Mixture/Functional/mixt_Function.h"
#include "Mixture/Functional/mixt_FunctionalComputation.h"
#include "Mixture/Functional/mixt_FunctionalClass.h"
#include "Mixture/Functional/mixt_FunctionalParser.h"
#include "Mixture/SimpleMixtureBridge/mixt_Clust_Traits.h"
#include "Manager/mixt_MixtureManager.h"
#include "Strategy/mixt_SEMStrategy.h"
#include "Strategy/mixt_GibbsStrategy.h"
#include "Strategy/mixt_StrategyParam.h"
#include "Various/mixt_Timer.h"

#endif /* MIXT_MIXTCOMP_H */
