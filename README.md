# Bam Read Index (bri)

`bri` is a small tool to quickly retrieve alignments in a bam file by readname. Most bam files are sorted by genomic position to allow fast sequential reading across regions of the genome. Bam does not allow random access to alignments by readname as short reads  would require very large readname-to-file position indices. With long read sequencing however, it is sometimes desirable to extract all of the alignments for a particular read. `bri` is designed for this usecase and provides both a command line interface and htslib-inspired API. The index for ~50X of nanopore data for a human genome is under 1GB.

## Installation

```
git clone https://github.com/jts/bri.git
cd bri
make HTSDIR=/path/to/htslib
```

## Usage

Index a bam file:

```
> bri index reads.sorted.bam
```

Extract the alignments for a particular read (output is in SAM):

```
> bri get reads.sorted.bam ffc71c5d-5aa0-4c4c-88e8-ed686d520d8c

ffc71c5d-5aa0-4c4c-88e8-ed686d520d8c    0       chr10   12775107        60 1000M * *
ffc71c5d-5aa0-4c4c-88e8-ed686d520d8c    2064    chr10   12773732        29 800S200M * *
```


