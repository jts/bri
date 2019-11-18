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

typedef struct bam_read_idx_record
{
    // When building the index and storing it on disk
    // the read name for this record is stored as an
    // offset into readnames. When it is loaded from
    // disk the size of the index is fixed and we
    // convert the offset into a direct pointer.
    union read_name {
        size_t offset;
        char* ptr;
    } read_name;

    size_t file_offset;   
} bam_read_idx_record;

//
typedef struct bam_read_idx
{
    // read names are stored contiguously in one large block
    // of memory as null terminated strings
    size_t name_capacity_bytes;
    size_t name_count_bytes;
    char* readnames;

    // records giving the offset into the bam file
    size_t record_capacity;
    size_t record_count;
    bam_read_idx_record* records;
} bam_read_idx;

// main of subprogram - build an index for a file
// given in the command line arguments
int bri_index_main(int argc, char** argv); 

// load the index for the specific bam file
bam_read_idx* bam_read_idx_load(const char* input_bam);

// cleanup the index by deallocating everything
void bam_read_idx_destroy(bam_read_idx* bri);

