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
 *  Created on: January 27, 2015
 *  Authors:    Vincent KUBICKI <vincent.kubicki@inria.fr>
 **/

#ifndef MIXT_EIGENMATRIXBASEADDONS_H
#define MIXT_EIGENMATRIXBASEADDONS_H

//#include <numeric>

#include "mixt_Iterator.h"
#include "mixt_ConstIterator.h"

typedef Iterator iterator;
typedef ConstIterator const_iterator;

/** Element-wise comparison to a scalar */
inline bool operator==(const Scalar& scalar) const
{
  for(int j = 0; j < cols(); ++j)
    for(int i = 0; i < rows(); ++i)
      if(derived().coeff(i, j) != scalar)
        return false;
  return true;
}

/** Element-wise comparison to a scalar */
inline bool operator!=(const Scalar& scalar) const
{
  for(int j = 0; j < cols(); ++j)
    for(int i = 0; i < rows(); ++i)
      if(derived().coeff(i, j) != scalar)
        return true;
  return false;
}

/** Comparison between vectors / matrices. Note that this is not a component-wise comparison,
 * but rather a form of order, like the alphabetical order between words. Comparaison stops at the first
 * different coefficient. */
inline bool operator<(const Derived& rhs) const
{
  typename Derived::const_iterator lhsIt = derived().begin();
  typename Derived::const_iterator rhsIt = rhs      .begin();
  for (;
       lhsIt != derived().end();
       ++lhsIt, ++rhsIt)
  {
    if (*lhsIt < *rhsIt)
    {
      return true;
    }
    else if (*lhsIt > *rhsIt)
    {
      return false;
    }
  }
  return false; // equality of all terms
}

inline bool operator>(const Derived& rhs) const
{
  typename Derived::const_iterator lhsIt = derived().begin();
  typename Derived::const_iterator rhsIt = rhs      .begin();
  for (;
       lhsIt != derived().end();
       ++lhsIt, ++rhsIt)
  {
    if (*lhsIt > *rhsIt)
    {
      return true;
    }
    else if (*lhsIt < *rhsIt)
    {
      return false;
    }
  }
  return false;
}

/** Component-wise comparison with a scalar. */
inline bool operator>(Scalar rhs) const {
  typename Derived::const_iterator lhsIt = derived().begin();
  for (;
       lhsIt != derived().end();
       ++lhsIt) {
    if (*lhsIt <= rhs) {
      return false;
    }
  }
  return true;
}

/** Component-wise comparison with a scalar. */
inline bool operator<(Scalar rhs) const {
  typename Derived::const_iterator lhsIt = derived().begin();
  for (;
       lhsIt != derived().end();
       ++lhsIt) {
    if (*lhsIt >= rhs) {
      return false;
    }
  }
  return true;
}

/** Element-wise + between matrix and scalar */
inline const CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                          Derived>
operator+(const Scalar& scalar) const
{
  return CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_add_op<Scalar>(scalar));
}

/** Element-wise + between a scalar and a matrix */
friend inline const CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                                 Derived>
operator+(const Scalar& scalar,
          const MatrixBase<Derived>& mat)
{
  return CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                      Derived>(mat.derived(),
                               internal::scalar_add_op<Scalar>(scalar));
}

/** Element-wise - between matrix and scalar */
inline const CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                          Derived>
operator-(const Scalar& scalar) const
{
  return CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_add_op<Scalar>(-scalar));
}

/** Element-wise - between a scalar and a matrix */
friend inline const CwiseUnaryOp<internal::scalar_add_op<Scalar>,
                                 CwiseUnaryOp<internal::scalar_opposite_op<Scalar>,
                                              const Derived> >
operator-(const Scalar& scalar,
          const MatrixBase<Derived>& mat)
{
  return (-mat) + scalar;
}

/** Element-wise assignment of a scalar */
inline MatrixBase<Derived>&
operator=(const Scalar& scalar)
{
  (*this) = CwiseNullaryOp<Eigen::internal::scalar_constant_op<Scalar>,
                           Derived >(derived().rows(),
                                     derived().cols(),
                                     internal::scalar_constant_op<Scalar>(scalar));
  return *this;
}

/** Element-wise += between matrix and scalar */
inline MatrixBase<Derived>&
operator+=(const Scalar& scalar)
{
  (*this) = derived() + scalar;
  return *this;
}

/** Element-wise -= between matrix and scalar */
inline MatrixBase<Derived>&
operator-=(const Scalar& scalar)
{
  (*this) = derived() - scalar;
  return *this;
}

/** Component-wise product */
template<typename OtherDerived>
inline const CwiseBinaryOp<Eigen::internal::scalar_product_op<Scalar, Scalar>,
                           Derived,
                           Derived>
