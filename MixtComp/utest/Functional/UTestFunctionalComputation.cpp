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
 *  Created on: May 31, 2016
 *  Author:     Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "../UTest.h"

using namespace mixt;

TEST(FunctionalComputation, Vandermonde) {
  Vector<Real> timeStep(2);
  timeStep << 2., 12.;

  Index nCoeff = 3;
  Index nObs = 2;
  Matrix<Real> vm;

  vandermondeMatrix(timeStep,
                    nCoeff,
                    vm);

  Matrix<Real> expectedVm(nObs, nCoeff);
  expectedVm << 1., 2., 4.,
                1., 12., 144.;

  ASSERT_EQ(true, vm.isApprox(expectedVm));
}

TEST(FunctionalComputation, regressionNoNoise) {
  Index nCoeff = 4;
  Index nObs = 100;

  Real xMin = -50.;
  Real xMax = 50.;

  Vector<Real> x(nObs);
  Vector<Real> y(nObs, 0.);

  Matrix<Real> design(nObs, nCoeff);

  Vector<Real> beta(nCoeff);
  beta << 6, -4, 12, 5; // 5 x**3 + 12 x**2 - 4 x + 6, standard error is 0
  Vector<Real> betaEstimated;
  Real sdEstimated;

  NormalStatistic normal;
  UniformStatistic uni;

  for (Index i = 0; i < nObs; ++i) {
    x(i) = uni.sample(xMin, xMax);
    for (Index p = 0; p < nCoeff; ++p) {
      y(i) += beta(p) * pow(x(i), p);
    }
  }

  vandermondeMatrix(x,
                    nCoeff,
                    design);

  regression(design,
             y,
             betaEstimated,
             sdEstimated);

  ASSERT_EQ(true, betaEstimated.isApprox(beta, epsilon));
  ASSERT_NEAR(0., sdEstimated, epsilon);
}

TEST(FunctionalComputation, regressionNoise) {
  Index nCoeff = 4;
  Index nObs = 100000;

  Real xMin = -50.;
  Real xMax = 50.;

  Vector<Real> x(nObs);
  Vector<Real> y(nObs, 0.);

  Matrix<Real> design(nObs, nCoeff);

  Vector<Real> beta(nCoeff); // +1 for the standard deviation
  beta << 6, -4, 12, 5; // 5 x**3 + 12 x**2 - 4 x + 6
  Real sd = 1.; // standard deviation is 1.
  Real sdEstimated;
  Vector<Real> betaEstimated;

  NormalStatistic normal;
  UniformStatistic uni;

  for (Index i = 0; i < nObs; ++i) {
    x(i) = uni.sample(xMin, xMax);
    for (Index p = 0; p < nCoeff; ++p) {
      y(i) += beta(p) * pow(x(i), p);
    }
    y(i) += normal.sample(0., sd);
  }

  vandermondeMatrix(x,
                    nCoeff,
                    design);

  regression(design,
             y,
             betaEstimated,
             sdEstimated);

  ASSERT_EQ(true, betaEstimated.isApprox(beta, 1e-3));
  ASSERT_NEAR(sd, sdEstimated, 0.01);
}

