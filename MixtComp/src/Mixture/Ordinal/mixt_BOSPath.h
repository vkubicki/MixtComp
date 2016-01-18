/*--------------------------------------------------------------------*/
/*  Copyright (C) Inria 2015

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
 *  Created on: March 23, 2015
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#ifndef MIXT_BOSPATH
#define MIXT_BOSPATH

#include <list>
#include <utility>
#include "../LinAlg/mixt_LinAlg.h"
#include "mixt_BOSNode.h"
#include "../Statistic/mixt_MultinomialStatistic.h"

namespace mixt
{

enum zCondition {
  allZ0Forbidden_,
  allZ1Forbidden_,
  allZAuthorized_
};

enum allZ {
  allZ0_,
  allZ1_,
  mixZ0Z1_
};

class BOSPath
{
  public:
    void setInit(int a, int b);
    void setEnd(int a, int b);

    /** Used for debug / test, because in a standard run, should be initialized using removeMissing */
    void setC(const Vector<BOSNode>& c);

    /**
     * Joint probability on the whole BOSPath (path + end condition)
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @return joint probability
     */
    Real computeLogProba(int mu,
                         Real pi) const;

    /**
     * Multinomial conditional probability distribution for tuples inside the path
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @param startIndex index of the first node of the tuple inside the complete path
     * @param sizeTuple size of the tuple
     * @param[out] pathList list of the various path
     * @param[out] probaVec vector of the conditional probabilities of the paths in pathList
     */
    void tupleMultinomial(int mu,
                          Real pi,
                          int startIndex,
                          int sizeTuple,
                          std::list<Vector<BOSNode> >& pathList,
                          Vector<Real>& probaVec,
                          zCondition zCond) const;

    /**
     * Multinomial conditional probability distribution for the elements of the partition at a specific index.
     * This is a recursive function that computes the conditional probabilities for each element in a node,
     * and then call itself for the following node inside a tuple. At the end of the tuple, endMultinomial
     * is called to push back tuples and proba at the end of pathList and probaList.
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @param startIndex the start index of the tuple, inside the complete path
     * @param currIndex the current index of the tuple, inside the complete path
     * @param logProba the conditional log probability of the previous nodes inside the path
     * @param[out] pathList list of the various path
     * @param[out] probaList list of the conditional probabilities of the paths in pathList
     * @param[out] tuple
     */
    void nodeMultinomial(int mu,
                         Real pi,
                         int startIndex,
                         int currIndex,
                         Real logProba,
                         std::list<Vector<BOSNode> >& pathList,
                         std::list<Real>& probaList,
                         Vector<BOSNode>& tuple,
                         zCondition zCond) const;

    /**
     * Compute the conditional probability of the end condition given the last node, and fill pathList and probaList.
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @param logProba the conditional log probability of the previous nodes inside the path
     * @param tuple
     * @param[out] pathList list of the various path
     * @param[out] probaList list of the conditional probabilities of the paths in pathList
     */
    void endMultinomial(int mu,
                        Real pi,
                        int startIndex,
                        int currIndex,
                        Real logProba,
                        const Vector<BOSNode>& tuple,
                        std::list<Vector<BOSNode> >& pathList,
                        std::list<Real>& probaList,
                        zCondition zCond) const;

    /**
     * Randomly initialize the path while being compatible with eInit and endCond constraints
     */
    void initPath();

    /**
     * Performs a single step of Gibbs sampling across all elements in the search path. sizeTupleMax provides
     * the number of tuples to be sampled simultaneously. A minimum of two seems to be necessary to have a mixing
     * Markov chain.
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @param sizeTupleMax maximum size of the tuple (number of contiguous nodes allowed to vary simultaneously)
     * @param allOneAuthorized is it possible to sample all z in c_ at value 1 ?
     */
    void samplePath(int mu,
                    Real pi,
                    int sizeTupleMax,
                    zCondition zCond);

    /**
     * Overwrite the complete path by sampling everything. Usually used when data is completely missing, or
     * when the observed probability distribution must be estimated by numerous samplings.
     *
     * @param mu localization parameter (mode) of the distribution
     * @param pi precision parameter of the distribution
     * @param allOneAuthorized is it possible to sample all z in c_ at value 1 ?
     */
    void forwardSamplePath(int mu,
                           Real pi,
                           zCondition zCond);

    /**
     * Check if all the z in the path are at 1 or not */
    allZ allZ() const {return allZ_;}

    /** Compute the number of z at 1 in path_ */
    int nbZ() const {return nbZ_;}

    const Vector<int, 2>& eInit() const {return eInit_;}
    const Vector<BOSNode>& c() const {return c_;}
    const Vector<int, 2>& endCond() const {return endCond_;}

    int nbNode() const {return nbNode_;}

  private:
    void computeNbZ();

    MultinomialStatistic multi_;

    Vector<int, 2> eInit_;
    Vector<BOSNode> c_;
    Vector<int, 2> endCond_;

    int nbNode_;
    int nbZ_;

    enum allZ allZ_;
};

/**
 * Output a representation of the single node to std::cout
 *
 * @param[in] node
 */
void BOSDisplaySegNode(const BOSNode& node);

/**
 * Output a representation of the path to std::cout
 *
 * @param[in] eInit initial segment of the path
 * @param[in] c path
 */
void BOSDisplayPath(const BOSPath& path);

} // namespace mixt

#endif // MIXT_BOSPATH
