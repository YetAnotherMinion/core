/****************************************************************************** 

  Copyright 2011 Scientific Computation Research Center, 
      Rensselaer Polytechnic Institute. All rights reserved.
  
  This work is open source software, licensed under the terms of the
  BSD license as described in the LICENSE file in the top-level directory.

*******************************************************************************/
#include "pcu_mbox.h"
#include "pcu_thread.h"
#include "pcu_pmpi.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>

struct pcu_mbox
{
  pcu_message* sent;
  pthread_spinlock_t lock;
};

static void init_mbox(struct pcu_mbox* b)
{
  int err;
  b->sent = 0;
  err = pthread_spin_init(&b->lock, PTHREAD_PROCESS_PRIVATE);
  assert(!err);
}

static void free_mbox(struct pcu_mbox* b)
{
  int err = pthread_spin_destroy(&b->lock);
  assert(!err);
}

static void lock_mbox(struct pcu_mbox* b)
{
  int err = pthread_spin_lock(&b->lock);
  assert(!err);
}

static void unlock_mbox(struct pcu_mbox* b)
{
  int err = pthread_spin_unlock(&b->lock);
  assert(!err);
}

static void fill_mbox(struct pcu_mbox* b, pcu_message* m)
{
  lock_mbox(b);
  assert(!b->sent);
  b->sent = m;
  memset(&m->request, 0xFF, sizeof(m->request));
  unlock_mbox(b);
}

static bool check_mbox(struct pcu_mbox* b, pcu_message* m)
{
  lock_mbox(b);
  bool ret = false;
  if (b->sent) {
    *m = *b->sent;
    memset(&b->sent->buffer, 0, sizeof(b->sent->buffer));
    memset(&b->sent->request, 0, sizeof(b->sent->request));
    b->sent = 0;
    ret = true;
  }
  unlock_mbox(b);
  return ret;
}

void pcu_make_mboxes(struct pcu_mboxes* b, int n)
{
  b->n = n;
  PCU_MALLOC(b->boxes, n);
  PCU_MALLOC(b->boxes[0], n * n);
  for (int i = 0; i < n; ++i)
  for (int j = 0; j < n; ++j)
    init_mbox(&b->boxes[i][j]);
}

void pcu_free_mboxes(struct pcu_mboxes* b)
{
  for (int i = 0; i < b->n; ++i)
  for (int j = 0; j < b->n; ++j)
    free_mbox(&b->boxes[i][j]);
  pcu_free(b->boxes[0]);
  pcu_free(b->boxes);
}

static bool is_local(pcu_message* m)
{
  return (m->peer / pcu_thread_size()) == pcu_pmpi_rank();
}

bool pcu_mbox_send(struct pcu_mboxes* b, pcu_message* m)
{
  if (!is_local(m))
    return false;
  int peer = m->peer % pcu_thread_size();
  int self = pcu_thread_rank();
  fill_mbox(&b->boxes[self][peer], m);
  return true;
}

bool pcu_mbox_done(pcu_message* m)
{
  MPI_Request r;
  if (!is_local(m))
    return false;
  memset(&r, 0xFF, sizeof(r));
  return memcmp(&r, &m->request, sizeof(r)) == 0;
}

bool pcu_mbox_receive(struct pcu_mboxes* b, pcu_message* m)
{
  int self = pcu_thread_rank();
  for (int i = 0; i < b->n; ++i)
    if (check_mbox(&b->boxes[i][self], m))
      return true;
  return false;
}

