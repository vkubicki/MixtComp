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
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 *              iovleff, S..._Dot_I..._At_stkpp_Dot_org (see copyright for ...)
 *              Parmeet Bhatia <b..._DOT_p..._AT_gmail_Dot_com>
 **/

#ifndef MIXT_IMODELMIXTUREBASE_H
#define MIXT_IMODELMIXTUREBASE_H

#include "../LinAlg/mixt_LinAlg.h"
#include "../Various/mixt_Clust_Util.h"
#include "../Statistic/mixt_MultinomialStatistic.h"

namespace mixt
{

/** @ingroup Clustering
 *  @brief Base class for Mixture (composed) model.
 *
 * In statistics, a mixture model is a probabilistic model for representing
 * the presence of sub-populations within an overall population, without
 * requiring that an observed data-set should identify the sub-population to
 * which an individual observation belongs. Formally a mixture model
 * corresponds to the mixture distribution that represents the probability
 * distribution of observations in the overall population. However, while
 * problems associated with "mixture distributions" relate to deriving the
 * properties of the overall population from those of the sub-populations,
 * "mixture models" are used to make statistical inferences about the
 * properties of the sub-populations given only observations on the pooled
 * population, without sub-population-identity information.
 *
 * Some ways of implementing mixture models involve steps that attribute
 * postulated sub-population-identities to individual observations (or weights
 * towards such sub-populations), in which case these can be regarded as types
 * unsupervised learning or clustering procedures. However not all inference
 * procedures involve such steps.
 *
 * In this interface we assume there is an underline generative model that will
 * be estimated using either an EM, SEM or CEM algorithm.
 * All mixture parameters: proportions, Tik, Zi and components are accessed by
 * pointer. These parameters are created using the method
 * @code
 *   void intializeMixtureParameters();
 * @endcode
 * in the constructor. They can be accessed from the mixtures using constant
 * accessors.
 *
 * The pure virtual function to implement in derived class are
 * @code
 *   virtual IMixtureComposerBase* create() const = 0;
 *   virtual IMixtureComposerBase* clone() const = 0;
 *   virtual void initializeStep() =0;
 *   virtual bool randomInit() =0;
 *   virtual void mStep() = 0;
 *   virtual Real lnComponentProbability(int i, int k) = 0;
 * @endcode
 *
 * The virtual function that can be re-implemented in derived class for a
 * specific behavior are:
 * @code
 *   virtual void writeParameters(std::ostream& os) const;
 *   virtual void pStep();
 *   virtual void inputationStep();
 *   virtual void samplingStep();
 *   virtual void finalizeStep();
 * @endcode
 *
 * @note the virtual method @c IMixtureComposerBase::initializeStep have to be
 * called before any use of the class as it will create/resize the arrays
 * and initialize the constants of the model.
 */
class IMixtureComposerBase
{
  protected:
    /** Constructor.
     * @param nbCluster,nbSample,nbVariable number of clusters, samples and Variables
     **/
    IMixtureComposerBase(int nbSample,
                         int nbCluster);
    /** copy constructor. If the pointer on the mixture parameters are not zero
     *  then they are cloned.
     *  @note if the model have not created the parameters, then the pointer are
     *  initialized to 0.
     *  @param model the model to clone
     **/
    IMixtureComposerBase( IMixtureComposerBase const& model);

  public:
    /** destructor */
    virtual ~IMixtureComposerBase();

    /** @return the number of cluster */
    inline int nbCluster() const { return nbCluster_;}
    /** @return the state of the model*/
    inline modelState state() const { return state_;}
    /** @return the proportions of each mixtures */
    inline Vector<Real> const* p_pk() const
    {
      return &prop_;
    };
    /** @return the tik probabilities */
    inline Matrix<Real> const* p_tik() const {return &tik_;};
    /** @return  the zi class label */
    inline Vector<int> const* p_zi() const {return &zi_;};

    /** set the state of the model : should be used by any strategy*/
    inline void setState(modelState state) {state_ = state;}

    /** @return the value of the probability of the i-th sample in the k-th component.
     *  @param i index of the sample
     *  @param k index of the component
     **/
    virtual Real lnComponentProbability(int i, int k) = 0;

    // virtual with default implementation
    /** write the parameters of the model in the stream os. */
    virtual void writeParameters(std::ostream& os) const {};
    /** compute the number of free parameters of the model.
     *  This method is used in IMixtureComposerBase::initializeStep
     *  in order to give a value to IModelBase::nbFreeParameter_.
     *  @return the number of free parameters
     **/
    virtual int nbFreeParameters() const = 0;
    /** @brief Initialize the model before at its first use.
     *  This function can be overloaded in derived class for initialization of
     *  the specific model parameters. It should be called prior to any used of
     *  the class.
     *  @sa IMixture,MixtureBridge
     **/
    virtual void initializeStep() = 0;

    /** @brief Simulation of all the latent variables and/or missing data
     *  excluding class labels. Default behavior is "do nothing".
     */
    virtual void samplingStep() {};
    /** @brief Finalize the estimation of the model.
     * The default behavior is "do nothing".
     **/
    inline virtual void finalizeStep() {}

    // not virtual
    /** Initialize randomly the labels zi of the model.
     *  Initialize the model parameters using initializeStep()
     *  and compute the tik.
     **/
    void randomClassInit();

    /** Simulate zi accordingly to tik and replace tik by zik by calling cStep().
     *  @return the minimal value of individuals in a class
     **/
    int sStep();
    void sStep(int i);

    /** compute Tik */
    void eStep();
    void eStep(int i);

    /** Compute zi using the Map estimator. */
    void mapStep();
    void mapStep(int i);

    void setProportions(Vector<Real> prop)
    {
      prop_ = prop;
#ifdef MC_DEBUG
      std::cout << "IMixtureComposerBase::setProportions" << std::endl;
      std::cout << "prop_: " << prop_ << std::endl;
#endif
    };

    void setPartition(Vector<int>& zi)
    {
      zi_ = zi;
    };

    int nbSample() const
    {
      return nbSample_;
    };

  protected:
    /** number of cluster. */
    int nbCluster_;
    /** Number of samples */
    int nbSample_;
    /** The proportions of each mixtures */
    Vector<Real> prop_;
    /** The tik probabilities */
    Matrix<Real> tik_;
    /** The zik class label */
    Vector<int> zi_;

    /** Compute proportions using the ML estimator, default implementation. Set
     *  as virtual in case we impose fixed proportions in derived model.
     **/
    virtual void pStep();

    /** Create the mixture model parameters. */
    void intializeMixtureParameters();

    /** returns the range of values over which to loop */
    std::pair<int, int> forRange(int ind) const;
  private:
    /** state of the model*/
    modelState state_;

    /** multinomial law */
    MultinomialStatistic multi_;
};

} // namespace mixt

#endif

