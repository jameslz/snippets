#include <stdlib.h>
#include <zlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <math.h>
#include <float.h>

#include "kstring.h"
#include "kseq.h"
KSTREAM_INIT(gzFile, gzread, 16384)

#include "ksort.h"
KSORT_INIT_GENERIC(double)

#include "kvec.h"
static kvec_t( char * ) vt;

static int log_trans = 0;

double inline median (double *x,  const int size){
   
   int i;
   double buf[size];
   for (i = 0; i < size; ++i) buf[i] = x[i];

   ks_mergesort(double, size, buf, 0);

   int k = size >> 1;
   return ( (k << 1) < size) ? buf[k] : (buf[k] + buf[k - 1]) / 2;
  
}

void inline normalization (double *x,  const int size, const double sum, const double factor){
    
    int i;
    for (i = 0; i < size; ++i){
        if(x[i] == 0.0f) x[i] = 0;
        else{
           x[i] =  (log_trans == 1) ? log( x[i]*factor/sum) :  x[i]*factor/sum;
        }
    }

}

double inline agg (double *x, const double q, const int size){
    
    double sum = 0.0;
    int i;
    for (i = 0; i < size; ++i)
        if(x[i] <= q ) sum += x[i];
    
    return sum;
}

double inline quantile (double *x, double p, const int size){
    
    int i, shift = 0;
    for (i = 0; i < size; ++i){
       if( 0.0 == x[i]) ++shift;
       else break;
    }
    
    if (p >= 1.0f)  return x[size - 1];
    if( p <= 0.0f)  return x[shift];
    
    int non_zeros = size - shift;
    return x[shift + (int)(ceil( non_zeros * p) -  1)];

}

int main(int argc, char *argv[]){

    int norm = 0;
    int c;
    while ((c = getopt(argc, argv, "lf")) >= 0)
        if (c == 'l') log_trans = 1;
        else if(c == 'f')  norm = 1000;

    if (optind + 1 > argc) {
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: css <otutab>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -l   logarithmic transform to normalized count. [NONE].\n");
        fprintf(stderr, "  -f   fixed constant normalized factor 1000. [NONE].\n\n");

        return 1;
    }

    int tot = 0, size = 0;
    kstream_t  *ks;
    kstring_t  kt     = {0, 0, 0};

    gzFile  fp; 
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){

        ks = ks_init(fp);
        if( ks_getuntil( ks, '\n', &kt, 0) >=  0 ){
            
            if( kt.s[0] == '#'){
                ksplit(&kt, '\t', &size);
                size = size - 1;
            }else{
                fprintf(stderr, "[ERR]: no title fields, exit.\n");
                exit(-1);
            }
        }
        
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if( kt.l == 0 ) continue;
            tot++;
        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{        
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[ optind ]);
        exit(1);
    }
    
    if(!tot){
        fprintf(stderr, "[ERR]: empty otutab.?\n");
        exit(-1);
    }

    int i, j;
    double *buf[size], *abund[size];
    for (i = 0; i < size; ++i){
        buf[i]     = (double *) calloc(tot, sizeof(double));
        abund[i]   = (double *) calloc(tot, sizeof(double));
    }

    int *fields, n;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){
        
        int line = 0;
        ks = ks_init(fp);
        kv_init(vt);
 
        if( ks_getuntil( ks, '\n', &kt, 0) >=  0 ) printf("%s\n", kt.s);

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if( kt.l == 0 || kt.s[0] == '#' ) continue;
            fields =  ksplit(&kt, '\t', &n);
            kv_push(char*, vt, strdup(kt.s));          
            for (i = 1; i < n; ++i) abund[i - 1][line] = atof(kt.s + fields[i]);
            ++line;
        }
        
        ks_destroy(ks);
        gzclose(fp);
    
    }

    for (i = 0; i < size; ++i){
        memcpy(buf[i], abund[i], tot * sizeof(double));
        ks_mergesort(double, tot, buf[i], 0);
    }

    double sum[size], quant[size];
    for (i = 0; i < size; ++i){
        quant[i] = quantile(buf[i], 0.5, tot);
        sum[i]   = agg(buf[i], quant[i],  tot); 

    }

    double factor = (norm) ? norm : median(sum, size);

    for (i = 0; i < size; ++i) normalization(abund[i], tot, sum[i], factor);

    for (i = 0; i < tot; ++i){
        printf("%s", kv_A(vt, i) );
        free( kv_A(vt, i) );
        for (j = 0; j < size; ++j){
            (log_trans) ? printf("\t%.4g", abund[j][i]) : printf("\t%d", (int)abund[j][i]);
        }
        putchar('\n');

    }

    for (i = 0; i < size; ++i){
        if(buf[i]   != NULL)   free(buf[i]);
        if(abund[i] != NULL)   free(abund[i]);
    }
    
    free(kt.s);
    kv_destroy(vt);
    return 0;

}