# snippets：code snippets

### 1. aitchison distance matrix

**compile**

```
cd aitchison
cc -g -Wall -Wno-unused-function -Wc++-compat -O2   aitchison.c  -o  aitchison  kstring.c  -lz -lm
```

**example**

```
 ./aitchison zotu_table.txt
```