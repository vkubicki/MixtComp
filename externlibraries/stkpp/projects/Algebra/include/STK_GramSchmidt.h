/*--------------------------------------------------------------------*/
/*     Copyright (C) 2004-2007  Serge Iovleff

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this program; if not, write to the
    Free Software Foundation, Inc.,
    59 Temple Place,
    Suite 330,
    Boston, MA 02111-1307
    USA

    Contact : Serge.Iovleff@stkpp.org
*/

/*
 * Project:  Algebra
 * Purpose:  Define the GramScmidt Class.
 * Author:   Serge Iovleff, serge.iovleff@stkpp.org
 *
 **/

/** @file STK_GramSchmidt.h
 *  @brief In this file we define the GramSchidt method.
 *
 **/
 
#ifndef STK_GRAMSCHMIDT_H
#define STK_GRAMSCHMIDT_H

// Container classes
#include "../../Arrays/include/STK_ArrayBase.h"
#include "STK_LinAlgebra1D.h"

namespace STK
{

/** @ingroup Algebra
 *  @brief The gramSchmidt method perform the Gram Schmidt orthonormalization
 *  of an Array of Real.
 *  @param A the container to orthnormalize
 **/
template < class TContainer2D>
void gramSchmidt( ArrayBase<TContainer2D>& A)
{
  // orthonormalize
  for (int j= A.firstIdxCols(); j<= A.lastIdxCols(); j++)
  {
    for( int i= A.firstIdxCols(); i < j; i++)
    {
      const Real dotij = dot(A.asDerived().col(i), A.asDerived().col(j));
      A.asDerived().col(j) -= A.asDerived().col(i) * dotij;
    }
    // normalize
    const Real norm = normTwo(A.asDerived().col(j));
    if (norm)
    {
      A.asDerived().col(j)/=norm;
    }
  }
}

} // Namespace STK

#endif // STK_GRAMSCHMIDT_H

