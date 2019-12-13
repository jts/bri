//---------------------------------------------------------
// Copyright 2019 Ontario Institute for Cancer Research
// Written by Jared Simpson (jared.simpson@oicr.on.ca)
//---------------------------------------------------------
//
// bri - simple utility to provide random access to
//       bam records by read name
//
#ifndef BAM_READ_IDX_BENCHMARK
#define BAM_READ_IDX_BENCHMARK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <htslib/sam.h>
#include <htslib/hts.h>
#include <htslib/bgzf.h>

// main of the "benchmark" subprogram
int bam_read_idx_benchmark_main(int argc, char** argv);

#endif
