#ifndef MIXTCOMPFACADE_H_
#define MIXTCOMPFACADE_H_
/**@file MixtCompFacade.h
 * @brief 
 */
#include <initializer_list>
#include <iostream>
#include "framework/MixtureInterface/IMixture.h"
#include "framework/MixtureInterface/CompositeMixture.h"
#include "framework/model/Model.h"
#include "framework/algorithms/IAlgo.h"
#include "framework/algorithms/SEMGibbs.h"
#include "framework/strategy/IStrategy.h"
#include "framework/strategy/IterationsStrategy.h"

struct FrameworkInfo{
    std::string datafilename_;
    std::string modalitiesfilename_;
    char filesep_;
    int nbIterations_;
    int burnin_;
    int nbtry_;
    int nbSample_;
    int nbCluster_;
};

class MixtCompFacade
{
  public:
    MixtCompFacade(FrameworkInfo& info);
    bool registerMixture(IMixture& mixture);
    bool instantiateFramework();
    void run();
    ~MixtCompFacade();
  protected:
    IMixture *p_mixture_;
    std::vector<IMixture*> v_mixture_;
    IAlgo *p_algo_;
    Model *p_model_;
    IStrategy *p_strategy_;
    FrameworkInfo info_;
};

#endif /* MIXTCOMPFACADE_H_ */