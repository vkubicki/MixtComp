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

namespace mixt
{

template<int Id,
         typename DataHandler,
         typename DataExtractor,
         typename ParamSetter,
         typename ParamExtractor>

class MixtureBridge : public mixt::IMixture
{
  public:
    // data type
    typedef typename BridgeTraits<Id>::Data Data;
    // augmented data type
    typedef typename BridgeTraits<Id>::AugData AugData;
    // statistics on DataStat computer
    typedef typename BridgeTraits<Id>::DataStatComputer DataStatComputer;
    // type of DataStat storage
    typedef typename BridgeTraits<Id>::DataStatStorage DataStatStorage;
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
                  int nbCluster,
                  Vector<int> const* p_zi,
                  const DataHandler* p_handler_,
                  DataExtractor* p_extractor,
                  const ParamSetter* p_paramSetter,
                  ParamExtractor* p_paramExtractor,
                  Real confidenceLevel) :
      mixt::IMixture(idName,
                     p_zi,
                     nbCluster),
      mixture_(nbCluster),
      m_augDataij_(),
      nbSample_(0),
      nbVariable_(0),
      confidenceLevel_(confidenceLevel),
      sampler_(&m_augDataij_,
               getParam(),
               nbCluster),
      dataStatComputer_(&m_augDataij_,
                        &dataStatStorage_,
                        confidenceLevel),
      paramStat_(&param_,
                 &paramStatStorage_,
                 &paramLogStorage_,
                 confidenceLevel),
      likelihood_(getParam(),
                  getData(),
                  getDataStatStorage(),
                  nbCluster),
      p_handler_(p_handler_),
      p_dataExtractor_(p_extractor),
      p_paramSetter_(p_paramSetter),
      p_paramExtractor_(p_paramExtractor)
      // dataStatStorage_ is an empty STK::Array2D at construction
    {}
    /** copy constructor */
    MixtureBridge(MixtureBridge const& bridge) :
      mixt::IMixture(bridge),
      mixture_(bridge.mixture_),
      m_augDataij_(bridge.m_augDataij_),
      nbSample_(bridge.nbSample_),
      nbVariable_(bridge.nbVariable_),
      confidenceLevel_(bridge.confidenceLevel_),
      sampler_(bridge.sampler_),
      dataStatComputer_(bridge.dataStatComputer_),
      paramStat_(bridge.paramStat_),
      likelihood_(bridge.likelihood_),
      p_handler_(bridge.p_handler_),
      p_dataExtractor_(bridge.p_dataExtractor_),
      p_paramSetter_(bridge.p_paramSetter_),
      p_paramExtractor_(bridge.p_paramExtractor_),
      dataStatStorage_(bridge.dataStatStorage_)
    {
      mixture_.setData(m_augDataij_.data_);
      mixture_.initializeModel();
    }
    /** @brief Initialize the model before its use by the composer.
     *  The parameters values are set to their default values if the mixture_ is
     *  newly created. if MixtureBridge::initializeModel is used during a
     *  cloning, mixture class have to take care of the existing values of the
     *  parameters.
     **/
    virtual void initializeStep()
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::initializeStep()"  << std::endl;
      std::cout << "\tidName(): " << idName() << std::endl;
#endif
      mixture_.setMixtureParameters(p_zi());
      mixture_.initializeStep();
#ifdef MC_DEBUG
      mixture_.getParameters(param_);
      std::cout << "param_, after initializeStep: " << std::endl;
      std::cout << param_ << std::endl;
#endif
    }
    /** This function will be defined to set the data into your data containers.
     *  To facilitate data handling, framework provide templated functions,
     *  that can be called directly to get the data.
     */
    void setDataParam(std::string& warnLog)
    {
#ifdef MC_DEBUG
        std::cout << "MixtureBridge::setDataParam(), idName(): " << idName() << std::endl;
#endif
      p_handler_->getData(idName(),
                          m_augDataij_,
                          nbSample_,
                          nbVariable_,
                          paramStr_,
                          warnLog);
      m_augDataij_.computeRange();
      if (mixture_.checkMinVal() && m_augDataij_.dataRange_.min_ < mixture_.minVal()) // test the requirement for the data (and bounds) to be above a specified value
      {
        std::stringstream sstm;
        sstm << "Variable: " << idName() << " requires a minimum value of " << mixture_.minVal()
             << " in either provided values or bounds. The minimum value currently provided is : " << m_augDataij_.dataRange_.min_ << std::endl;
        warnLog += sstm.str();
      }
      else // minimum value requirements have been met
      {
        m_augDataij_.removeMissing();
        p_paramSetter_->getParam(idName(),
                                 param_);
        mixture_.setData(m_augDataij_.data_);

        // test if parameters are provided, in that case the mode is prediction, not learning and setModalities
        // must use the range provided by the ParamSetter
        if (param_.rows() > 0 && param_.cols() > 0)
        {
          int nbParam = param_.rows() / nbCluster_; // number of parameters for each cluster
          mixture_.setModalities(nbParam);
          mixture_.initializeModel(); // resize the parameters inside the mixture, to be ready for the mStep to come later
          mixture_.setParameters(param_);
          paramStatStorage_.resize(param_.rows(),
                                   1); // no quantiles have to be computed for imported parameters, hence the single column
          paramStatStorage_.col(0) = param_;
          // for some mixtures, there will be errors if the range of the data in prediction is different from the range of the data in learning
          if (mixture_.checkMaxVal() && mixture_.maxVal() < m_augDataij_.dataRange_.max_)
          {
            std::stringstream sstm;
            sstm << "Variable: " << idName() << " requires a maximum value of " << mixture_.maxVal()
                 << " for the data during prediction. This maximum value usually corresponds to the maximum value used during the learning phase."
                 << " The maximum value in the data provided for prediction is : " << m_augDataij_.dataRange_.max_ << std::endl;
            warnLog += sstm.str();
          }
  #ifdef MC_DEBUG
          std::cout << "\tparam set " << std::endl;
          std::cout << "\tnbParam: " << nbParam << std::endl;
          std::cout << "\tparam_: " << param_ << std::endl;
  #endif
        }
        else // code is in learning mode. setModalities must use the range provided by the data
        {
  #ifdef MC_DEBUG
          std::cout << "\tparam not set " << std::endl;
  #endif
          if (m_augDataij_.nbPresent_ < 3) // Any variable with less than three samples will be rejected as not privinding enough information for learning
          {
            std::stringstream sstm;
            sstm << "Variable: " << idName() << " only has " << m_augDataij_.nbPresent_
                 << " present values. Maybe there is an error in the data encoding. If the variable truly has less than"
                 << "3 samples, it should be removed from the study as it does not provide enough information." << std::endl;
            warnLog += sstm.str();
          }
          mixture_.setModalities(m_augDataij_.dataRange_.max_);
          mixture_.initializeModel(); // resize the parameters inside the mixture, to be ready for the mStep to come later
        }
        dataStatStorage_.resize(nbSample_,
                                nbVariable_);
        mixture_.paramNames(paramNames_); // set the parameters names to be used for export
      }
    }
    /** This function must be defined for simulation of all the latent variables
     * and/or missing data excluding class labels. The class labels will be
     * simulated by the framework itself because to do so we have to take into
     * account all the mixture laws. do nothing by default.
     */
    virtual void samplingStep(int i)
    {
      sampler_.sampleIndividual(i,
                                (*p_zi())(i));
    }
    /** This function is equivalent to Mstep and must be defined to update parameters.
     */
    virtual std::string mStep()
    {
      std::string warn = mixture_.mStep();
      mixture_.getParameters(param_); // update the parameters
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
      writeDataCsv(fileNameB.str(), m_augDataij_.data_);
#endif
    }
    /** This function should be used to store any intermediate results during
     *  various iterations after the burn-in period.
     *  @param iteration Provides the iteration number beginning after the burn-in period.
     */
    virtual void storeSEMRun(int iteration,
                             int iterationMax)
    {
      mixture_.getParameters(param_);
      paramStat_.sampleParam(iteration,
                             iterationMax);
      if (iteration == iterationMax)
      {
        // reinject the SEM estimated parameters into the mixture
        paramStat_.setExpectationParam();
        mixture_.setParameters(param_);
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
      dataStatComputer_.sampleVals(sample,
                                   iteration,
                                   iterationMax);
      if (iteration == iterationMax)
      {
        dataStatComputer_.imputeData(sample);
      }
    }

    /**
     *  This step can be used by developer to finalize any thing. It will be called only once after we
     * finish running the SEM-gibbs algorithm.
     */
    virtual void finalizeStep() {}
    /**
     * This function must be defined to return the posterior probability (PDF)
     * for corresponding sample i and cluster k.
     * @param i Sample number
     * @param k Cluster number
     * @return the log-component probability
     */
    virtual double lnComponentProbability(int i, int k)
    {
      return mixture_.lnComponentProbability(i, k);
    }

    /**
     * This function must be defined to return the completed likelihood
     * @return the observed log-likelihood
     */
    virtual void lnCompletedLikelihood(Matrix<Real>* lnComp)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::lnCompletedLikelihood(), getParameters" << std::endl;
      std::cout << "\tidName: " << idName() << std::endl;
      std::cout << "\tparam: " << std::endl;
      std::cout << param_ << std::endl;
#endif
      likelihood_.lnCompletedLikelihood(lnComp);
    }

    /**
     * This function must be defined to return the observed likelihood
     * @return the observed log-likelihood
     */
    virtual void lnObservedLikelihood(Matrix<Real>* lnComp)
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge::lnObservedLikelihood(), getParameters" << std::endl;
      std::cout << "\tidName: " << idName() << std::endl;
      std::cout << "\tparam: " << std::endl;
      std::cout << param_ << std::endl;
