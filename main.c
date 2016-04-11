//
//  main.c
//  W4112 Project 2
//  Yihe Chen yc3075
//  David Chen dhc2129
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

int main(int argc, const char* argv[]) {

    // initialize counters
    int counter = 0;
    int counter2 = 0;
    size_t counter3 = 0;

    // Check for proper argument count
    if (argc < 4){
        fprintf(stderr , "Error 1 : Incorect Argument Count\n");
        fprintf(stderr, "Proper Usage: ./build K P { fanout list }\n");
        exit(0);
    }

    // Gather values from arguments
    int levels = argc - 3;

    size_t k = atoi(argv[1]);
    int p = atoi(argv[2]);

    int largestNumberK = 0;
    int fanout[levels]; //record fanout for each level
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
    
    // check if there are too many keys
    for ( counter = 0 ; counter < levels ; counter++ ){
        largestNumberK += levelNumber[counter] * (fanout[counter] - 1);
    }
    if ( k > largestNumberK){
        printf("Error 2 : too many keys\n");
        exit(0);
    }

    // check if there are enough keys
    if (k < (largestNumberK + 1) / fanout[0] ){
        printf("Error 3 : not enough keys\n");
        exit(-1);
    }
    
    //generate keys
    rand32_t *gen = rand32_init(time(NULL));
    int32_t *delimiters = generate_sorted_unique(k, gen);

    //generate probes
    int32_t *probes = generate_sorted_unique(p, gen);
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
    
    //allocate ints
    void *tree[levels];
    for ( counter = 0 ; counter < levels ; counter++){
        int32_t err = posix_memalign(&tree[counter], 32, levelCounter[counter] * 32);
        if(err){
            fprintf(stderr , "Allocating error number: %d\n", err);
            exit(-2);
        }
    }

    for (counter = 0 ; counter < levels ; counter++){
        //initilize the tree with max values
        for (counter2 = 0 ; counter2 < levelCounter[counter] ; counter2++){
            ((int32_t *)tree[counter])[counter2] = MAX_INT;
        }
    }

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

    // //testing print tree
    // fprintf(stdout, "Generated tree\n");
    // for (int i = 0 ; i < levels ; i++){
    //     fprintf(stdout,"level %d : ", i);
    //     for (int j = 0 ; j < levelCounter[i] ; j++){
    //         fprintf(stdout, "%d ",((int32_t *)tree[i])[j]);
    //     }
    //     fprintf(stdout, "\n");
    // }
    // //testing probes
    // for(int i = 0; i<p; i++){
    //     fprintf(stdout, "probe %d: %d\n", i, probes[i]);
    // }

    //probe the index (phase 2)

    int rangeIds[p];
    int first, last, middle, pVal, curLevel, curFanout, prevLoc, range; 
    int32_t * curArr;   
    int i;
    for(i =0; i<p; i++){
        pVal = probes[i];
        curLevel = 0;
        prevLoc = 0;
        range = 0;
        while(curLevel != levels -1 ){
            curFanout = fanout[curLevel];
            curArr = (int32_t *)tree[curLevel];
            first = prevLoc * (curFanout-1);
            last = first + curFanout - 2;
            middle = (first + last)/2;

            while (first < last){
                if (pVal <= curArr[middle])
                    last = middle - 1;
                else
                    first = middle + 1;
                middle = (first + last)/2;
            }
            if (pVal <= curArr[middle]){
                range += middle;
                prevLoc = middle + (middle/(curFanout-1));
            }
            else{
                prevLoc = middle + (middle/(curFanout-1)) + 1;
                range += middle + 1;
            }
            curLevel ++;
        }
        curFanout = fanout[curLevel];
        curArr = (int32_t *)tree[curLevel];
        first = prevLoc * (curFanout-1);
        last = first + curFanout - 2;
        middle = (first + last)/2;
        while (first < last){
            if (pVal <= curArr[middle])
                last = middle - 1;
            else
                first = middle + 1;
            middle = (first + last)/2;
        }    
        if(pVal<=curArr[middle]){
            range += middle;
            rangeIds[i] = range;
        }
        else {
            range += middle + 1;
            rangeIds[i] = range;
        }  
    }

    //phase 3, write to stdout
    for (i=0; i<p; i++){
        fprintf(stdout, "%d %d\n", i, rangeIds[i]);
    }

    return 0;
    
}
