//
//  main.c
//  W4112 Project 2
//  Yihe Chen yc3075
//  David Chen dhc2129
//

#include <stdio.h>
#include <time.h>
#include "p2random2.c"

#define MAX_INT 0x7FFFFFFF

int main(int argc, const char* argv[]) {

    // initiate
    int counter = 0;
    int counter2 = 0;
    size_t counter3 = 0;

    // check arguement numbers
    if (argc < 4){
        printf("Error 1 : Incorrect Argument Count\n");
        printf("Proper Usage: ./build K P { fanout list }\n");
        exit(0);
    }

    // record value from command line
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
        printf("%d\n" , (largestNumberK + 1) / fanout[0]);
        printf("Error 1 : not enough keys\n");
        exit(-1);
    }
    
    //generate keys
    rand32_t *gen = rand32_init(time(NULL));
    int32_t *delimiters = generate_sorted_unique(k, gen);
    free(gen);

    //generate probes
    rand32_t *gen2 = rand32_init(time(NULL));
    int32_t *probes = generate_sorted_unique(p, gen2);
    free(gen2);

    printf("Key Values\n");
    for (int i =0; i<k; i++){
        printf("%d\n", delimiters[i]);
    }

    printf("Probe Values\n");
    for (int i =0; i<p; i++){
        printf("%d\n", probes[i]);
    }
    

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
            printf("Allocating error number: %d\n", err);
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

    for (counter = 0 ; counter < levels ; counter++){
        //print the tree
        printf("level %d : ", counter);
        for (counter2 = 0 ; counter2 < levelCounter[counter] ; counter2++){
            printf("%d ",((int32_t *)tree[counter])[counter2]);
        }
        printf("\n");
    }

    //probing
    int position = 0;
    int pos2 = 0;
    for(counter = 0 ; counter < p ; counter++){
        position = 0;
        for (counter2 = 0 ; counter2 < levels ; counter2 ++){
            counter3 = position * (fanout[counter2]-1);
            pos2 = (position + 1) * (fanout[counter2] -1);
            for(; counter3 < pos2 ; counter3++){
                if(probes[counter] < ((int32_t *)tree[counter2])[counter3]){
                    printf("%zu\n",counter3);
                    position = counter3;
                    break;
                }
            }
        }
        printf("probe %d : [%d , %d]\n",counter , ((int32_t *)tree[levels - 1])[counter3],((int32_t *)tree[levels - 1])[counter3 + 1]);
    }
    return 0;
    
}
