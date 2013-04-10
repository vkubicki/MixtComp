#ifndef IDeveloper_H_
#define IDeveloper_H_
/**@file IDeveloper.h
 * @brief
 */
#include <fstream>
#include <vector>
#include "framework/DataHandling/DataHandler.h"
class Model;
class IDeveloper
{
  public:
    IDeveloper();
    virtual void initializeStep() = 0;
    virtual void imputationStep() {/**Do nothing by default*/}
    virtual void samplingStep() = 0;
    virtual void paramUpdateStep() = 0;
    virtual void finalizeStep() {/**Do nothing by default*/}
    virtual double posteriorProbability(int sample_num,int Cluster_num) = 0;
    virtual double** allPosteriorProbabilties();
    virtual double logLikelihood() const = 0;
    virtual int freeParameters() const = 0;
    virtual void setData() = 0;
    virtual void writeParameters(std::ostream&) const = 0;

    void setID(char id);
    virtual ~IDeveloper();
  protected:
    int nbSample_,nbVariable_;
    char id_;

    //protected functions
    int nbCluster() const;
    double** conditionalProbabilities() const;
    int* classLabels() const;
    double* proportions() const;
  private:
    Model * p_model_;
};

inline void IDeveloper::setID(char id){
  id_ = id;
}
#endif /* IDeveloper_H_ */
