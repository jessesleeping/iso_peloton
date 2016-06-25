//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// pqmq.h
//
// Identification: src/include/parser/libpq/pqmq.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//


/*-------------------------------------------------------------------------
 *
 * pqmq.h
 *	  Use the frontend/backend protocol for communication over a shm_mq
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/pqmq.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQMQ_H
#define PQMQ_H

#include "parser/lib/stringinfo.h"
#include "parser/storage/shm_mq.h"

extern void	pq_redirect_to_shm_mq(shm_mq *, shm_mq_handle *);
extern void pq_set_parallel_master(pid_t pid, BackendId backend_id);

extern void pq_parse_errornotice(StringInfo str, ErrorData *edata);

#endif   /* PQMQ_H */