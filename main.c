//
//  main.c
//  W4112P2Step1
//
//  Created by chen on 16/4/3.
//  Copyright © 2016年 Yihe. All rights reserved.
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define MAX_INT 0x7FFFFFFF
#define INT_MIN     (-2147483647 - 1)

typedef struct {
    size_t index;
    uint32_t num[625];
} rand32_t;

rand32_t *rand32_init(uint32_t x)
{
    rand32_t *s = malloc(sizeof(rand32_t));
    uint32_t *n = s->num;
    size_t i = 1;
    n[0] = x;
    do {
        x = 0x6c078965 * (x ^ (x >> 30));
        n[i] = x;
    } while (++i != 624);
    s->index = i;
    return s;
}

uint32_t rand32_next(rand32_t *s)
{
    uint32_t x, *n = s->num;
    size_t i = s->index;
    if (i == 624) {
        i = 0;
        do {
            x = (n[i] & 0x80000000) + (n[i + 1] & 0x7fffffff);
            n[i] = (n[i + 397] ^ (x >> 1)) ^ (0x9908b0df & -(x & 1));
        } while (++i != 227);
        n[624] = n[0];
        do {
            x = (n[i] & 0x80000000) + (n[i + 1] & 0x7fffffff);
            n[i] = (n[i - 227] ^ (x >> 1)) ^ (0x9908b0df & -(x & 1));
        } while (++i != 624);
        i = 0;
    }
    x = n[i];
    x ^= (x >> 11);
    x ^= (x <<  7) & 0x9d2c5680;
    x ^= (x << 15) & 0xefc60000;
    x ^= (x >> 18);
    s->index = i + 1;
    return x;
}

int int32_cmp(const void *x, const void *y)
{
    int32_t a = * (const int*) x;
    int32_t b = * (const int*) y;
    return a < b ? -1 : a > b ? 1 : 0;
}

int32_t *generate(size_t n, rand32_t *gen)
{
    size_t i;
    int32_t *a = malloc(n << 2);
    for (i = 0 ; i != n ; ++i)
        a[i] = rand32_next(gen);
    return a;
}

int32_t *generate_sorted_unique(size_t n, rand32_t *gen)
{
    size_t i = 0;
    size_t m = n / 0.7;
    uint8_t z = 0;
    uint32_t *a = malloc(n << 2);
    uint32_t *b = calloc(m, 4);
    while (i != n) {
        uint32_t k = rand32_next(gen);
        if (k != 0) {
            size_t h = (uint32_t) (k * 0x9e3779b1);
            h = (h * (uint64_t) m) >> 32;
            while (b[h] != k) {
                if (b[h] == 0) {
                    b[h] = a[i++] = k;
                    break;
                }
                if (++h == m) h = 0;
            }
        } else if (z == 0) {
            a[i++] = 0;
            z = 1;
        }
    }
    free(b);
    qsort(a, n, 4, int32_cmp);
    return (int32_t*) a;
}

void ratio_per_bit(const int32_t *a, size_t n)
{
    size_t i, j, *c = calloc(32, sizeof(size_t));
    for (i = 0 ; i != n ; ++i) {
        int32_t x = a[i];
        for (j = 0 ; j != 32 ; ++j)
            c[j] += (a[i] >> j) & 1;
    }
    for (j = 0 ; j != 32 ; ++j)
        fprintf(stderr, "%2ld: %.2f%%\n", j + 1, c[j] * 100.0 / n);
    free(c);
}

