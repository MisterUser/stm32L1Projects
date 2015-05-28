#include "fft.h"
#include <stdio.h>
#include <math.h>

#define DFT_SIZE  1024
#define half_SIZE DFT_SIZE/2
#define DFT_M 10 
double ReX[DFT_SIZE]={0};
double ImX[DFT_SIZE]={0};
const double Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

int main()
{
   int i = 0;
   int p = 0;
   for(;i<32;i++)
   {
     p  = 0;
     for(;p<32;p++)
     {
        ReX[i*32+p]=Sine12bit[p];
     }
   }

   int j = 0;
   printf("ReX={");
   for(;j<DFT_SIZE-1;j++)
   {
	printf("%f,",ReX[j]);
   }
   printf("%f}\n",ReX[DFT_SIZE]);

   j = 0;
   printf("ImX={");
   for(;j<DFT_SIZE-1;j++)
   {
        printf("%f,",ImX[j]);
   }
   printf("%f}\n",ImX[DFT_SIZE]);

   FFT(1,DFT_M,ReX,ImX);
   j = 0;
   printf("{");
   for(;j<half_SIZE-1;j++)
   {
      printf("%f,",sqrt(ReX[j]*ReX[j]+ImX[j]*ImX[j]));
   }
   printf("%f}\n",sqrt(ReX[half_SIZE]*ReX[half_SIZE]+ImX[half_SIZE]*ImX[half_SIZE]));

   FFT(0,DFT_M,ReX,ImX);
   j = 0;
   printf("ReX={");
   for(;j<DFT_SIZE-1;j++)
   {
        printf("%f,",ReX[j]);
   }
   printf("%f}\n",ReX[DFT_SIZE]);

   j = 0;
   printf("ImX={");
   for(;j<DFT_SIZE-1;j++)
   {
        printf("%f,",ImX[j]);
   }
   printf("%f}",ImX[DFT_SIZE]);
   return 1;
}
