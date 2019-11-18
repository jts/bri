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

// main of subprogram - build an index for a file
// given in the command line arguments
int bri_index_main(int argc, char** argv); 

