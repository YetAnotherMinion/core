/****************************************************************************** 

  Copyright 2011 Scientific Computation Research Center, 
      Rensselaer Polytechnic Institute. All rights reserved.
  
  This work is open source software, licensed under the terms of the
  BSD license as described in the LICENSE file in the top-level directory.

*******************************************************************************/
#ifndef PCU_TMPI_H
#define PCU_TMPI_H

#include "pcu_mpi.h"

void pcu_tmpi_init(int n);
void pcu_tmpi_free(void);

int pcu_tmpi_size(void);
int pcu_tmpi_rank(void);
void pcu_tmpi_send(pcu_message* m, int type);
bool pcu_tmpi_done(pcu_message* m);
bool pcu_tmpi_receive(pcu_message* m, int type);

void pcu_tmpi_check_support(void);

extern pcu_mpi pcu_tmpi;

#endif