int main(int argc, const char * argv[]) {
    // initiate
    int counter = 0;
    int counter2 = 0;
    size_t counter3 = 0;
    // check arguement numbers
    if (argc < 4){
        fprintf(stderr , "Error 1 : not enough arguments\n");
        exit(0);
    }
    // record value from command line
    int argumentCount = argc;
    int fanout[argc - 3];
    size_t k = atoi(argv[1]);
    int p = atoi(argv[2]);
    int levels = argc - 3;
    int smallestNumberK = 0;
    int largestNumberK = 0;
    int levelNumber[levels]; // record the maximum number of nodes of every level
    int branchNumber[levels]; //number of delimiters in each branch
    for ( counter = 3 ; counter < argc ; counter++ )
    {
        fanout[counter -3] = atoi(argv[counter]);
    }
    levelNumber[0] = 1;
    for ( counter = 1 ; counter < levels ; counter++ ){
        levelNumber[counter] = levelNumber[counter - 1] * (fanout[counter - 1]);
    }
    branchNumber[levels - 1] = 0;
    for ( counter = levels - 2 ; counter >= 0 ; counter-- ){
        branchNumber[counter] = branchNumber[counter + 1] * fanout[counter + 1] + fanout[counter + 1] - 1;
    }
//    for ( counter = 3 ; counter < argc ; counter++ )
//    {
//        printf("branchNumber level %d : %d\n" , counter - 3 ,branchNumber[counter -3]);
//    }
    // print out everything (to be removed later).
//    printf("K : %zu\n" , k);
//    printf("P : %d\n" , p);
//    for ( counter = 3 ; counter < argc ; counter++ )
//    {
//        printf("Fanout level %d : %d\n" , counter - 3 , fanout[counter -3]);
//    }
    
    // check if there are too many keys
    for ( counter = 0 ; counter < levels ; counter++ ){
        largestNumberK += levelNumber[counter] * (fanout[counter] - 1);
//        printf("largestNumber for level %d : %d\n" , counter, levelNumber[counter] * (fanout[counter] - 1));
    }
//    printf("largestNumberK : %d\n", largestNumberK);
    if ( k > largestNumberK){
        printf("Error 2 : too many keys\n");
        exit(0);
    }
    // check if there are enough keys
    if (k < (largestNumberK + 1) / fanout[0] ){
//        printf("%d\n" , (largestNumberK + 1) / fanout[0]);
        printf("Error 1 : not enough keys\n");
        exit(-1);
    }
    
    // calculate parameter to construct the tree
    rand32_t *gen = rand32_init(time(NULL));
    int32_t *delimiters = generate_sorted_unique(k, gen);
    free(gen);
    int tempCounter[levels]; //determine wheather a node is filled
    int levelCounter[levels]; //record number of delimiters for each level, posix_memalign takes it as indicator of space of relocating
    for (counter = 0 ; counter < levels ; counter++){
        //initiate
        tempCounter[counter] = 0;
        levelCounter[counter] = 0;
    }
    //construct the tree
    for (counter3 = 0 ; counter3 < k ; counter3++){
//        for(int i = 0; i < levels ; i++){
//            //print out the actual allocation
//            printf("round %zu allocation for level %d : %d\n" , counter3, i , levelCounter[i]);
//        }
//        printf("\n");
        for (counter = levels -1; counter >= 0 ; counter--){
            //determing levelCounter
            if (tempCounter[counter] == (fanout[counter] - 1)){
                continue;
            }
            else{
                tempCounter[counter]++;
                levelCounter[counter]++;
                for(counter2 = counter + 1 ; counter2 < levels ; counter2++){
                    // reset temporary record for all lower levels to be 0
                    tempCounter[counter2] = 0;
                }
                break;
            }
        }
    }
//    for(counter = 0; counter < levels ; counter++){
//        //print out the actual allocation
//        printf("allocation for level %d : %d\n" , counter , levelCounter[counter]);
//    }
    if(levelCounter[levels - 1] % (fanout[levels - 1] - 1)){
        levelCounter[levels - 1] = (levelCounter[levels - 1] / (fanout[levels - 1] - 1) + 1) * (fanout[levels - 1] - 1);
    }
    else{
        levelCounter[levels - 1] = (levelCounter[levels - 1] / (fanout[levels - 1] - 1)) * (fanout[levels - 1] - 1);
    }
    for (counter = levels - 2 ; counter >= 0 ; counter--){
        if(levelCounter[counter + 1] % (fanout[counter + 1] - 1)){
            levelCounter[counter] = (levelCounter[counter + 1] / (fanout[counter + 1] - 1) + 1);
        }
        else{
            levelCounter[counter] = (levelCounter[counter + 1] / (fanout[counter + 1] - 1));
        }
        if(levelCounter[counter] % fanout[counter]){
            levelCounter[counter] = (levelCounter[counter] / fanout[counter] + 1) * (fanout[counter] - 1);
        }
        else{
            levelCounter[counter] = (levelCounter[counter] / fanout[counter]) * (fanout[counter] - 1);
        }
    }
//    for(counter = 0; counter < levels ; counter++){
//        //print out the actual allocation
//        printf("allocation for level %d : %d\n" , counter , levelCounter[counter]);
//    }
    
    //construct the tree
    void *tree[levels];
    for ( counter = 0 ; counter < levels ; counter++){
        int32_t err = posix_memalign(&tree[counter], 32, levelCounter[counter] * 32);
        if(err){
            fprintf(stderr , "Allocating error number: %d\n", err);
            exit(-2);
        }
    }
    for (counter = 0 ; counter < levels ; counter++){
        //initiate the tree
        for (counter2 = 0 ; counter2 < levelCounter[counter] ; counter2++){
            ((int32_t *)tree[counter])[counter2] = MAX_INT;
        }
    }
    
//    for (counter = 0 ; counter < levels ; counter++){
//        //print the tree
//        printf("level %d : ", counter);
//        for (counter2 = 0 ; counter2 < levelCounter[counter] ; counter2++){
//            printf("%04x ",((int32_t *)tree[counter])[counter2]);
//        }
//        printf("\n");
//    }
    int levelCounter2[levels];
    for (counter = 0 ; counter < levels ; counter++){
        //initiate
        tempCounter[counter] = 0;
        levelCounter2[counter] = 0;
    }
    for (counter3 = 0 ; counter3 < k ; counter3++){
        //counter3 represents pointer on delimiter[]
        for (counter = levels -1; counter >= 0 ; counter--){
            //counter represents pointer to tree level
            if (tempCounter[counter] == fanout[counter] - 1){
                continue;
            }
            else{
                ((int32_t *)tree[counter])[levelCounter2[counter]] = delimiters[counter3];
                tempCounter[counter]++;
                levelCounter2[counter]++;
                for(counter2 = counter + 1 ; counter2 < levels ; counter2++){
                    // reset temporary record for all lower levels to be 0
                    tempCounter[counter2] = 0;
                }
                break;
            }
        }
    }
//    for (counter = 0 ; counter < levels ; counter++){
//        //print the tree
//        printf("level %d : ", counter);
//        for (counter2 = 0 ; counter2 < levelCounter[counter] ; counter2++){
//            printf("%d ",((int32_t *)tree[counter])[counter2]);
//        }
//        printf("\n");
//    }
    
    //probe
    
    //load probes
    rand32_t *gen2 = rand32_init(time(NULL));
    int32_t *probes = generate(p, gen2);
//    int32_t probes[1];
//    probes[0] = 5;
    free(gen2);
    int position = 0;
    for(counter = 0 ; counter < p ; counter++){
        counter3 = 0;
        position = 0;
        for(counter2 = 0 ; counter2 < levels ; counter2++){
        if(probes[counter] <= ((int32_t *)tree[levels - 1])[0]){
            fprintf(stdout , "probe%d : [%d , %d]\n",counter + 1 , INT_MIN,((int32_t *)tree[levels - 1])[0]);
            continue;
        }
        if(probes[counter] > delimiters[k - 1]){
            fprintf(stdout, "probe%d : [%d , %d]\n" ,counter + 1 , delimiters[k - 1], MAX_INT);
            continue;
        }
        for (counter2 = 0 ; counter2 < levels ; counter2++){
//            printf("counter3 : %zu\n", counter3);
            for(counter3 = position * (fanout[counter2] - 1); counter3 < (position + 1) * (fanout[counter2] - 1) ; counter3++){
                if(counter3 > levelCounter[counter2] - 1 ){
                    break;
                }
                if(probes[counter] <= ((int32_t *)tree[counter2])[counter3]){
                    position = counter3;
                    break;
                }
                else{
                    position = counter3 + 1;
                }
            }
                
        }
        fprintf(stdout , "probe%d : [%d , %d]\n",counter + 1 , ((int32_t *)tree[levels - 1])[counter3 - 1],((int32_t *)tree[levels - 1])[counter3]);
        if((probes[counter] > ((int32_t *)tree[levels - 1])[counter3 - 1]) && (probes[counter] <= ((int32_t *)tree[levels - 1])[counter3])){
//            printf("ok\n");
        }
        else{
//            printf("probe %d , left : %d, right: %d range %zu\n" , probes[counter],((int32_t *)tree[levels - 1])[counter3 - 1], ((int32_t *)tree[levels - 1])[counter3], counter3 - 1);
            }
        }
    }
    return 0;
    
}
