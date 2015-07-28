/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2013-2014

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
 *  Created on: Nov 15, 2013
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>,
 *              iovleff <S..._Dot_I..._At_stkpp_Dot_org> (see copyright for ...),
 *              Parmeet Bhatia <b..._DOT_p..._AT_gmail_Dot_com>
 **/

/** @file mixt_MixtureBridge.h
 *  @brief In this file .
 **/

#ifndef MIXT_MIXTUREBRIDGE_H
#define MIXT_MIXTUREBRIDGE_H

#include "../mixt_IMixture.h"
#include "../../Data/mixt_AugmentedData.h"
#include "mixt_CategoricalBridges.h"
#include "mixt_GaussianBridges.h"
#include "mixt_PoissonBridges.h"
#include "../../IO/mixt_IO.h"
#include "../../Various/mixt_Constants.h"
#include "../../Param/mixt_ConfIntParamStat.h"

namespace mixt
{

template<int Id,
         typename DataHandler,
         typename DataExtractor,
         typename ParamSetter,
         typename ParamExtractor>

class MixtureBridge : public IMixture
{
  public:
    // data type
    typedef typename BridgeTraits<Id>::Data Data;
    // augmented data type
    typedef typename BridgeTraits<Id>::AugData AugData;
    // statistics on DataStat computer
    typedef typename BridgeTraits<Id>::DataStat DataStat;
    // type of the data
    typedef typename BridgeTraits<Id>::Type Type;
    // type of Mixture
    typedef typename BridgeTraits<Id>::Mixture Mixture;
    // type of Sampler
    typedef typename BridgeTraits<Id>::Sampler Sampler;
    // type of Likelihood
    typedef typename BridgeTraits<Id>::Likelihood Likelihood;

    /** constructor.
     *  @param idName id name of the mixture
     *  @param nbCluster number of cluster
     **/
    MixtureBridge(std::string const& idName,
                  int nbClass,
                  Vector<int> const* p_zi,
                  const DataHandler* p_handler_,
                  DataExtractor* p_extractor,
                  const ParamSetter* p_paramSetter,
                  ParamExtractor* p_paramExtractor,
                  Real confidenceLevel) :
      IMixture(idName),
      p_zi_(p_zi),
      nbClass_(nbClass),
      param_(), // must be initialized here, as will immediately be resized in mixture_ constructor
      mixture_(nbClass,
               param_,
               p_zi),
      augData_(),
      nbSample_(0),
      confidenceLevel_(confidenceLevel),
      sampler_(augData_,
               param_,
               nbClass),
      dataStat_(augData_,
                confidenceLevel),
      paramStat_(param_,
                 confidenceLevel),
      likelihood_(param_,
                  augData_,
                  nbClass),
      p_handler_(p_handler_),
      p_dataExtractor_(p_extractor),
      p_paramSetter_(p_paramSetter),
      p_paramExtractor_(p_paramExtractor)
    {}
    /** copy constructor */
    MixtureBridge(MixtureBridge const& bridge) :
      IMixture(bridge),
      p_zi_(bridge.p_zi_),
      nbClass_(bridge.nbClass_),
      param_(bridge.param_),
      mixture_(bridge.mixture_),
      augData_(bridge.augData_),
      nbSample_(bridge.nbSample_),
      confidenceLevel_(bridge.confidenceLevel_),
      sampler_(bridge.sampler_),
      dataStat_(bridge.dataStat_),
      paramStat_(bridge.paramStat_),
      likelihood_(bridge.likelihood_),
      p_handler_(bridge.p_handler_),
      p_dataExtractor_(bridge.p_dataExtractor_),
      p_paramSetter_(bridge.p_paramSetter_),
      p_paramExtractor_(bridge.p_paramExtractor_)
    {
      mixture_.setData(augData_.data_);
    }

