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
#include <getopt.h>
#include "bri_index.h"

enum {
    OPT_HELP = 1,
};

static const char* shortopts = ""; // placeholder
static const struct option longopts[] = {
    { "help",                      no_argument,       NULL, OPT_HELP },
    { NULL, 0, NULL, 0 }
};

void print_usage_get()
{
    fprintf(stderr, "usage: bri get <input.bam> <readname>\n");
}

//
int compare_records_by_readname_ptr(const void* r1, const void* r2)
{
    const char* n1 = ((bam_read_idx_record*)r1)->read_name.ptr;
    const char* n2 = ((bam_read_idx_record*)r2)->read_name.ptr;
    return strcmp(n1, n2);
}

int bri_get_main(int argc, char** argv)
{
    int die = 0;
    for (char c; (c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1;) {
        switch (c) {
            case OPT_HELP:
                print_usage_get();
                exit(EXIT_SUCCESS);
        }
    }
    
    if (argc - optind < 2) {
        fprintf(stderr, "bri get: not enough arguments\n");
        die = 1;
    }

    if(die) {
        print_usage_get();
        exit(EXIT_FAILURE);
    }

    char* input_bam = argv[optind++];
    char* readname = argv[optind++];

    bam_read_idx* bri = bam_read_idx_load(input_bam);
    
    htsFile* bam_fp = hts_open(input_bam, "r");
    bam_hdr_t *h = sam_hdr_read(bam_fp);
    htsFile* out_fp = hts_open("-", "w");

    //
    bam_read_idx_record query;
    query.read_name.ptr = readname;
    query.file_offset = 0;

    bam_read_idx_record* rec = bsearch(&query, bri->records, bri->record_count, sizeof(bam_read_idx_record), compare_records_by_readname_ptr);
    if(rec == NULL) {
        fprintf(stderr, "read not found in bri %s\n", readname);
    } else {
        fprintf(stderr, "read found in bri %s (%s)\n", readname, rec->read_name.ptr);
        hts_itr_t* itr = sam_itr_queryi(NULL, HTS_IDX_REST, 0, 0);
        assert(itr != NULL);
        bam1_t* b = bam_init1();

        // get next record
        int ret = bgzf_seek(bam_fp->fp.bgzf, rec->file_offset, SEEK_SET);
        if(ret != 0) {
            fprintf(stderr, "[bri] bgzf_seek failed\n");
            exit(EXIT_FAILURE);
        }

        int tid, beg, end;
        itr->readrec(bam_fp->fp.bgzf, bam_fp, b, &tid, &beg, &end);
        //int ret = sam_itr_next(bam_fp, itr, b);
        
        ret = sam_write1(out_fp, h, b);
        if(ret < 0) {
            fprintf(stderr, "[bri] sam_write1 failed\n");
            exit(EXIT_FAILURE);
        }

        hts_itr_destroy(itr);
        bam_destroy1(b);
    }

    hts_close(out_fp);
    bam_hdr_destroy(h);
    hts_close(bam_fp);
    bam_read_idx_destroy(bri);
    bri = NULL;
}