TEST(FunctionalComputation, subRegression) {
  Index nCoeff = 4;
  Index nObs = 100000;
  Index nSub = 3;

  Real xMin = -50.;
  Real xMax = 50.;

  Vector<Real> t(nObs);

  Matrix<Real> vandermonde(nObs, nCoeff); // design matrix

  Matrix<Real> beta(nSub, nCoeff);
  beta.row(0) << 6, -4, 12, 1; // x**3 + 12 x**2 - 4 x + 6
  beta.row(1) << 9, 1, -3, 2; // 2 x**3 - 3 x**2 - x + 9
  beta.row(2) << -25, 25, 75, -4; // -4 x**3 + 75 x**2 + 25 x - 25

  Vector<Real> sd(nSub);
  sd << 1, 2, 3;

  Matrix<Real> betaEstimated;
  Vector<Real> sdEstimated;

  MultinomialStatistic multi;
  NormalStatistic normal;
  UniformStatistic uni;

  Vector<std::list<Index> > w(nSub);
  for (Index i = 0; i < nObs; ++i) {
    t(i) = uni.sample(xMin, xMax);
    Index currW = multi.sampleInt(0, nSub - 1);
    w(currW).push_back(i);
  }

  vandermondeMatrix(t,
                    nCoeff,
                    vandermonde);

  Vector<Matrix<Real> > design(nSub); // vector of design matrices, one element per subregression
  Vector<Vector<Real> > y(nSub); // vector of rhs values, one per subregression
  Vector<Index> nT(nSub, 0); // number of timesteps in each subregression

  for (Index s = 0; s < nSub; ++s) {
    nT(s) = w(s).size();
    design(s).resize(nT(s), nCoeff);
    y(s).resize(nT(s));
  }

  for (Index s = 0; s < nSub; ++s) {
    Index i = 0;
    y(s) = 0.;
    for (std::list<Index>::const_iterator it = w(s).begin(), itE = w(s).end();
        it != itE;
        ++it) {
      design(s).row(i) = vandermonde.row(*it);

      for (Index p = 0; p < nCoeff; ++p) {
        y(s)(i) += beta(s, p) * pow(t(*it), p);
      }
      y(s)(i) += normal.sample(0, sd(s));

      ++i;
    }
  }

  subRegression(design,
                y,
                betaEstimated,
                sdEstimated);

  ASSERT_EQ(true, betaEstimated.isApprox(beta, 1e-3));
  ASSERT_EQ(true, sdEstimated.isApprox(sd, 0.01));
}

TEST(FunctionalComputation, smallTest) {
  Vector<Real> a(4, 0.);

  Vector<Real> dummy;
  ASSERT_NEAR(4, std::exp(dummy.logToMulti(a)), epsilon);
}

TEST(FunctionalComputation, costAndGrad1SubReg) {
  Index nTime = 4;
  Index nParam = 2;
  Real delta = epsilon;

  Index nSub = nParam / 2;

  Vector<Real> t(nTime);
  t << 0., 1., 2., 3.;

  Vector<std::set<Index> > w(nSub);
  w(0).insert(0);
  w(0).insert(1);
  w(0).insert(2);
  w(0).insert(3);

  double alpha0[] = {-1., 0.5};

  Matrix<Real> value;
  Vector<Real> sumExpValue;
  Vector<Real> fdGrad(nParam); // finite differences gradient
  Vector<Real> computedGrad(nParam); // analytical gradient
  double computedGradVec[nParam];
  Real c0; // base cost

  timeValue(t,
            nParam,
            alpha0,
            value,
            sumExpValue);

  costFunction(t,
               value,
               sumExpValue,
               w,
               c0);

  gradCostFunction(t,
                   value,
                   sumExpValue,
                   w,
                   computedGradVec);

  for (Index s = 0; s < nParam; ++s) {
    Real c1;
    double alpha1[nParam];
    for (Index i = 0; i < nParam; ++i) {
      alpha1[i] = alpha0[i];
    }
    alpha1[s] += delta;

    timeValue(t,
              nParam,
              alpha1,
              value,
              sumExpValue);

    costFunction(t,
                 value,
                 sumExpValue,
                 w,
                 c1);

    fdGrad(s) = (c1 - c0) / delta;
  }

  for (Index i = 0; i < nParam; ++i) {
    computedGrad(i) = computedGradVec[i];
  }

  ASSERT_EQ(true, computedGrad.isApprox(fdGrad, epsilon));
}

