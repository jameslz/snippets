# snippets：code snippets

### 1. aitchison distance matrix

**compile**

```bash
cd aitchison
cc -g -Wall -Wno-unused-function -Wc++-compat -O2   aitchison.c  -o  aitchison  kstring.c  -lz -lm
```

**example**

```
 ./aitchison zotu_table.txt
```

### 2. otutab_rare: implemetation uisng fastrand.

**fastrand**

[fastrand]: https://github.com/lemire/fastrand

** compile **

```bash
cc -g -Wall -Wno-unused-function -Wc++-compat -O2   otutab_rare.c  -o  otutab_rare  kstring.c fastrand.c  -lz
```
**example**

```
usearch -otutab_rare zotu_table.txt  -randseed 11  -sample_size  30000  -output otutab_usearch.txt
otutab_rare  -s 11  zotu_table.txt  30000   >otutab_fastrand.txt
tsv-utils  agg  fastrand:otutab_fastrand.txt  usearch:otutab_usearch.txt  >otutab.norm.txt
```

### 3. Cumulative sum scaling (css) normalization

`DO NOT USE IT`

[Differential abundance analysis for microbial marker-gene surveys]: https://www.nature.com/articles/nmeth.2658
[metagenomeSeq]: https://github.com/HCBravoLab/metagenomeSeq

normalization constant： 1000
lth quantile： 0.5

** compile **

```bash
cc -g -Wall -Wno-unused-function -Wc++-compat -O2  css.c  -o  css  kstring.c -lm  -lz
```

**interface**

```bash

Usage: css <otutab>

Options:
  -l   logarithmic transform to normalized count. [NONE].
  -f   fixed constant normalized factor 1000. [NONE].


```

**example**

```bash

./css  -l  zotu_table.txt

```