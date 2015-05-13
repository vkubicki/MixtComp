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

#include "mixt_BOSPath.h"

#include "../../Various/mixt_Constants.h"

namespace mixt
{

void BOSPath::setInit(int a, int b)
{
  eInit_(0) = a;
  eInit_(1) = b;
  nbNode_ = eInit_(1) - eInit_(0); // number of segments in the path;
  c_.resize(nbNode_);
}

void BOSPath::setEnd(int a, int b)
{
  endCond_(0) = a;
  endCond_(1) = b;
};

Real BOSPath::computeLogProba(int mu,
                              Real pi) const
{
#ifdef MC_DEBUG
  std::cout << "BOSPath::computeProba" << std::endl;
#endif
  Real logProba = 0.; // The initial probability of being in any of the member of the input interval is 1

  int lastSeg = c_(nbNode_ - 1).e_(0); // last segment only contains one element
  if (endCond_(0) <= lastSeg && lastSeg <= endCond_(1)) // is the path compatible with the provided condition ?
  {
    for (int node = 0; node < nbNode_; ++node) // loop over each BOSNode in c
    {
#ifdef MC_DEBUG
      std::cout << "node: " << node << std::endl;
#endif
      Real yLogProba;
      if (node == 0)
      {
        yLogProba = c_(node).yLogProba(eInit_); // yProba based on initial segment
      }
      else
      {
        yLogProba = c_(node).yLogProba(c_(node - 1).e_); // yProba based on previous iteration segment
      }
      Real zLogProba = c_(node).zLogProba(pi);
      Real eLogProba = c_(node).eLogProba(mu,
                                 pi);
#ifdef MC_DEBUG
      std::cout << "yLogProba: " << yLogProba << ", zLogProba: " << zLogProba << ", eLogProba: " << eLogProba << std::endl;
#endif
      logProba += yLogProba + zLogProba + eLogProba;
    }
  }
  else // conditional probability of verifying condition given the path is null
      logProba = minInf;

  return logProba;
}

void BOSPath::nodeMultinomial(int mu,
                              Real pi,
                              int index,
                              std::list<Vector<BOSNode, 2> >& pathList,
                              Vector<Real>& probaVec) const
{
#ifdef MC_DEBUG
  std::cout << "BOSPath::nodeMultinomial" << std::endl;
#endif
  std::list<Real> probaList;
  Vector<BOSNode, 2> path; // a specific path is used for this computation, instead of c
  Vector<int, 2> firstSeg;

  if (index == 0)
  {
    firstSeg = eInit_;
  }
  else
  {
    firstSeg = c_(index - 1).e_;
  }

  for (path(0).y_ = firstSeg(0)    ;
       path(0).y_ < firstSeg(1) + 1;
       ++path(0).y_) // outer node
  {
    path(0).partition(firstSeg); // computation of path(0).part_
    Real y0LogProba = path(0).yLogProba(firstSeg);
#ifdef MC_DEBUG
    std::cout << "y0: " << y0 << std::endl;
    std::cout << "\ty0LogProba: " << y0LogProba << std::endl;
#endif
    for (path(0).z_ = 0;
         path(0).z_ < 2;
         ++path(0).z_)
    {
      Real z0LogProba = path(0).zLogProba(pi);
#ifdef MC_DEBUG
      std::cout << "y0: " << path(0).y_ << ", z0: " << path(0).z_ << std::endl;
      std::cout << "\tz0LogProba: " << z0LogProba << std::endl;
#endif
      for(int e0 = 0            ;
          e0 < path(0).partSize_;
          ++e0)
      {
        path(0).e_ = path(0).part_(e0);
        Real e0LogProba = path(0).eLogProba(mu, pi);
#ifdef MC_DEBUG
        std::cout << "y0: " << path(0).y_ << ", z0: " << path(0).z_ << ", e0: " << path(0).e_ << ", part0(e0)(0): " << path(0).part_(path(0).e_)(0) << ", part0(e0)(1): " << path(0).part_(path(0).e_)(1) << std::endl;
        std::cout << "\te0LogProba: " << e0LogProba << std::endl;
#endif
        if (e0LogProba > minInf) // null probability segments are not computed further
        {
          for(path(1).y_ = path(0).e_(0)    ;
              path(1).y_ < path(0).e_(1) + 1;
              ++path(1).y_) // inner node
          {
            path(1).partition(path(0).e_); // computation of path(1).part_
            Real y1LogProba = path(1).yLogProba(path(0).e_);
#ifdef MC_DEBUG
            std::cout << "y0: " << path(0).y_ << ", z0: " << path(0).z_ << ", e0: " << path(0).e_ << ", part0(e0)(0): " << path(0).part_(path(0).e_)(0) << ", part0(e0)(1): " << path(0).part_(path(0).e_)(1) << std::endl;
            std::cout << "y1: " << path(1).y_ << std::endl;
            std::cout << "\ty1LogProba: " << y1LogProba << std::endl;
#endif
            for (path(1).z_ = 0;
                 path(1).z_ < 2;
                 ++path(1).z_)
            {
              Real z1LogProba = path(1).zLogProba(pi);
#ifdef MC_DEBUG
              std::cout << "y0: " << path(0).y_ << ", z0: " << path(0).z_ << ", e0: " << path(0).e_ << ", part0(e0)(0): " << path(0).part_(path(0).e_)(0) << ", part0(e0)(1): " << path(0).part_(path(0).e_)(1) << std::endl;
              std::cout << "y1: " << path(1).y_ << ", z0: " << path(1).z_  << std::endl;
              std::cout << "\tz1LogProba: " << z1LogProba << std::endl;
#endif
              for(int e1 = 0;
                  e1 < path(1).partSize_;
                  ++e1)
              {
                path(1).e_ = path(1).part_(e1);
                Real e1LogProba = path(1).eLogProba(mu, pi);
                // Vector<int, 2> endSeg = path(1).part_(path(1).e_);
#ifdef MC_DEBUG
                std::cout << "y0: " << path(0).y_ << ", z0: " << path(0).z_ << ", e0: " << path(0).e_ << ", part0(e0)(0): " << path(0).part_(path(0).e_)(0) << ", part0(e0)(1): " << path(0).part_(path(0).e_)(1) << std::endl;
                std::cout << "y1: " << path(1).y_ << ", z0: " << path(1).z_ << ", e0: " << path(1).e_ << ", part0(e0)(0): " << path(1).part_(path(1).e_)(0) << ", part0(e0)(1): " << path(1).part_(path(1).e_)(1) << std::endl;
                std::cout << "\te1LogProba: " << e1LogProba << std::endl;
#endif
                if (index != nbNode_ - 2) // does the second node in the pair corresponds to the last node in c, if not, the proba of the next node conditionally to the current last segment must be computed
                {
#ifdef MC_DEBUG
                  std::cout << "index != nbNode_ - 2" << std::endl;
#endif
                  BOSNode lastNode = c_(index + 2); // copy of the last node
                  lastNode.partition(path(1).e_); // computation of the partition in the last node

                  if (lastNode.isInPart(c_(index + 2).e_)) // is the next node final segment still compatible with the new partition of the node ?
                  {
                    Real lastNodeLogProba =   lastNode.yLogProba(path(1).e_) // yProba based on second node
                                            + lastNode.zLogProba(pi)
                                            + lastNode.eLogProba(mu, pi); // the proba of the segment with the new partition

                    if (lastNodeLogProba > minInf) // has the next node a non zero probability, given the current value of path(1).e_ ?
                    {
                      Real logProba =   y0LogProba + z0LogProba + e0LogProba
                                      + y1LogProba + z1LogProba + e1LogProba
                                      + lastNodeLogProba;
                      pathList.push_back(path);
                      probaList.push_back(logProba); // proba of current path is saved
                    }
                  }
#ifdef MC_DEBUG
                  std::cout << "lastNodeLogProba: " << lastNodeLogProba << std::endl;
#endif
                }
                else // the second node corresponds to the last node in c, and the compatibility with the data constraint is checked
                {
#ifdef MC_DEBUG
                  std::cout << "index == nbNode_ - 2" << std::endl;
#endif
                  if (e1LogProba > minInf && path(1).e_(0) <= endCond_(1) && endCond_(0) <= path(1).e_(1)) // is the final segment compatible with the data constraint ?
                  {
                    Real logProba =   y0LogProba + z0LogProba + e0LogProba
                                    + y1LogProba + z1LogProba + e1LogProba;
                    pathList.push_back(path);
                    probaList.push_back(logProba); // proba of current path is saved
                  }
                  else
                  {
#ifdef MC_DEBUG
                    std::cout << "e1, null proba case or final condition not verified" << std::endl;
#endif
                  }

                }
              }
            }
          }
        }
        else
        {
#ifdef MC_DEBUG
          std::cout << "e0, null proba case detected" << std::endl;
#endif
        }
      }
    }
  }

  // conversion from list of logs to categorical density distribution, similar to eStep conversion
  int nbPath = probaList.size();
#ifdef MC_DEBUG
  std::cout << "nbPath: " << nbPath << std::endl;
#endif
  probaVec.resize(nbPath);
  std::list<Real>::const_iterator it = probaList.begin();
  for (int i = 0; i < nbPath; ++i, ++it)
  {
#ifdef MC_DEBUG
    std::cout << "i: " << i << " / " << nbPath << std::endl;
#endif
    probaVec(i) = *it;
  }

  Real max = probaVec.maxCoeff();
  probaVec -= max;
  probaVec = probaVec.exp();
  Real sum = probaVec.sum();
  probaVec /= sum;
}

void BOSPath::initPath()
{
#ifdef MC_DEBUG
  std::cout << "initPath" << std::endl;
  std::cout << "endCond_(0): " << endCond_(0) << ", endCond_(1): " << endCond_(1) << std::endl;
#endif
  Vector<int, 2> currSeg = eInit_;

  for (int i = 0; i < nbNode_; ++i) // loop to fill all the elements of the path
  {
#ifdef MC_DEBUG
    std::cout << "i: " << i << std::endl;
#endif
    c_(i).y_ = multi_.sampleInt(currSeg(0), currSeg(1));
    c_(i).partition(currSeg);
    c_(i).z_ = 0; // comparisons are all blind in initialization
    Vector<Real> segProba(c_(i).partSize_); // vector of proba of each segment in the partition
    for (int s = 0; s < c_(i).partSize_; ++s) // computation of the allowed segments
    {
      if (c_(i).part_(s)(0) <= endCond_(1) && // test if the current segment of the partition can reach any allowed point
          endCond_(0)       <= c_(i).part_(s)(1) )
      {
        segProba(s) = c_(i).part_(s)(1) - c_(i).part_(s)(0) + 1; // proba to sample segment is proportional to its size
      }
      else
      {
        segProba(s) = 0.; // segments incompatible with the last segment have a zero probability
      }
    }
    segProba /= segProba.sum();
    int sampleSeg = multi_.sample(segProba);
#ifdef MC_DEBUG
    std::cout << "segProba" << std::endl;
    std::cout << segProba << std::endl;
    std::cout << "sampleSeg: " << sampleSeg << std::endl;
#endif
    c_(i).e_ = c_(i).part_(sampleSeg);
    currSeg = c_(i).e_; // initial segment used for next node
#ifdef MC_DEBUG
    std::cout << "currNode" << std::endl;
    displaySegNode(c_(i));
#endif
  }
}

void BOSPath::samplePath(int mu,
                         Real pi)
{
#ifdef MC_DEBUG
  std::cout << "BOSPath::samplePath" << std::endl;
#endif
  for (int node = 0; node < nbNode_ - 1; ++node)
  {
#ifdef MC_DEBUG
    std::cout << "node: " << node << " / " << nbNode_ - 2 << std::endl;
#endif
    // computation of the possible node values and associated probabilities
    std::list<Vector<BOSNode, 2> > pathList;
    Vector<Real> probaVec;
    nodeMultinomial(mu,
                    pi,
                    node,
                    pathList,
                    probaVec);

    // sampling and replacement in the path
#ifdef MC_DEBUG
    std::cout << "probaVec.size(): " << probaVec.size() << std::endl;
    std::cout << "pathList.size(): " << pathList.size() << std::endl;
#endif
#ifdef MC_DEBUG
    std::cout << "probaVec: " << probaVec << std::endl;
    std::cout << "pathList" << std::endl;
    for(std::list<Vector<BOSNode, 2> >::iterator it = pathList.begin();
        it != pathList.end();
        ++it)
    {
      std::cout << "displaySegNode((*it)(0))" << std::endl;
      displaySegNode((*it)(0));
      std::cout << "displaySegNode((*it)(1))" << std::endl;
      displaySegNode((*it)(1));
    }
#endif
    int pathSampled = multi_.sample(probaVec);
    std::list<Vector<BOSNode, 2> >::iterator it = pathList.begin();
    for(int path = 0; path < pathSampled; ++path)
    {
      ++it; // fast-forward to the sampled sub-path
    }
    c_[node    ] = (*it)(0);
    c_[node + 1] = (*it)(1);
#ifdef MC_DEBUG
    std::cout << "end of node iteration inside samplePath, displayPath:" << std::endl;
    displayPath(*this);
#endif
  }
}

void displaySegNode(const BOSNode& node)
{
  std::cout << "\ty: " << node.y_ << std::endl;
  std::cout << "\tpart: " << std::endl;
  for (int s = 0; s < node.partSize_; ++s)
  {
    std::cout << "\t\tpart(" << s << ")(0): "  << node.part_(s)(0)
              << ", part(" << s << ")(1): "   << node.part_(s)(1) << std::endl;
  }
  std::cout << "\tz: " << node.z_ << std::endl;
  std::cout << "\tnode.e_(0): " << node.e_(0) << ", node.e_(1): " << node.e_(1) << std::endl;
}

void displayPath(const BOSPath& path)
{
  std::cout << "path.eInit_(0): " << path.eInit_(0) << ", path.eInit_(1): " << path.eInit_(1) << std::endl;
  for (int node = 0; node < path.c_.size(); ++node)
  {
    std::cout << "node: " << node << std::endl;
    displaySegNode(path.c_(node));
  }
}

} // namespace mixt
