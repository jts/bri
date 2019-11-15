//---------------------------------------------------------
// Copyright 2019 Ontario Institute for Cancer Research
// Written by Jared Simpson (jared.simpson@oicr.on.ca)
//---------------------------------------------------------
//
// bri - simple utility to provide random access to
//         bam records by read name
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
    size_t read_name_offset;
    size_t file_offset;   
} bam_read_idx_record;

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

bam_read_idx* bam_read_idx_init()
{
    bam_read_idx* bri = (bam_read_idx*)malloc(sizeof(bam_read_idx));

    bri->name_capacity_bytes = 0;
    bri->name_count_bytes = 0;
    bri->readnames = NULL;

    bri->record_capacity = 0;
    bri->record_count = 0;
    bri->records = NULL;

    return bri;
}

void bam_read_idx_destroy(bam_read_idx* bri)
{
    fprintf(stderr, "[bri-destroy] %zu name bytes %zu records\n", bri->name_count_bytes, bri->record_count);
    free(bri->readnames);
    bri->readnames = NULL;

    free(bri->records);
    bri->records = NULL;

    free(bri);
}

int compare_records_by_readname(const void* r1, const void* r2, const void* names)
{
    const char* cnames = (const char*)names;
    const char* n1 = cnames + ((bam_read_idx_record*)r1)->read_name_offset;
    const char* n2 = cnames + ((bam_read_idx_record*)r2)->read_name_offset;

    return strcmp(n1, n2);
}

void bam_read_idx_save(bam_read_idx* bri, const char* filename)
{
    FILE* fp = fopen(filename, "wb");

    // Sort records by readname
    qsort_r(bri->records, bri->record_count, sizeof(bam_read_idx_record), compare_records_by_readname, bri->readnames);
    
    // write header, containing file version, the size (in bytes) of the read names
    // and the number of records. The readnames size is a placeholder and will be
    // corrected later.
    
    // version
    size_t FILE_VERSION = 1;
    fwrite(&FILE_VERSION, sizeof(FILE_VERSION), 1, fp);
    
    // readname length
    size_t readname_bytes = 0;
    fwrite(&readname_bytes, sizeof(readname_bytes), 1, fp);

    // num records
    fwrite(&bri->record_count, sizeof(bri->record_count), 1, fp);

    // Pass 1: count up the number of non-redundant read names, write them to disk
    // Also store the position in the file where the read name for each record was written
    size_t* disk_offsets_by_record = malloc(bri->record_count * sizeof(size_t));
    const char* rn = bri->readnames; // for convenience 

    for(size_t i = 0; i < bri->record_count; ++i) {
        int redundant = i > 0 && 
            strcmp(rn + bri->records[i].read_name_offset, rn + bri->records[i - 1].read_name_offset) == 0;
        disk_offsets_by_record[i] = readname_bytes; // current position in file
        if(!redundant) {
            size_t len = strlen(rn + bri->records[i].read_name_offset) + 1;
            fwrite(rn + bri->records[i].read_name_offset, len, 1, fp);
            readname_bytes += len;
        }
        fprintf(stderr, "record %zu name: %s redundant: %d\n", i, bri->readnames + bri->records[i].read_name_offset, redundant);
    }

    // Pass 2: write the records, getting the read name offset from the disk offset (rather than
    // the memory offset stored)
    for(size_t i = 0; i < bri->record_count; ++i) {
        bam_read_idx_record brir = bri->records[i];
        brir.read_name_offset = disk_offsets_by_record[i];
        fwrite(&brir, sizeof(brir), 1, fp);
    }
    
    // finish by writing the actual size of the read name segment
    fseek(fp, sizeof(FILE_VERSION), SEEK_SET);
    fwrite(&readname_bytes, sizeof(readname_bytes), 1, fp);

    free(disk_offsets_by_record);
    fclose(fp);
}

void bam_read_idx_add(bam_read_idx* bri, const char* readname, size_t offset)
{
    // 
    // add readname to collection
    //
    size_t len = strlen(readname) + 1;
    if(bri->name_count_bytes + len > bri->name_capacity_bytes) {

        // if already allocated double size, if initialization start with 1Mb
        bri->name_capacity_bytes = bri->name_capacity_bytes > 0 ? 2 * bri->name_capacity_bytes : 1024*1024;
        //fprintf(stderr, "[bri] allocating %zu bytes for names\n", bri->name_capacity_bytes);
    
        bri->readnames = realloc(bri->readnames, bri->name_capacity_bytes);
        if(bri->readnames == NULL) {
            fprintf(stderr, "[bri] malloc failed\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // in principle this incoming name can be so larger than doubling the size
    // doesn't allow it to fit. This really shouldn't happen so we'll just exit here
    if(bri->name_capacity_bytes <= bri->name_count_bytes + len) {
        //fprintf(stderr, "[bri] incoming name with length %zu is too large\n", len);
        exit(EXIT_FAILURE);
    }

    // copy name
    size_t name_offset = bri->name_count_bytes;
    strncpy(bri->readnames + bri->name_count_bytes, readname, len);
    bri->name_count_bytes += len;
    assert(bri->readnames[bri->name_count_bytes - 1] == '\0');

    //
    // add record
    //
    if(bri->record_count == bri->record_capacity) {
        bri->record_capacity = bri->record_capacity > 0 ? 2 * bri->record_capacity : 1024;
        bri->records = realloc(bri->records, sizeof(bam_read_idx_record) * bri->record_capacity);
        if(bri->records == NULL) {
            fprintf(stderr, "[bri] malloc failed\n");
            exit(EXIT_FAILURE);
        }
    }

    bri->records[bri->record_count].read_name_offset = name_offset;
    bri->records[bri->record_count].file_offset = offset;
    bri->record_count += 1;
}

bam_read_idx* build_index(const char* filename)
{
    fprintf(stderr, "[bri] indexing %s\n", filename);
    htsFile *fp = hts_open(filename, "r");
    if(fp == NULL) {
        exit(EXIT_FAILURE);
    }

    bam_read_idx* bri = bam_read_idx_init();

    bam1_t* b = bam_init1();
    bam_hdr_t *h = sam_hdr_read(fp);
    int ret = 0;
    while ((ret = sam_read1(fp, h, b)) >= 0) {
        char* readname = bam_get_qname(b);
        size_t file_offset = bgzf_tell(fp->fp.bgzf);
        bam_read_idx_add(bri, readname, file_offset);
        if(bri->record_count % 10000 == 0) {

            bam_read_idx_record brir = bri->records[bri->record_count - 1];
            fprintf(stderr, "[bri] record %zu [%zu %zu] chr: %s read: %s\n", 
                bri->record_count, brir.read_name_offset, brir.file_offset, h->target_name[b->core.tid], bri->readnames + brir.read_name_offset);
        }
    }

    bam_hdr_destroy(h);
    bam_destroy1(b);
    hts_close(fp);

    return bri;
}

int main(int argc, char** argv)
{
    char* input_bam = argv[1];
    bam_read_idx* bri = build_index(input_bam);
    bam_read_idx_save(bri, "scratch/small.bam.bri");
    bam_read_idx_destroy(bri);
    bri = NULL;
}
