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
 *  Created on: July 2, 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 *              Serge IOVLEFF <serge.iovleff@inria.fr>
 **/

#include "mixt_MixtureComposer.h"
#include "../Mixture/mixt_IMixture.h"
#include "../IO/mixt_IO.h"
#include "../Various/mixt_Timer.h"
#include "../LinAlg/mixt_LinAlg.h"
#include "../Various/mixt_Various.h"

namespace mixt {

MixtureComposer::MixtureComposer(Index nbInd,
                                 Index nbClass,
                                 Real confidenceLevel) :
    idName_("z_class"),
    nbClass_(nbClass),
    nbInd_(nbInd),
    nbVar_(0),
    prop_(nbClass),
    tik_(nbInd,
         nbClass),
    sampler_(*this,
             zClassInd_,
             tik_,
             nbClass),
    paramStat_(prop_,
               confidenceLevel),
    dataStat_(zClassInd_),
    confidenceLevel_(confidenceLevel) {
  std::cout << "MixtureComposer::MixtureComposer, nbInd: " << nbInd << ", nbClass: " << nbClass << std::endl;
  zClassInd_.setIndClass(nbInd, nbClass);
  initializeProp(); // default values that will be overwritten either by pStep (learning), or eStepObserved (prediction)
  initializeTik();
}

MixtureComposer::~MixtureComposer()
{
  for (MixtIterator it = v_mixtures_.begin() ; it != v_mixtures_.end(); ++it)
  {
    delete (*it);
  }
}

void MixtureComposer::initializeProp()
{
  prop_     = 1./(Real)nbClass_;
}

void MixtureComposer::initializeTik()
{
  tik_      = 1./(Real)nbClass_;
}

Real MixtureComposer::lnObservedProbability(int i, int k) const {
  Real sum = std::log(prop_[k]);

  for (ConstMixtIterator it = v_mixtures_.begin() ; it != v_mixtures_.end(); ++it) {
    Real logProba = (*it)->lnObservedProbability(i, k);
    sum += logProba;
  }

  return sum;
}

void MixtureComposer::printObservedTik() const {
  Matrix<Real> lnComp(nbInd_,
                      nbClass_);

  for (Index k = 0; k < nbClass_; ++k) {
    for (Index i = 0; i < nbInd_; ++i) {
      lnComp(i, k) = lnObservedProbability(i, k);
    }
  }

  Matrix<Real> observedTik(nbInd_, nbClass_);
  for (Index i = 0; i < nbInd_; ++i) { // sum is inside a log, hence the numerous steps for the computation
    RowVector<Real> dummy;
    observedTik.row(i).logToMulti(lnComp.row(i));
  }
}

Real MixtureComposer::lnObservedLikelihood() {
  Real lnLikelihood = 0.;
  Matrix<Real> lnComp(nbInd_,
                      nbClass_);

  for (Index k = 0; k < nbClass_; ++k) {
    for (Index i = 0; i < nbInd_; ++i) {
      lnComp(i, k) = lnObservedProbability(i, k);
    }
  }

  for (Index i = 0; i < nbInd_; ++i) { // sum is inside a log, hence the numerous steps for the computation
    RowVector<Real> dummy;
    lnLikelihood += dummy.logToMulti(lnComp.row(i));
  }

  return lnLikelihood;
}

Real MixtureComposer::lnCompletedLikelihood()
{
  Real lnLikelihood = 0.;

  for (Index i = 0; i < nbInd_; ++i) { // Compute the completed likelihood for the complete mixture model, using the completed data
    lnLikelihood += lnObservedProbability(i, zClassInd_.zi().data_(i));
  }

  return lnLikelihood;
}

Real MixtureComposer::lnCompletedProbability(int i, int k) const {
  Real sum = std::log(prop_[k]);
  Real logProba;

  for (ConstMixtIterator it = v_mixtures_.begin() ; it != v_mixtures_.end(); ++it) {
    logProba = (*it)->lnCompletedProbability(i, k);
    sum += logProba;
  }

  return sum;
}

void MixtureComposer::mStep(EstimatorType bias) {
  pStep(); // computation of z_ik frequencies, which correspond to ML estimator of proportions
  for (MixtIterator it = v_mixtures_.begin() ; it != v_mixtures_.end(); ++it) {
    (*it)->mStep(bias); // call mStep on each variable
  }
}

void MixtureComposer::sStepCheck() {
  for (Index i = 0; i < nbInd_; ++i) {
    sStepCheck(i);
  }
}

void MixtureComposer::sStepCheck(int i) {
  sampler_.sStepCheck(i);
}

void MixtureComposer::sStepNoCheck() {
  #pragma omp parallel for
  for (Index i = 0; i < nbInd_; ++i) {
    sStepNoCheck(i);
  }
}

void MixtureComposer::sStepNoCheck(int i) {
  sampler_.sStepNoCheck(i);
}

void MixtureComposer::eStep() {
  #pragma omp parallel for
  for (Index i = 0; i < nbInd_; ++i) {
    eStepInd(i);
  }

}

void MixtureComposer::eStepInd(int i) {
  RowVector<Real> lnComp(nbClass_);
  for (Index k = 0; k < nbClass_; k++) {
    lnComp(k) = lnCompletedProbability(i, k);
  }

  tik_.row(i).logToMulti(lnComp);
}

void MixtureComposer::pStep() {
  for (Index i = 0; i < zClassInd_.zi().data_.rows(); ++i) {
    prop_(zClassInd_.zi().data_(i)) += 1.;
  }
  prop_ = prop_ / prop_.sum();
}

void MixtureComposer::writeParameters() const {
  std::cout << "Composer nbFreeParameter = " << nbFreeParameters() << std::endl;
  std::cout << "Composer proportions = " << itString(prop_) << std::endl;

  for (ConstMixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    std::cout << "Parameters of " << (*it)->idName() << "\n";
    (*it)->writeParameters();
  }
}

int MixtureComposer::nbFreeParameters() const
{
  int sum = nbClass_ - 1; // proportions
  for (ConstMixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it)
  {
    sum+= (*it)->nbFreeParameter();
  }
  return sum;
}

void MixtureComposer::samplingStepCheck() {
  for (Index i = 0; i < nbInd_; ++i) {
    samplingStepCheck(i);
  }
}

void MixtureComposer::samplingStepCheck(int i) {
  for (MixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    (*it)->samplingStepCheck(i);
  }
}

void MixtureComposer::samplingStepNoCheck() {
  #pragma omp parallel for
  for (Index i = 0; i < nbInd_; ++i) {
    samplingStepNoCheck(i);
  }
}

void MixtureComposer::samplingStepNoCheck(int i) {
  for (MixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    (*it)->samplingStepNoCheck(i);
  }
}

int MixtureComposer::checkSampleCondition(std::string* warnLog) const {
  if (warnLog == NULL) { // if no description of the error is expected, to speed the treatment
    if (checkNbIndPerClass() == 0) {
      return 0;
    }
    for (ConstMixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
      if ((*it)->checkSampleCondition() == 0) {
        return 0; // no need for log generation -> faster evaluation of checkSampleCondition
      }
    }
  }
  else { // if error description is expected
    int probaCondition = 1; // proba of condition on data given the completed data
    std::string indLog;
    probaCondition *= checkNbIndPerClass(&indLog);
    *warnLog += indLog;
    for (ConstMixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
      std::string mixtLog;
      int currProba = (*it)->checkSampleCondition(&mixtLog); // the global warnLog is not passed directly to the mixture, to avoid accidental wiping
      probaCondition *= currProba;
      *warnLog += mixtLog;
    }

    return probaCondition;
  }

  return 1;
}

int MixtureComposer::checkNbIndPerClass(std::string* warnLog) const
{
  for (Index k = 0; k < nbClass_; ++k) {
    if (zClassInd_.classInd()(k).size() > 0) {
      continue;
    }
    else {
      if (warnLog != NULL) {
        std::stringstream sstm;
        sstm << "MixtureComposer::checkNbIndPerClass, at least one class is empty. Did you provide more individuals "
             << "that the number of classes ?" << std::endl;
        *warnLog += sstm.str();
      }

      return 0;
    }
  }

  return 1;
}

void MixtureComposer::storeSEMRun(int iteration,
                                  int iterationMax) {
  paramStat_.sampleParam(iteration,
                         iterationMax);
  if (iteration == iterationMax){
    paramStat_.setExpectationParam(); // replace pi by the median values
  }
  for (MixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    (*it)->storeSEMRun(iteration,
                       iterationMax);
  }
}

void MixtureComposer::storeGibbsRun(int ind,
                                    int iteration,
                                    int iterationMax) {
  dataStat_.sampleVals(ind,
                       iteration,
                       iterationMax);

  if (iteration == iterationMax) {
    dataStat_.imputeData(ind); // impute the missing values using empirical mean or mode, depending of the model
  }

  for (MixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    (*it)->storeGibbsRun(ind,
                         iteration,
                         iterationMax);
  }
}

void MixtureComposer::registerMixture(IMixture* p_mixture)
{
  v_mixtures_.push_back(p_mixture);
  ++nbVar_;
}

void MixtureComposer::gibbsSampling(GibbsSampleData sample,
                                    int nbGibbsIter,
                                    int group,
                                    int groupMax) {
  Timer myTimer;
  if (sample == sampleData_) {
    myTimer.setName("Gibbs: run (individuals count as iterations)");
  }
  else {
    myTimer.setName("Gibbs: burn-in (individuals count as iterations)");
  }

  #pragma omp parallel for
  for (Index i = 0; i < nbInd_; ++i) {
    myTimer.iteration(i, nbInd_ - 1);
    writeProgress(group,
                  groupMax,
                  i,
                  nbInd_ - 1);

    for (int iterGibbs = 0; iterGibbs < nbGibbsIter; ++iterGibbs) {
      eStepInd(i);

      sStepNoCheck(i);
      samplingStepNoCheck(i);

      if (sample == sampleData_) {
        storeGibbsRun(i,
                      iterGibbs,
                      nbGibbsIter - 1);
      }
    }
  }
}

std::vector<std::string> MixtureComposer::paramName() const
{
  std::vector<std::string> names(nbClass_);
  for (Index k = 0; k < nbClass_; ++k)
  {
    std::stringstream sstm;
    sstm << "k: "
         << k + minModality;
    names[k] = sstm.str();
  }

  return names;
}

std::vector<std::string> MixtureComposer::mixtureName() const
{
  std::vector<std::string> names(nbVar_);
  for (Index j = 0; j < nbVar_; ++j)
  {
    names[j] = v_mixtures_[j]->idName();
  }

  return names;
}

void MixtureComposer::removeMissing(InitParam algo) {
  initializeTik();
  sStepNoCheck(); // uniform initialization of z

  for(MixtIterator it = v_mixtures_.begin(); it != v_mixtures_.end(); ++it) {
    (*it)->removeMissing(algo);
  }
}

void MixtureComposer::E_kj(Matrix<Real>& ekj) const {
  ekj.resize(nbClass_, nbVar_);
  ekj = 0.;

  for (Index i = 0; i < nbInd_; ++i) {
    for(Index j = 0; j < nbVar_; ++j) {
      Vector<Real> lnP(nbClass_); // ln(p(z_i = k, x_i^j))
      Vector<Real> t_ik_j(nbClass_); // p(z_i = k / x_i^j)
      for (Index k = 0; k < nbClass_; ++k) {
        lnP(k) = std::log(prop_(k)) + v_mixtures_[j]->lnObservedProbability(i, k);
      }
      t_ik_j.logToMulti(lnP); // "observed" t_ik, for the variable j
      Vector<Real> t_ink_j = 1. - t_ik_j; // The nj means: "all classes but k".

      for (Index k = 0; k < nbClass_; ++k) {
        Real p, nP;

        if (epsilon < t_ik_j(k)) {
          p = - t_ik_j(k) * std::log(t_ik_j (k));
        }
        else {
          p = 0.;
        }

        if (epsilon < t_ink_j(k)) {
          nP = - t_ink_j(k) * std::log(t_ink_j(k));
        }
        else {
          nP = 0.;
        }

        ekj(k, j) += p + nP;
      }
    }
  }
}

void MixtureComposer::IDClass(Matrix<Real>& idc) const {
  idc.resize(nbClass_, nbVar_);

  if (nbClass_ > 1) {
    Matrix<Real> ekj;
    E_kj(ekj);

    for (Index k = 0; k < nbClass_; ++k) {
      Real min = ekj.row(k).minCoeff();
      Real max = ekj.row(k).maxCoeff();

      for(Index j = 0; j < nbVar_; ++j) {
        idc(k, j) = (max - ekj(k, j)) / (max - min);
  //      idc(k, j) = 1. - ekj(k, j) / ekj.row(k).sum();
      }
    }
  }
  else {
    idc = 1.;
  }

}

void MixtureComposer::printClassInd() const {
  zClassInd_.printState();
}

} /* namespace mixt */