TEST(FunctionalComputation, costAndGrad) {
  Index nTime = 4;
  Index nParam = 4;
  Real delta = epsilon;

  Index nSub = nParam / 2;

  Vector<Real> t(nTime);
  t << 0., 1., 2., 3.;

  Vector<std::set<Index> > w(nSub);
  w(0).insert(0);
  w(0).insert(1);
  w(1).insert(2);
  w(1).insert(3);

  double alpha0[] = {-1., 0.5, 1., -0.5};

  Matrix<Real> value;
  Vector<Real> sumExpValue;
  Vector<Real> fdGrad(nParam); // finite differences gradient
  Vector<Real> computedGrad(nParam); // analytical gradient
  double computedGradVec[nParam];
  Real c0; // base cost

  timeValue(t,
            nParam,
            alpha0,
            value,
            sumExpValue);

  costFunction(t,
               value,
               sumExpValue,
               w,
               c0);

  gradCostFunction(t,
                   value,
                   sumExpValue,
                   w,
                   computedGradVec);

  for (Index s = 0; s < nParam; ++s) {
    Real c1;
    double alpha1[nParam];
    for (Index i = 0; i < nParam; ++i) {
      alpha1[i] = alpha0[i];
    }
    alpha1[s] += delta;

    timeValue(t,
              nParam,
              alpha1,
              value,
              sumExpValue);

    costFunction(t,
                 value,
                 sumExpValue,
                 w,
                 c1);

    fdGrad(s) = (c1 - c0) / delta;
  }

  for (Index i = 0; i < nParam; ++i) {
    computedGrad(i) = computedGradVec[i];
  }

  ASSERT_EQ(true, computedGrad.isApprox(fdGrad, 1e-4));
}

TEST(FunctionalComputation, hessian) {
  Index nTime = 4;
  Index nParam = 4;
  Real delta = 1e-5;

  Index nSub = nParam / 2;

  Vector<Real> t(nTime);
  Vector<std::set<Index> > w(nSub);
  for (Index i = 0; i < nTime; ++i) {
    t(i) = i;
    if (i < nTime / 2) {
      w(0).insert(i);
    }
    else {
      w(1).insert(i);
    }
  }

  double alpha0[] = {-1., 0.5, 1., -0.5};

  Matrix<Real> value;
  Vector<Real> sumExpValue;
  Matrix<Real> fdHessian(nParam, nParam); // finite differences gradient
  Matrix<Real> computedHessian; // analytical gradient
  Real c00; // base cost

  timeValue(t,
            nParam,
            alpha0,
            value,
            sumExpValue);

  costFunction(t,
               value,
               sumExpValue,
               w,
               c00);

  hessianCostFunction(t,
                      value,
                      sumExpValue,
                      w,
                      computedHessian);

  for (Index row = 0; row < nParam; ++row) {
    for (Index col = 0; col < nParam; ++col) {
      Real c01;
      Real c10;
      Real c11;

      double alpha1[nParam];
      for (Index i = 0; i < nParam; ++i) {
        alpha1[i] = alpha0[i];
      }
      alpha1[row] += delta;

      timeValue(t,
                nParam,
                alpha1,
                value,
                sumExpValue);
      costFunction(t,
                   value,
                   sumExpValue,
                   w,
                   c10);

      for (Index i = 0; i < nParam; ++i) {
        alpha1[i] = alpha0[i];
      }
      alpha1[col] += delta;

      timeValue(t,
                nParam,
                alpha1,
                value,
                sumExpValue);
      costFunction(t,
                   value,
                   sumExpValue,
                   w,
                   c01);

      for (Index i = 0; i < nParam; ++i) {
        alpha1[i] = alpha0[i];
      }
      alpha1[row] += delta;
      alpha1[col] += delta;

      timeValue(t,
                nParam,
                alpha1,
                value,
                sumExpValue);
      costFunction(t,
                   value,
                   sumExpValue,
                   w,
                   c11);

      fdHessian(row, col) = (c11 - c01 - c10 + c00) / (delta * delta);
    }
  }

  ASSERT_EQ(true, computedHessian.isApprox(fdHessian, 1e-4));
}

