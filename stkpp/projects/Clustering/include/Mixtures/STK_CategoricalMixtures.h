/*--------------------------------------------------------------------*/
/*     Copyright (C) 2004-2013 Serge Iovleff

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

    Contact : S..._DOT_I..._AT_stkpp.org (see copyright for ...)
*/

/*
 * Project:  stkpp::Clustering
 * created on: 15 nov. 2013
 * Author:   iovleff, serge.iovleff@stkpp.org
 **/

/** @file STK_GaussianMixtures.h
 *  @brief In this file we define the Gaussian mixtures of the Clustering project.
 **/


#ifndef STK_CATEGORICALMIXTURES_H
#define STK_CATEGORICALMIXTURES_H


#include "../CategoricalMixtureModels/STK_Categorical_pjk.h"
#include "../CategoricalMixtureModels/STK_Categorical_pk.h"

namespace STK
{

namespace Clust
{
/** @ingroup Clustering
 *  specialization of the MixtureTraits for the Categorical_pjk model
 **/
template<>
struct MixtureTraits<Clust::Categorical_pjk_>
{
   /** Type of the data set to used as input */
   typedef Array2D<int> Data;
   /** Type of the parameter set to used as output */
   typedef Array2D<int> Param;
   /** Type of the Data */
   typedef typename Data::Type Type;
   /** Type of the mixture model */
   typedef Categorical_pjk<Data> Mixture;
};
/** @ingroup Clustering
 *  specialization of the MixtureTraits for the Categorical_pk model
 **/
template<>
struct MixtureTraits<Clust::Categorical_pk_>
{
   /** Type of the data set to used as input */
   typedef Array2D<int> Data;
   /** Type of the parameter set to used as output */
   typedef Array2D<int> Param;
   /** Type of the Data */
   typedef typename Data::Type Type;
   /** Type of the mixture model */
   typedef Categorical_pk<Data> Mixture;
};

} // namespace Clust

typedef MixtureBridge<Clust::Categorical_pjk_> MixtureCategorical_pjk;
typedef MixtureBridge<Clust::Categorical_pk_> MixtureCategorical_pk;

} /* namespace STK */

#endif /* STK_CATEGORICALMIXTURES_H */