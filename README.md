# Bam Read Index (bri)

`bri` is a small tool to quickly retrieve alignments in a bam file by readname. Most bam files are sorted by genomic position to allow fast sequential reading across regions of the genome. Bam does not allow random access to alignments by readname as short read sequencing would require very large readname-to-file position indices. With long read sequencing however, it is sometimes desirable to extract all of the alignments for a particular read. `bri` is designed for this usecase and provides both a command line interface and htslib-inspired API.

## Installation

```
git clone https://github.com/jts/bri.git
cd bri
make HTSDIR=/path/to/htslib
```

## Usage

Index a bam file:

```
bri index reads.sorted.bam
```

Extract the alignments for a particular read:

```
bri get reads.sorted.bam ffc71c5d-5aa0-4c4c-88e8-ed686d520d8c
```


