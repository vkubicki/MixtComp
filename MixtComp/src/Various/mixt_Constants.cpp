/* MixtComp version 2.0  - 13 march 2017
 * Copyright (C) Inria - Lille 1 */

/*
 *  Project:    MixtComp
 *  Created on: Sept 4, 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

/** @file mixt_Constant.cpp
 *  @brief In this file we define various constants
 **/

#include "mixt_Constants.h"

namespace mixt {

const std::string eol = "\n";

const int nbSamplingAttempts = 10000;

const Real epsilon = 1.e-8;
const Real logEpsilon = std::log(epsilon);

const int minModality = 1;
const int minIndex = 1;
const Real pi = 3.141592653589793;
const Real l2pi = 0.9189385332046727417;
const std::string progressFile = "progress";
const Real minInf = std::log(0.);

const int sizeTupleBOS = 2;
const int nbGibbsIniBOS = 100;
const Real piInitBOS = 0.9;
const Real piInitISR = 0.75;
const Real piThreshold = 0.05;

const int nbGibbsIterRankMStep = 50;
const Index nbGibbsIniISR = 100;

const int nbSampleObserved = 10000;

const int maxIterationFunctional = 10000;

const std::string rankPosSep = ",";

// const Real poissonInitMinAlpha = 0.5;

const Index nSemTry = 10;

} // namespace mixt
