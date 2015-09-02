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
 *  Created on: August 20, 2015
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#include "mixt_RankIndividual.h"

#include "../LinAlg/mixt_Math.h"

namespace mixt
{

RankIndividual::RankIndividual() :
    nbPos_(0),
    lnFacNbPos_(0)
{}

RankIndividual::RankIndividual(int nbPos) :
    nbPos_(nbPos),
    lnFacNbPos_(- std::log(fac(nbPos))),
    x_(nbPos)
{
  obsData_.resize(nbPos);
  y_.resize(nbPos);
  for (int p = 0; p < nbPos_; ++p) // presentation order initialized, ready to be shuffled by removeMissing
  {
    y_(p) = p;
  }
}

void RankIndividual::setNbPos(int nbPos)
{
  nbPos_ = nbPos;
  obsData_.resize(nbPos);
  x_ .setNbPos(nbPos);

  y_.resize(nbPos);
  for (int p = 0; p < nbPos_; ++p) // presentation order initialized, ready to be shuffled by removeMissing
  {
    y_(p) = p;
  }

  lnFacNbPos_ = - std::log(fac(nbPos_));
}

void RankIndividual::removeMissing()
{
  multi_.shuffle(y_);

#ifdef MC_DEBUG
  std::cout << "y_: " << y_.transpose() << std::endl;
#endif
}

Real RankIndividual::xGen(const RankVal& mu,
                          Real pi)
{
#ifdef MC_DEBUG
  int a = 0;
  int g = 0;
#endif

  Real logProba = 0.;

  Real goodlp = std::log(     pi);
  Real badlp  = std::log(1. - pi);

  std::vector<int> x(1); // vector is suboptimal for insertion, but provides contiguous memory storage which will fit in CPU cache. std::list on the contrary does not guarantee contiguity for example.
  x.reserve(nbPos_);

  x[0] = y_(0);

  for (int j = 1; j < nbPos_; ++j) // current element in the presentation order, or current size of the x vector
  {
    int currY = y_(j);
    bool yPlaced = false; // has currY been placed correctly ?
    for (int i = 0; i < j; ++i)
    {
      bool comparison = mu.r()(currY) < mu.r()(x[i]); // true if curr elem is correctly ordered

      if (multi_.sampleBinomial(pi) == 1) // is the comparison correct ?
      {
        logProba += goodlp;

#ifdef MC_DEBUG
        ++a;
        ++g;
#endif
      }
      else
      {
        comparison = !comparison;
        logProba += badlp;

#ifdef MC_DEBUG
        ++a;
#endif
      }

      if (comparison) // element j must be placed here
      {
        x.insert(x.begin() + i, currY);
        yPlaced = true;
        break; // no need to test further position for j element
      }
    }
    if (!yPlaced)
    {
      x.push_back(currY); // if element j has not been placed yet, it goes at the end of x
    }
  }

  x_.setO(x);

#ifdef MC_DEBUG
  std::cout << "Rank::xGen, a: " << a << ", g:" << g << std::endl;
#endif

  return lnFacNbPos_ + logProba;
}

Real RankIndividual::lnCompletedProbability(const RankVal& mu,
                                            Real pi,
                                            int& a,
                                            int& g) const
{
  AG(mu, a, g);

#ifdef MC_DEBUG
  std::cout << "Rank::lnCompletedProbability, a: " << a << ", g:" << g << std::endl;
#endif

  return lnFacNbPos_ + g * std::log(pi) + (a - g) * std::log(1. - pi);
}

void RankIndividual::AG(const RankVal& mu,
                        int& a,
                        int& g) const
{
  a = 0;
  g = 0;

  std::vector<int> x(1); // vector is suboptimal for insertion, but provides contiguous memory storage which will fit in CPU cache
  x.reserve(nbPos_);

  x[0] = y_(0);

  for (int j = 1; j < nbPos_; ++j) // current element in the presentation order, or current size of the x vector
  {
    int currY = y_(j);
    bool yPlaced = false;
    for (int i = 0; i < j; ++i)
    {
      yPlaced = (x_.r()(currY) < x_.r()(x[i]));

      if (yPlaced == (mu.r()(currY) < mu.r()(x[i]))) // is the comparison correct, according to the order provided in mu ?
      {
        ++a;
        ++g;
      }
      else
      {
        ++a;
      }

      if (yPlaced)
      {
        x.insert(x.begin() + i, currY);
        break;
      }
    }
    if (!yPlaced)
    {
      x.push_back(currY); // if element j has not been placed yet, it goes at the end of x
    }
  }
}

/**
 * Perform one round of Gibbs sampling for the presentation order
 * @param mu central rank
 * @param pi precision */
void RankIndividual::sampleY(const RankVal& mu,
                             Real pi)
{
  int a, g; // dummy variables
  Vector<Real, 2> logProba; // first element: current log proba, second element: logProba of permuted state
  Vector<Real, 2> proba   ; // multinomial distribution obtained from the logProba

  logProba(0) = lnCompletedProbability(mu, pi, a, g); // proba of current y

  for (int p = 0; p < nbPos_ - 1; ++p)
  {
    permutationY(p);
    logProba(1) = lnCompletedProbability(mu, pi, a, g);
    proba.logToMulti(logProba);

#ifdef MC_DEBUG
    std::cout << "p: " << p << ", logProba: " << logProba.transpose() << ", proba: " << proba.transpose() << std::endl;
#endif

    if (multi_.sample(proba) == 1) // switch to permuted state ?
    {
      logProba(0) = logProba(1); // accept permutation
    }
    else
    {
      permutationY(p); // revert to previous state
    }
  }
}

void RankIndividual::permutationY(int firstElem)
{
  int dummy = y_(firstElem);
  y_(firstElem    ) = y_(firstElem + 1);
  y_(firstElem + 1) = dummy;
}

void RankIndividual::probaYgX(const RankVal& mu,
                              Real pi,
                              Vector<Vector<int> >& resVec,
                              Vector<Real>& resProba)
{
  int nbInd = fac(nbPos_);

  std::set<int> remainingMod;
  for (int m = 0; m < nbPos_; ++m)
  {
    remainingMod.insert(m);
  }

  Vector<int> vec(nbPos_);
  Vector<Real> logProba(nbInd);

  recYgX(mu,
         pi,
         resVec,
         logProba,
         vec,
         remainingMod,
         0,
         nbInd,
         0,
         nbPos_);

  resProba.logToMulti(logProba); // from log of joint distribution to conditional distribution
}

void RankIndividual::recYgX(const RankVal& mu,
                            Real pi,
                            Vector<Vector<int> >& resVec,
                            Vector<Real>& resProba,
                            Vector<int>& vec,
                            const std::set<int>& remainingMod,
                            int firstElem,
                            int nbElem,
                            int currPos,
                            int nbPos)
{
  int a, g; // dummy variables
  if (currPos == nbPos) // no more modalities to add in the vector
  {
    y_ = vec; // assignment to compute
    resVec(firstElem) = vec;
    resProba(firstElem) = lnCompletedProbability(mu, pi, a, g); // register current vector and its value
  }
  else
  {
    int nextNbElem = nbElem / (nbPos - currPos);
    int indexMod = 0;
    for (std::set<int>::const_iterator it = remainingMod.begin();
         it != remainingMod.end();
         ++it, ++indexMod) // iteration over the modality that have not yet been included in the vector
    {
      std::set<int> remainingModNew = remainingMod; // computation of the modalities available for the remaining of vec
      for (std::set<int>::iterator itNew = remainingModNew.begin();
           itNew != remainingModNew.end();
           ++itNew)
      {
        if (*itNew == *it) // the current modality will not be available further down the vector
        {
          remainingModNew.erase(itNew);
          break;
        }
      }

      vec(currPos) = *it;
      recYgX(mu,
             pi,
             resVec,
             resProba,
             vec,
             remainingModNew,
             firstElem + indexMod * nextNbElem,
             nextNbElem,
             currPos + 1,
             nbPos);
    }
  }
}

} // namespace mixt