#endif
      likelihood_.lnObservedLikelihood(lnComp);
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
    /** This function must return the number of variables.
     *  @return Number of variables
     */
    virtual int nbVariable() const
    {
      return mixture_.nbVariable();
    }
    virtual void writeParameters(std::ostream& out) const
    {
      mixture_.writeParameters(out);
    }
    /** Utility function to use in mode debug in order to test that the
     *  model is well initialized. */
    int checkModel() const
    {
      return mixture_.checkModel();
    }

    virtual AugmentedData<Data> const* getData() const
    {
      return &m_augDataij_;
    }

    virtual const Vector<Real>* getParam() const
    {
      return &param_;
    }

    virtual const DataStatStorage* getDataStatStorage() const
    {
      return &dataStatStorage_;
    }

    virtual const Matrix<Real>* getParamStatStorage() const
    {
      return &paramStatStorage_;
    }

    virtual const Matrix<Real>* getParamLogStorage() const
    {
      return &paramLogStorage_;
    }

    virtual void exportDataParam() const
    {
#ifdef MC_DEBUG
      std::cout << "MixtureBridge: exportDataParam, idName(): " << idName() << std::endl;
#endif
      p_dataExtractor_->exportVals(idName(),
                                   getData(),
                                   getDataStatStorage()); // export the obtained data using the DataExtractor
      p_paramExtractor_->exportParam(idName(),
                                     getParamStatStorage(),
                                     getParamLogStorage(),
                                     paramNames_,
                                     confidenceLevel_);
    }

  protected:
    /** The stkpp mixture to bridge with the composer */
    Mixture mixture_;
    /** The augmented data set */
    AugData m_augDataij_;
    /** Current parameters of the STK Mixture */
    Vector<Real> param_;
    /** Parameters transmitted by the user */
    std::string paramStr_;
    /** number of samples in the data set*/
    int nbSample_;
    /** number of variables in the data set */
    int nbVariable_;
    /** confidence level used in computation of parameters and missing values statistics */
    Real confidenceLevel_;
    /** Sampler to generate values */
    Sampler sampler_;
    /** Statistics computer for missing data */
    DataStatComputer dataStatComputer_;
    /** Statistics storage for parameters */
    SimpleParamStat paramStat_;
    /** Vector of parameters names */
    std::vector<std::string> paramNames_;
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

    /** Statistics storage for missing data */
    DataStatStorage dataStatStorage_;
    /** Statistics storage for parameters */
    Matrix<Real> paramStatStorage_;
    /** Log for sampled parameters */
    Matrix<Real> paramLogStorage_;
};

} // namespace mixt

#endif /* MIXT_MIXTUREBRIDGE_H */
