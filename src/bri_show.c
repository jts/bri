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
#include "bri_index.h"

//
// Getopt
//
enum {
    OPT_HELP = 1,
};

static const char* shortopts = ""; // placeholder
static const struct option longopts[] = {
    { "help",                      no_argument,       NULL, OPT_HELP },
    { NULL, 0, NULL, 0 }
};

//
void print_usage_show()
{
    fprintf(stderr, "usage: bri show <index_filename.bri>\n");
}

int bam_read_idx_show_main(int argc, char** argv)
{
    int die = 0;
    for (char c; (c = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1;) {
        switch (c) {
            case OPT_HELP:
                print_usage_show();
                exit(EXIT_SUCCESS);
        }
    }
    
    if (argc - optind < 1) {
        fprintf(stderr, "bri show: not enough arguments\n");
        die = 1;
    }

    if(die) {
        print_usage_show();
        exit(EXIT_FAILURE);
    }

    char* input_bri = argv[optind++];
    bam_read_idx* bri = bam_read_idx_load(NULL, input_bri);

    for(size_t i = 0; i < bri->record_count; ++i) {
        printf("%s\n", bri->records[i].read_name.ptr);
    }

    return 0;
}
