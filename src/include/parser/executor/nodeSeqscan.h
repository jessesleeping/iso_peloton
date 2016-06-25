//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// nodeSeqscan.h
//
// Identification: src/include/parser/executor/nodeSeqscan.h
//
// Copyright (c) 2015-16, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//


/*-------------------------------------------------------------------------
 *
 * nodeSeqscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2015, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeSeqscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODESEQSCAN_H
#define NODESEQSCAN_H

#include "parser/nodes/execnodes.h"

extern SeqScanState *ExecInitSeqScan(SeqScan *node, EState *estate, int eflags);
extern TupleTableSlot *ExecSeqScan(SeqScanState *node);
extern void ExecEndSeqScan(SeqScanState *node);
extern void ExecReScanSeqScan(SeqScanState *node);

#endif   /* NODESEQSCAN_H */