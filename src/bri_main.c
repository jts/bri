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
#include "bri_index.h"
#include "bri_get.h"
#include "bri_test.h"

#define BRI_VERSION "0.2"

void print_version()
{
    printf("%s\n", BRI_VERSION);
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        fprintf(stderr, "[bri] usage bri <subprogram> [...]\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "index") == 0) {
        bam_read_idx_index_main(argc - 1, argv + 1);
    } else if(strcmp(argv[1], "get") == 0) {
       bam_read_idx_get_main(argc - 1, argv + 1);
    } else if(strcmp(argv[1], "test") == 0) {
        bam_read_idx_test_main(argc - 1, argv + 1);
    } else if(strcmp(argv[1], "version") == 0) {
        print_version();
    } else {
        fprintf(stderr, "[bri] unrecognized subprogram: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}

