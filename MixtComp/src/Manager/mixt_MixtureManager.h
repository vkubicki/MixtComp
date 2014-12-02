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
 *  Created on: March 18, 2014
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>,
 *              Serge IOVLEFF <serge.iovleff@inria.fr>
 **/

#ifndef MIXT_MIXTUREMANAGER_H
#define MIXT_MIXTUREMANAGER_H

#include "../Mixture/StkppMixturesBridges/mixt_MixtureBridge.h"
#include "../Composer/mixt_MixtureComposer.h"

namespace mixt
{

template<typename DataHandler,
         typename DataExtractor,
         typename ParamSetter,
         typename ParamExtractor>
class MixtureManager
{
  public:
    typedef typename DataHandler::InfoMap InfoMap;

    MixtureManager(const DataHandler* handler,
                   DataExtractor* p_dataExtractor,
                   const ParamSetter* p_paramSetter,
                   ParamExtractor* p_paramExtractor,
                   STK::Real confidenceLevel) :
      p_handler_(handler),
      p_dataExtractor_(p_dataExtractor),
      p_paramSetter_(p_paramSetter),
      p_paramExtractor_(p_paramExtractor),
      confidenceLevel_(confidenceLevel)
    {}

    void createMixtures(mixt::MixtureComposer* composer,
                        int nbCluster)
    {
      for (typename InfoMap::const_iterator it=p_handler_->info().begin(); it!=p_handler_->info().end(); ++it)
      {
        std::string idName = it->first;
        std::string model = it->second;
        Mixture idModel = stringToMixture(model);
#ifdef MC_DEBUG_NEW
        std::cout << "MixtureManager::createMixtures, "
        		  << "idName: " << idName
				  << ", model: " << model
				  << ", idModel:" << idModel << std::endl;
#endif
        // get a mixture fully
        mixt::IMixture* p_mixture = createMixture(idModel,
                                                  idName,
                                                  nbCluster,
                                                  confidenceLevel_);
        if (p_mixture) composer->registerMixture(p_mixture);
      }
    }

    /** create a mixture and initialize it*
     *  @param idModel id of the model
     *  @param idName name of the model
     *  @param nbCluster number of cluster of the model
     **/
    mixt::IMixture* createMixture(Mixture idModel,
                                  std::string const& idName,
                                  int nbCluster,
                                  STK::Real confidenceLevel)
    {
      switch (idModel)
      {
        case Gaussian_sjk_:
        {
          typename GaussianBridge_sjk_m<DataHandler,
                                        DataExtractor,
                                        ParamSetter,
                                        ParamExtractor>::type* p_bridge = new typename GaussianBridge_sjk_m<DataHandler,
                                                                                                            DataExtractor,
                                                                                                            ParamSetter,
                                                                                                            ParamExtractor>::type(idName,
                                                                                                                                  nbCluster,
                                                                                                                                  p_handler_,
                                                                                                                                  p_dataExtractor_,
                                                                                                                                  p_paramSetter_,
                                                                                                                                  p_paramExtractor_,
                                                                                                                                  confidenceLevel);
          p_bridge->setDataParam();
          return p_bridge;
        }
        break;

        case Categorical_pjk_:
        {
          typename CategoricalBridge_pjk_m<DataHandler,
                                           DataExtractor,
                                           ParamSetter,
                                           ParamExtractor>::type* p_bridge = new typename CategoricalBridge_pjk_m<DataHandler,
                                                                                                                  DataExtractor,
                                                                                                                  ParamSetter,
                                                                                                                  ParamExtractor>::type(idName,
                                                                                                                                        nbCluster,
                                                                                                                                        p_handler_,
                                                                                                                                        p_dataExtractor_,
                                                                                                                                        p_paramSetter_,
                                                                                                                                        p_paramExtractor_,
                                                                                                                                        confidenceLevel);
          p_bridge->setDataParam();
          return p_bridge;
        }
        break;

        default:
          return 0;
        break;
      }
      return 0; // 0 if idModel is not a stk++ model
    }

  private:
    /** pointer to the dataHandler */
    const DataHandler* p_handler_;

    /** pointer to the dataExtractor */
    DataExtractor* p_dataExtractor_;

    /** pointer to the paramSetter */
    const ParamSetter* p_paramSetter_;

    /** pointer to the parameter extractor */
    ParamExtractor* p_paramExtractor_;

    /** confidence interval, to be transmitted to the mixtures at creation */
    STK::Real confidenceLevel_;
};

} // namespace mixt

#endif /* MIXT_MIXTUREMANAGER_H */
