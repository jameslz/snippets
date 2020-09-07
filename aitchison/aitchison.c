// aitchison distance 

#include <stdio.h>

#include <math.h>
#include <float.h>
#include <zlib.h>

#include "kstring.h"
#include "kseq.h"

KSTREAM_INIT(gzFile, gzread, 16384)

void multiplicative_replacement(double *x, int size, double delta){

    int i, zero = 0;
    double sum = 0.0;
    for (i= 0; i < size; ++i){
        sum += x[i];
        if( x[i] == 0 ) ++zero;
    }

    if(!zero) return;
    
    if(delta == 0) delta = pow((1.0/size), 2);
    
    double zcnts = 1 - sum * delta;    

    for (i= 0; i < size; ++i){
       if( x[i] == 0 ) x[i] = delta;
       else x[i] = x[i] * zcnts;
    }

}

void clr(double *x, int size){

    multiplicative_replacement(x, size, 0);
    int i;
    double mean, sum = 0.0;

    for (i= 0; i < size; ++i){
        x[i] = log(x[i]);
        sum  += x[i];
    }
    mean = sum/size;
    for (i= 0; i < size; ++i) x[i] =  x[i] - mean;

}

double inline euclidean (double *x, double *y, int size){

    int i;
    double val = 0.0;
    for (i= 0; i < size; ++i) val += (x[i] * x[i] - 2 * x[i] * y[i] + y[i] * y[i]);
    return sqrt(val);

}

int main(int argc, char *argv[]){

    if (optind + 1 > argc) {
        fprintf(stderr, "\n");
        fprintf(stderr, "Usage: aitchison <otutab>\n\n");
        return 1;
    }

    int tot = 0, size = 0;
    kstream_t  *ks;
    kstring_t  kt     = {0, 0, 0};
    kstring_t  title  = {0, 0, 0};

    gzFile  fp; 
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){

        ks = ks_init(fp);
        if( ks_getuntil( ks, '\n', &kt, 0) >=  0 ){
            
            if( kt.s[0] == '#'){
                kputs(kt.s, &title);
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
    double *buf[size];
    for (i = 0; i < size; ++i) buf[i] = (double *) calloc(tot, sizeof(double));


    int *fields, n;
    fp = strcmp(argv[ optind ], "-")? gzopen(argv[ optind ], "r") : gzdopen(fileno(stdin), "r"); 
    if( fp ){
        
        int line = 0;
        ks = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            if( kt.l == 0 || kt.s[0] == '#' ) continue;
            fields =  ksplit(&kt, '\t', &n);
            for (i = 1; i < n; ++i){
                buf[i - 1][line] = atof(kt.s + fields[i]);
            }
            ++line;

        }
        ks_destroy(ks);
        gzclose(fp);
    
    }

    for (i = 0; i < size; ++i) clr(buf[i], tot);

    double matrix[ size * size ];
    for (i = 0; i < size * size; ++i) matrix[i] = -1;

    for (i = 0; i < size; ++i){
        for (j = 0; j < size; ++j){
            if(i == j) matrix[i * size + j]  = 0;
            else if( matrix[i * size + j] == -1)
                 matrix[i * size + j] =  matrix[j * size + i] = euclidean(buf[i], buf[j], tot);
        }
        
    }

    fields = ksplit(&title, '\t', &n);
    for (i = 1; i < n; ++i) printf("\t%s", title.s + fields[i]);
    printf("\n");

    for (i = 0; i < size; ++i){
        printf("%s", title.s + fields[i + 1]);
        for (j = 0; j < size; ++j) printf("\t%.4g", matrix[i * size + j]);
        printf("\n");
    }

    free(kt.s);
    free(title.s);
    return 0;

}