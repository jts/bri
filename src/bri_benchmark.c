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
#include <getopt.h>
#include <sys/time.h>
#include "bri_index.h"
#include "bri_get.h"

enum {
    OPT_HELP = 1,
};

static const char* shortopts = ""; // placeholder
static const struct option longopts[] = {
    { "help",                      no_argument,       NULL, OPT_HELP },
    { NULL, 0, NULL, 0 }
};

void print_usage_benchmark()
{
    fprintf(stderr, "usage: bri benchmark <input.bam>\n");
}

// from: https://stackoverflow.com/questions/6127503/shuffle-array-in-c
void shuffle(const char** array, size_t n) {    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);
 
    if(n <= 1) {
        return;
    }

    size_t i;
    for (i = n - 1; i > 0; i--) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        const char* t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

//
int bam_read_idx_benchmark_main(int argc, char** argv)
{
    int die = 0;
    for (char c; (c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1;) {
        switch (c) {
            case OPT_HELP:
                print_usage_benchmark();
                exit(EXIT_SUCCESS);
        }
    }
    
    if (argc - optind < 1) {
        fprintf(stderr, "bri test: not enough arguments\n");
        die = 1;
    }

    if(die) {
        print_usage_benchmark();
        exit(EXIT_FAILURE);
    }

    char* input_bam = argv[optind++];

    // open files
    bam_read_idx* bri = bam_read_idx_load(input_bam);
    htsFile* bam_fp = hts_open(input_bam, "r");
    bam_hdr_t* h = sam_hdr_read(bam_fp);
    bam1_t* b = bam_init1();

    // get readnames, we'll shuffle these to sample reads to get
    const char** names = malloc(bri->record_count * sizeof(char*));
    
    for(size_t ri = 0; ri < bri->record_count; ++ri) {
        names[ri] = bri->records[ri].read_name.ptr;
    }

    // shuffle
    shuffle(names, bri->record_count);

    size_t trials = 100;
    for(size_t t = 0; t < trials; ++t) {

        // timer
        struct timeval start;
        gettimeofday(&start, NULL);

        size_t N = 100;
        size_t base_count = 0;
        for(size_t i = 0; i < N; ++i) {
        
            bam_read_idx_record* start;
            bam_read_idx_record* end;
            bam_read_idx_get_range(bri, names[i], &start, &end);
            while(start != end) {
            
                bam_read_idx_get_by_record(bam_fp, h, b, start);
                base_count += b->core.l_qseq;
                start++;
            }
        }

        struct timeval end;
        gettimeofday(&end, NULL);

        size_t end_us = end.tv_sec * 1000000 + end.tv_usec;
        size_t start_us = start.tv_sec * 1000000 + start.tv_usec;
        size_t diff_us = end_us - start_us;
        fprintf(stderr, "bases read: %zu time: %.2lfms\n", base_count, diff_us / 1000.0);

    }
    free(names);
    bam_destroy1(b);
    bam_hdr_destroy(h);
    hts_close(bam_fp);
    bam_read_idx_destroy(bri);
    bri = NULL;
}
