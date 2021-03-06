/*
 * Copyright (C) 2011 Scientific Computation Research Center
 *
 * This work is open source software, licensed under the terms of the
 * BSD license as described in the LICENSE file in the top-level directory.
 */

#ifndef SPR_H
#define SPR_H

/** \file spr.h
 *  \brief The SPR error estimator interface
 */

#include "apf.h"
#include "apfNew.h"
#include "apfDynamicVector.h"
#include "apfDynamicMatrix.h"

/** \namespace spr
  * \brief All SPR error estimator functions
  */
namespace spr {

/** @brief compute the gradient of a vector or scalar
  *        field at integration points
  * @param f (In) scalar or vector nodal field
  * @param name (In) name of integration point field
  * @param order (In) integration order of accuracy
  */
apf::Field* getGradIPField(apf::Field* f,
                           const char* name, 
                           int order);

/** @brief recover a nodal field using patch recovery
  * @param ip_field (In) integration point field
  */
apf::Field* recoverField(apf::Field* ip_field);

/** @brief run the SPR ZZ error estimator
  * @param f the integration-point input field
  * @param adapt_ratio the fraction of allowable error,
  *                    scales the output size field.
  * @returns a nodal scalar size field with the same distribution
  *          as the mesh coordinate field
  */
apf::Field* getSPRSizeField(apf::Field* f, double adapt_ratio);

}

#endif
