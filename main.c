//
//  main.c
//  W4112P2Step1
//
//  Created by chen on 16/4/3.
//  Copyright © 2016年 Yihe. All rights reserved.
//
#include <stdio.h>
#include <time.h>
#include "p2random2.c"

int main(int argc, const char * argv[]) {
    // initiate
    int counter = 0;
    int counter2 = 0;
    // check arguement numbers
    if (argc < 4){
        printf("Error 1 : not enough arguments\n");
        exit(0);
    }
    // record value from command line
    int argumentCount = argc;
    int fanout[argc -3];
    int k = atoi(argv[1]);
    int p = atoi(argv[2]);
    int levels = argc - 3;
    int smallestNumberK = 0;
    int largestNumberK = 0;
    int levelNumber[levels]; // record the maximum number of nodes of every level
    levelNumber[0] = fanout[0] - 1;
    for ( counter = 3 ; counter < argc ; counter++ )
    {
        fanout[counter -3] = atoi(argv[counter]);
    }
    levelNumber[0] = 1;
    for ( counter = 1 ; counter < levels ; counter++ ){
        levelNumber[counter] = levelNumber[counter - 1] * (fanout[counter - 1]);
    }
    // print out everything (to be removed later).
    printf("K : %d\n" , k);
    printf("P : %d\n" , p);
    for ( counter = 3 ; counter < argc ; counter++ )
    {
        printf("Fanout level %d : %d\n" , counter - 3 , fanout[counter -3]);
    }
    // check if there are too many keys
    for ( counter = 0 ; counter < levels ; counter++ ){
        largestNumberK += levelNumber[counter] * (fanout[counter] - 1);
        printf("%d\n" , levelNumber[counter] * (fanout[counter] - 1));
    }
    printf("largestNumberK : %d\n", largestNumberK);
    if ( k > largestNumberK){
        printf("Error 2 : too many keys\n");
    }
    // check if there are enough keys
    if (k < (largestNumberK + 1) / fanout[0] ){
        printf("%d\n" , (largestNumberK + 1) / fanout[0]);
        printf("Error 1 : not enough keys\n");
        exit(-1);
    }
    return 0;
    
}
