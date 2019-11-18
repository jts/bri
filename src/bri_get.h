//---------------------------------------------------------
// Copyright 2019 Ontario Institute for Cancer Research
// Written by Jared Simpson (jared.simpson@oicr.on.ca)
//---------------------------------------------------------
//
// bri - simple utility to provide random access to
//       bam records by read name
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sam.h>
#include <hts.h>
#include <bgzf.h>

// retrieve pointers to the range of records for readname
// start and end will be NULL if readname is not in the index
// otherwise start will point at the first record with readname and end will point to one past the last record
void bam_read_idx_get_range(const bam_read_idx* bri, 
                            char* readname, 
                            bam_read_idx_record** start, 
                            bam_read_idx_record** end);

// fill in b for a single alignment
void bam_read_idx_get_by_record(htsFile* fp, bam_hdr_t* hdr, bam1_t* b, bam_read_idx_record* bri_record);

// main of the "get" subprogram
int bam_read_idx_get_main(int argc, char** argv); 