# COMSW4112DC-Yihe Chen (yc3076) David Chen
* Please use make to compile first
* After compile, use command
* ./build K P [fanout]
* to invoke the program

We copied the relevant methods from the provided number generators to 
our main.c file, which contains all of the code for this project. Some
 errors we catch are when the user provides too few keys for their 
 tree, too many keys for their tree, or when the argument counts are 
 too few. We make the assumption that the user will input proper 
 arguments for this project. 

Some test results below (we have commented out the test code that 
prints the tree and probe values for the assignment submission):

$ ./build 16 10 4 3 4
Generated tree
level 0 : 1540424151 2147483647 2147483647
level 1 : -1405510731 428216768 2026544212 2147483647
level 2 : -2117895539 -1603717550 -1419232442 -668226911 -411016142 53763038 589243728 991825871 1363345385 1569682274 1697781342 1959004450
probe 0: -2059699888
probe 1: -2015929413
probe 2: -1896726463
probe 3: -1765927075
probe 4: -1727910056
probe 5: -1408611339
probe 6: -1346564393
probe 7: -906016672
probe 8: 185649622
probe 9: 1324830309
0 1
1 1
2 1
3 1
4 1
5 3
6 4
7 4
8 7
9 10

$ ./build 11 10 3 2 2
Generated tree
level 0 : -1474539035 624273941
level 1 : -1715429846 388022161 1497309595
level 2 : -2077067880 -1634072432 -43377423 527742347 1044952075 2119866701
probe 0: -1511584902
probe 1: -937416340
probe 2: -719015470
probe 3: -85697804
probe 4: 138616416
probe 5: 1092791946
probe 6: 1448882711
probe 7: 1679565421
probe 8: 1688157296
probe 9: 1806861718
0 3
1 4
2 4
3 4
4 5
5 9
6 9
7 10
8 10
9 10

$ ./build 11 10 2 3 2
Generated tree
level 0 : 349078151
level 1 : -435001475 59864304 964534967 1629126628
level 2 : -1081905903 -59597384 166515373 707638299 1202215396 1997015620
probe 0: -1913483540
probe 1: -1731782057
probe 2: -1283089491
probe 3: -895619488
probe 4: -783305048
probe 5: 333881878
probe 6: 496354029
probe 7: 827646659
probe 8: 971551543
probe 9: 1353319075
0 0
1 0
2 0
3 1
4 1
5 5
6 6
7 6
8 8
9 9

$ ./build 11 10 2 3 2
Generated tree
level 0 : 1468586594
level 1 : -45361693 1342478682 1559894041 1789178678
level 2 : -164970480 1183769159 1424632529 1535337126 1693855895 1991463229
0 0
1 0
2 0
3 0
4 0
5 0
6 2
7 2
8 5
9 6

