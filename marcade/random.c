#include <stdio.h>
#include <stdlib.h>

short marc_random(unsigned long) ;

int main(int argc, char *argv[]) {
    int seed = 0;
    int i = 0;
    int p = 0;

/*    if (argc != 2) {
	printf("Usage: random [seed] \n \n Where [seed] is a number.\n\n");
    } else {*/
    printf("All outputs for marc_random with seed = 0 to 9999:\n");
    for (i = 0; i < 1000; i += 1) {
	seed = i;
	p += 1;
	if (p > 10) {
	    printf("\n");
	    p = 0;
	}
	printf("%hu	", marc_random(seed));	
    }
    //}
    
}

short marc_random(unsigned long egg) 
{ 
    unsigned long seed = egg; 
    unsigned char *s = (unsigned char *)&seed, a, b, x, y; 

    a = *(s+0); 
    x = (a & 0x2) >> 1; 
    a >>= 2;
    b = *(s+2); 
    y = (b & 0x80) >> 7;
    b <<= 1; 
    b |= x;
    *(s+2) = b; 
    b = *(s+1);
    x = (b & 0x80) >> 7; 
    b <<= 1;
    b |= y; 
    *(s+1) = b;
    x = 1 - x; 
    b = *(s+0); 
    a -= b + x; 
    x = a & 0x1; 
    a >>= 1; 
    b = *(s+0); 
    b >>= 1; 
    b |= (x << 7); 
    *(s+0) = b; 
    a = *(s+0); 
    b = *(s+1);
    a = (a | b) & (~(a & b));
    a &= 0xFF;
    return a;
}
