#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char** k = NULL;
    int fig = 2;
    if (fig == 2)
    {
        k = {
    "HphV0_phsA", "HphV1_phsA", "HphV2_phsA","HphV3_phsA","HphV4_phsA","HphV5_phsA","HphV6_phsA","HphV7_phsA",
    "HphV8_phsA","HphV9_phsA","HphV10_phsA","HphV11_phsA","HphV12_phsA","HphV13_phsA","HphV14_phsA","HphV15_phsA",
    "HphV16_phsA","HphV17_phsA","HphV18_phsA","HphV19_phsA","HphV20_phsA","HphV21_phsA","HphV22_phsA","HphV23_phsA",
    "HphV24_phsA","HphV25_phsA","HphV26_phsA","HphV27_phsA","HphV28_phsA","HphV29_phsA","HphV30_phsA","HphV31_phsA",
    "THDUa"
        };
    }
    printf("%s\n", k[0]);
}