TEST(FunctionalComputation, optimRealSimpleCaseNLOpt) {
  Index nTime = 100000;
  Index nSub = 2; // number of subregression in the generation / estimation phases
  Index nCoeff = 2; // order of each subregression
  Real xMax = 50.;
  Real alphaSlope = 0.5;

  Real alpha0 = xMax / 2.;
  Index nParam = nSub * 2; // regression order for

  Vector<Real> t(nTime);
  for (Index i = 0; i < nTime; ++i) {
    t(i) = i * xMax / nTime;
  }

  double alpha[] = { alpha0 * alphaSlope, -alphaSlope, // alpha is linearized in a single vector, for easier looping
                    -alpha0 * alphaSlope,  alphaSlope};

  Vector<std::set<Index> > w(nSub);
  Vector<Real> y(nTime, 0.);

  Matrix<Real> beta(nSub, nCoeff + 1);
  beta.row(0) <<  0.,  1., 0.; // y =  x      + N(0, 1)
  beta.row(1) << 50., -1., 0.; // y = -x + 50 + N(0, 1)

  Matrix<Real> logValue;
  Vector<Real> logSumExpValue;
  timeValue(t,
            nParam,
            alpha,
            logValue,
            logSumExpValue);

  MultinomialStatistic multi;
  NormalStatistic normal;
  UniformStatistic uni;

  Matrix<Real> kappa(nTime, nSub);
  for (Index i = 0; i < nTime; ++i) {
    kappa.row(i) = logValue.row(i).exp() / std::exp(logSumExpValue(i));
    Index currW = multi.sample(kappa.row(i));
    w(currW).insert(i); // sample the subregression

    for (Index p = 0; p < nCoeff; ++p) { // sample the y(t) value, knowing the subregression at t
      y(i) += beta(currW, p) * pow(t(i), p);
    }
    y(i) += normal.sample(0, beta(currW, nCoeff));
  }

  double estimatedAlpha[nParam];
  for (Index i = 0; i < nParam; ++i) {
    estimatedAlpha[i] = 0.;
  }
  CostData cData;
  cData.t_ = &t;
  cData.w_ = &w;
  cData.nParam_ = nParam;
  double minf;

  nlopt_opt opt;
  opt = nlopt_create(NLOPT_LD_LBFGS, nParam); /* algorithm and dimensionality */
  nlopt_set_max_objective(opt, optiFunc, &cData);
  nlopt_optimize(opt, estimatedAlpha, &minf);
  nlopt_destroy(opt);

  bool isApprox = true;
  for (Index i = 0; i < nParam; ++i) {
    if (0.1 < std::abs((estimatedAlpha[i] - alpha[i]) / alpha[i])) {
      isApprox = false;
      break;
    }
  }

  ASSERT_EQ(true, isApprox);
}

TEST(FunctionalComputation, removeMissingQuantile) {
  Index nInd = 1000;
  Index nTime = 1000;
  Index nSub = 3;

  Real min = -20.;
  Real max = 10.;

  Vector<Function> vecInd(nInd);
  Vector<Real> computedQuantile;

  UniformStatistic uni;

  for (Index i = 0; i < nInd; ++i) {
    Vector<Real> t(nTime);
    Vector<Real> x(nTime); // will not be used in this test
    Vector<std::set<Index> > w(nSub); // will not be used in this test, except to detect the number of subregressions

    for (Index currT = 0; currT < nTime; ++currT) {
      t(currT) = uni.sample(min, max);
    }

    vecInd(i).setVal(t, x, w);
  }

  globalQuantile(vecInd, computedQuantile);

  Vector<Real> expectedQuantile(nSub - 1);
  expectedQuantile << -10., 0.;

  ASSERT_EQ(true, computedQuantile.isApprox(expectedQuantile, 0.1));
}
