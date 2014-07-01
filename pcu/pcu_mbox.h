/****************************************************************************** 

  Copyright 2011 Scientific Computation Research Center, 
      Rensselaer Polytechnic Institute. All rights reserved.
  
  This work is open source software, licensed under the terms of the
  BSD license as described in the LICENSE file in the top-level directory.

*******************************************************************************/
#ifndef PCU_MBOX_H
#define PCU_MBOX_H

#include "pcu_mpi.h"

struct pcu_mbox;

struct pcu_mboxes
{
  int n;
  struct pcu_mbox** boxes;
};

void pcu_make_mboxes(struct pcu_mboxes* b, int n);
void pcu_free_mboxes(struct pcu_mboxes* b);

bool pcu_is_near(pcu_message* m);

void pcu_mbox_send(struct pcu_mboxes* b, pcu_message* m);
bool pcu_mbox_done(pcu_message* m);
bool pcu_mbox_receive(struct pcu_mboxes* b, pcu_message* m);

#endif