    /** This function will be defined to set the data into your data containers.
     *  To facilitate data handling, framework provide templated functions,
     *  that can be called directly to get the data.
     */
    std::string setDataParam(RunMode mode)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::setDataParam(), " << idName() << ", " << mixture_.model() << std::endl;
#endif
      std::string warnLog;
      p_handler_->getData(idName(),
                          augData_,
                          nbSample_,
                          paramStr_,
                          (mixture_.hasModalities()) ? (-minModality) : (0), // minModality offset for categorical models
                          warnLog);
      augData_.computeRange();
      std::string tempLog  = augData_.checkMissingType(mixture_.acceptedType()); // check if the missing data provided are compatible with the model
                  tempLog += augData_.sortAndCheckMissing(); // sort and check for duplicates in missing values descriptions
      if(tempLog.size() > 0) // check on the missing values description
      {
        std::stringstream sstm;
        sstm << "Variable " << idName() << " with model " << mixture_.model() << " has a problem with the descriptions of missing values.\n" << tempLog;
        warnLog += sstm.str();
      }
      else if (mixture_.checkMinVal() && augData_.dataRange_.min_ < mixture_.minVal()) // test the requirement for the data (and bounds) to be above a specified value
      {
        std::stringstream sstm;
        sstm << "Variable: " << idName() << " requires a minimum value of " << mixture_.minVal()
             << " in either provided values or bounds. The minimum value currently provided is : " << augData_.dataRange_.min_ << std::endl;
        warnLog += sstm.str();
      }
      else // minimum value requirements have been met, whether the mode is learning or prediction
      {
        mixture_.setData(augData_.data_);

        if (mode == learning_) // learning mode
        {
  #ifdef MC_DEBUG
          std::cout << "\tparam not set " << std::endl;
  #endif
          if (augData_.misCount_(present_) < minNbPresentValues) // Any variable with less than three samples will be rejected as not providing enough information for learning
          {
            std::stringstream sstm;
            sstm << "Variable: " << idName() << " only has " << augData_.misCount_(present_)
                 << " present values. Maybe there is an error in the data encoding. If the variable truly has less than "
                 << minNbPresentValues
                 << " present values, it should be removed from the study as it does not provide enough information." << std::endl;
            warnLog += sstm.str();
          }
          if (mixture_.hasModalities())
          {
            mixture_.setModalities(augData_.dataRange_.max_ + 1); // set the number of modalities
          }
        }
        else // predict mode
        {
          p_paramSetter_->getParam(idName(), // parameters are set using results from previous run
                                   param_);
          int nbModalities = param_.rows() / nbClass_; // number of parameters for each cluster
          if (mixture_.hasModalities()) // all modalities might not be present in the predict set, and as such the real data range from the learning set must be used
          {
            mixture_.setModalities(nbModalities);
          }
          paramStat_.setParamStorage(); // paramStatStorage_ is set now, using dimensions of param_, and will not be modified during predict run by the paramStat_ object
          // for some mixtures, there will be errors if the range of the data in prediction is different from the range of the data in learning
          // in the case of modalities, this can not be performed earlier, as the max val is computed at mixture_.setModalities(nbParam)
          if (mixture_.checkMaxVal() && mixture_.maxVal() < augData_.dataRange_.max_)
          {
            std::stringstream sstm;
            sstm << "Variable: " << idName() << " requires a maximum value of " << ((mixture_.hasModalities()) ? (minModality) : (0)) + mixture_.maxVal()
                 << " for the data during prediction. This maximum value usually corresponds to the maximum value used during the learning phase."
                 << " The maximum value in the data provided for prediction is : " << ((mixture_.hasModalities()) ? (minModality) : (0)) + augData_.dataRange_.max_ << std::endl;
            warnLog += sstm.str();
          }
          if (mixture_.hasModalities()) // now that predict observed values have been checked, the real data range must be used for all data
          {
            augData_.dataRange_.min_ = 0;
            augData_.dataRange_.max_ = nbModalities - 1;
            augData_.dataRange_.range_ = nbModalities;
          }
  #ifdef MC_DEBUG
          std::cout << "\tparam set " << std::endl;
          std::cout << "\tnbParam: " << nbModalities << std::endl;
          std::cout << "\tparam_: " << param_ << std::endl;
  #endif
        }

        dataStat_.resizeStatStorage(nbSample_);
      }

      return warnLog;
    }
    /** This function must be defined for simulation of all the latent variables
     * and/or missing data excluding class labels. The class labels will be
     * simulated by the framework itself because to do so we have to take into
     * account all the mixture laws. do nothing by default.
     */
    virtual void samplingStep(int ind)
    {
      sampler_.sampleIndividual(ind,
                                (*p_zi())(ind));
    }
    /** This function is equivalent to Mstep and must be defined to update parameters.
     */
    virtual std::string mStep(DegeneracyType& deg)
    {
      std::string warn = mixture_.mStep(deg);
      if (warn.size() > 0)
      {
        warn =   std::string("Error in variable ")
               + idName_
               + ": \n"
               + warn;
      }
      return warn;
    }
    /** This function should be used to store any results during the burn-in period
     *  @param iteration Provides the iteration number during the burn-in period
     */
    virtual void storeSEMBurnIn(int iteration,
                               int iterationMax)
    {
#ifdef MC_LOG
      std::stringstream fileNameA;
      fileNameA << "out/log/";
      fileNameA << idName();
      fileNameA << "-";
      fileNameA << iteration;
      fileNameA << "-param.csv";
      writeDataCsv(fileNameA.str(), param_);

      std::stringstream fileNameB;
      fileNameB << "out/log/";
      fileNameB << idName();
      fileNameB << "-";
      fileNameB << iteration;
      fileNameB << "-data.csv";
      writeDataCsv(fileNameB.str(), augData_.data_);
#endif
    }
    /** This function should be used to store any intermediate results during
     *  various iterations after the burn-in period.
     *  @param iteration Provides the iteration number beginning after the burn-in period.
     */
    virtual void storeSEMRun(int iteration,
                             int iterationMax)
    {
      paramStat_.sampleParam(iteration,
                             iterationMax);
      if (iteration == iterationMax)
      {
        paramStat_.setExpectationParam(); // set parameter to mode / expectation
        mixture_.checkParam(); // since estimated have not been estimated by maximum likelihood, but by statistics on the SEM chain, the values must be checked
#ifdef MC_DEBUG
        std::cout << "MixtureBridge::storeSEMRun" << std::endl;
        int nbModalities = param_.cols() / nbCluster_;
        for (int p = 0; p < nbModalities; ++p)
        {
          Real sum = 0.;
          for (int k = 0; k < nbCluster_; ++k)
          {
            sum += param_[k * nbModalities + p];
          }
          if (sum < epsilon)
          {
            std::cout << "probability of modality: " << p << " is 0 in every classes" << std::endl;
          }
        }
#endif
      }
    }

