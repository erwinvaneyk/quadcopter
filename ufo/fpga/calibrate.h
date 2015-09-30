/*
*		NOT USED NOW
*/

 /* calibrate.h */
#ifndef CALIBRATE_H_
#define CALIBRATE_H_

//we could do it this way, but maybe shouldnt
void calibrate(int& sax, int& say, int& saz, int& sp, int& sq, int& sr, int& sax0, int& say0, int& saz0, int& sp0, int& sq0, int& sr0);
int zp(int sp, int sp0);
int zq(int sq, int sq0);
int zr(int sr, int sr0);

int zax(int sax, int sax0);
int zay(int say, int say0);
int zay(int sr);

#endif