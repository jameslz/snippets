# otutab_rare: implemetation uisng fastrand.

### fastrand

[fastrand]: https://github.com/lemire/fastrand

### compile

    cc -g -Wall -Wno-unused-function -Wc++-compat -O2   otutab_rare.c  -o  otutab_rare  kstring.c fastrand.c  -lz

### example

    usearch -otutab_rare zotu_table.txt  -randseed 11  -sample_size  30000  -output otutab_usearch.txt
    otutab_rare  -s 11  zotu_table.txt  30000   >otutab_fastrand.txt
    tsv-utils  agg  fastrand:otutab_fastrand.txt  usearch:otutab_usearch.txt  >otutab.norm.txt

