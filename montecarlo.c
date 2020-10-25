#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<math.h>
#define PI 3.1416

float MonteCarlo ( float a , float b, float N )
{
    int i;
    float sum = 0;
    float x ;
    for(i=0;i<N;i++)
    {
        x = ((float)rand()/RAND_MAX)*b;
        sum = sum + sin(x);
    }

    return sum / N * (b-a);
}

main() {
   float a , b , N ,D;
   a=0;
   b=PI/4;
   scanf("%f",&N);
   D = MonteCarlo(a,b,N);
   printf("%f",D);
}