operator%(const MatrixBase<OtherDerived>& other) const
{
  return CwiseBinaryOp<internal::scalar_product_op<Scalar, Scalar>,
                       Derived,
                       OtherDerived>(derived(),
                                     other.derived(),
                                     internal::scalar_product_op<Scalar, Scalar>());
}

/** Component-wise quotient */
template<typename OtherDerived>
inline const CwiseBinaryOp<internal::scalar_quotient_op<Scalar, Scalar>,
                           Derived,
                           Derived>
operator/(const MatrixBase<OtherDerived>& other) const
{
  return CwiseBinaryOp<internal::scalar_quotient_op<Scalar, Scalar>,
                       Derived,
                       OtherDerived>(derived(),
                                     other.derived(),
                                     internal::scalar_quotient_op<Scalar, Scalar>());
}

/** Element-wise %= between matrices */
template<typename OtherDerived>
inline MatrixBase<Derived>&
operator%=(const MatrixBase<OtherDerived>& other)
{
  (*this) = derived() % other;
  return *this;
}

/** Element-wise /= between matrices */
template<typename OtherDerived>
inline MatrixBase<Derived>&
operator/=(const MatrixBase<OtherDerived>& other)
{
  (*this) = derived() / other;
  return *this;
}

/** Element-wise log computation */
inline const CwiseUnaryOp<internal::scalar_log_op<Scalar>,
                          Derived>
log() const
{
  return CwiseUnaryOp<internal::scalar_log_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_log_op<Scalar>());
}

/** Element-wise exp computation */
inline const CwiseUnaryOp<internal::scalar_exp_op<Scalar>,
                          Derived>
exp() const
{
  return CwiseUnaryOp<internal::scalar_exp_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_exp_op<Scalar>());
}

/** Element-wise abs computation */
inline const CwiseUnaryOp<internal::scalar_abs_op<Scalar>,
                          Derived>
abs() const
{
  return CwiseUnaryOp<internal::scalar_abs_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_abs_op<Scalar>());
}

/** Element-wise inverse computation */
inline const CwiseUnaryOp<internal::scalar_inverse_op<Scalar>,
                          Derived>
cInv() const
{
  return CwiseUnaryOp<internal::scalar_inverse_op<Scalar>,
                      Derived>(derived(),
                               internal::scalar_inverse_op<Scalar>());
}

/** Element-wise + between a scalar and a matrix */
friend inline const CwiseUnaryOp<internal::scalar_multiple_op<Scalar>,
                                 const CwiseUnaryOp<internal::scalar_inverse_op<Scalar>,
                                                    Derived> >
operator/(const Scalar& scalar,
          const MatrixBase<Derived>& mat)
{
  return scalar * mat.cInv();
}

iterator begin()
{
  return Iterator(0,
                  0,
                  derived());
}

const_iterator begin() const
{
  return ConstIterator(0,
                       0,
                       derived());
}

iterator end()
{
  return Iterator(0,
                  derived().cols(),
                  derived());
}

const_iterator end() const
{
  return ConstIterator(0,
                       derived().cols(),
                       derived());
}

/** Sort function for non contiguous data, for example block. Slower than sortContiguous */
void sort()
{
  std::sort(derived().begin(),
            derived().end());
}

/** Sorted indices of the container */
template<typename Container>
void sortIndex(Container& out) const
{
  out.resize(derived().size());
  for (int i = 0, ie = out.size();
	   i < ie;
	   ++i)
  {
	  out(i) = i;
  }
  std::sort(out.begin(),
            out.end(),
            [this](int left, int right) {return (*this)(left) < (*this)(right);});
}

/**
 * Computation of a multinomial distribution from log values of weights.
 * A common example of usage is the computations of the proportions t_ik
 * from the logProbabilities.
 *
 * @param log vector of log values
 * @param[out] multi multinomial distribution
 * @value log of the marginal probability
 */
template<typename OtherDerived>
Scalar logToMulti(const MatrixBase<OtherDerived>& multi) {
  derived() = multi;
  Scalar max = derived().maxCoeff();
  derived() -= max;
  derived() = derived().exp();
  Scalar sum = derived().sum();
  derived() = derived() / sum;

  return max + std::log(sum);
}

template<typename Type>
void copyIterable(const Type& other) {
  derived().resize(other.size());

  int currCoeff = 0;
  for (typename Type::const_iterator it = other.begin(), itE = other.end();
       it != itE;
       ++it, ++currCoeff) {
    derived()(currCoeff) = *it;
  }
}

#endif // MIXT_EIGENMATRIXBASEADDONS_H