    virtual void storeGibbsRun(int sample,
                               int iteration,
                               int iterationMax)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::storeData, for " << idName()
                << ", sample: " << sample
                << ", iteration: " << iteration
                << ", iterationMax: " << iterationMax << std::endl;
#endif
      dataStat_.sampleVals(sample,
                                   iteration,
                                   iterationMax);
      if (iteration == iterationMax)
      {
        dataStat_.imputeData(sample); // impute the missing values using empirical mean or mode, depending of the model
      }
    }

    /**
     * This function must be defined to return the completed likelihood, using the current values for
     * unknown values
     * @return the completed log-likelihood
     */
    virtual Real lnCompletedProbability(int i, int k)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::lnCompletedLikelihood(), getParameters" << std::endl;
      std::cout << "\tidName: " << idName() << std::endl;
      std::cout << "\tparam: " << std::endl;
      std::cout << param_ << std::endl;
#endif
      return likelihood_.lnCompletedProbability(i, k);
    }

    /**
     * This function must be defined to return the observed likelihood
     * @return the observed log-likelihood
     */
    virtual Real lnObservedProbability(int i, int k)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::lnObservedLikelihood(), getParameters" << std::endl;
      std::cout << "\tidName: " << idName() << std::endl;
      std::cout << "\tparam: " << std::endl;
      std::cout << param_ << std::endl;
#endif
      return likelihood_.lnObservedProbability(i, k);
    }
    /** This function must return the number of free parameters.
     *  @return Number of free parameters
     */
    virtual int nbFreeParameter() const
    {
      return mixture_.computeNbFreeParameters();
    }
    /** This function can be used to write summary of parameters on to the output stream.
     * @param out Stream where you want to write the summary of parameters.
     */
    virtual void writeParameters(std::ostream& out) const
    {
      mixture_.writeParameters(out);
    }

    virtual void exportDataParam() const
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge: exportDataParam, idName(): " << idName() << std::endl;
#endif
      p_dataExtractor_->exportVals(idName(),
                                   augData_,
                                   dataStat_.getDataStatStorage()); // export the obtained data using the DataExtractor
      p_paramExtractor_->exportParam(idName(),
                                     paramStat_.getStatStorage(),
                                     paramStat_.getLogStorage(),
                                     mixture_.paramNames(),
                                     confidenceLevel_);
    }

    /** This function can be used in derived classes to get class labels from the framework.
     *  @return Pointer to zi.
     */
    Vector<int> const* p_zi() const {return p_zi_;}

    bool possibleNullProbability() const {return mixture_.possibleNullProbability();}

    void removeMissing() {augData_.removeMissing();}

  protected:
    /** Pointer to the zik class label */
    Vector<int> const* p_zi_;
    /** Number of classes */
    int nbClass_;
    /** Current parameters of the mixture_ */
    Vector<Real> param_;
    /** The simple mixture to bridge with the composer */
    Mixture mixture_;
    /** The augmented data set */
    AugData augData_;
    /** Parameters transmitted by the user */
    std::string paramStr_;
    /** number of samples in the data set*/
    int nbSample_;
    /** confidence level used in computation of parameters and missing values statistics */
    Real confidenceLevel_;
    /** Sampler to generate values */
    Sampler sampler_;
    /** Statistics computer for missing data */
    DataStat dataStat_;
    /** Statistics storage for parameters */
    ConfIntParamStat<Real> paramStat_;
    /** Computation of the observed likelihood */
    Likelihood likelihood_;
    /** Pointer to the data handler */
    const DataHandler* p_handler_;
    /** Pointer to the data extractor */
    DataExtractor* p_dataExtractor_;
    /** Pointer to the param setter */
    const ParamSetter* p_paramSetter_;
    /** Pointer to the parameters extractor */
    ParamExtractor* p_paramExtractor_;
};

} // namespace mixt

#endif /* MIXT_MIXTUREBRIDGE_H */
