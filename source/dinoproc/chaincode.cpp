#include "../common/def_os_selector_export.h"
#include "chaincode.h"

int dinocv_get_cahincode(int oldx, int oldy, int x, int y)
{
	int ccode, xinc, yinc;
	double m;

	yinc = y-oldy;
	xinc = x-oldx;

#define TAN22_5	0.41421356237
#define TAN67_5 2.41421356237
	if(xinc==0 && yinc>0)
		ccode=2;
	else if(xinc==0 && yinc < 0)
		ccode=6;
	else if(yinc==0 && xinc > 0)
		ccode=0;
	else if(yinc==0 && xinc < 0)
		ccode=4;
	else{
		m = (double)yinc*(1./xinc);
		if(xinc > 0){
			if(		 TAN67_5 < m				 ) ccode=2;
			else if( TAN22_5 < m && m <=  TAN67_5) ccode=1;
			else if(-TAN22_5 < m && m <=  TAN22_5) ccode=0;
			else if(-TAN67_5 < m && m <= -TAN22_5) ccode=7;
			else								   ccode=6;//m <= -TAN67_5
		}
		else{ // xinc < 0
			if(					    m <= -TAN67_5) ccode=2;
			else if(-TAN67_5 < m && m <= -TAN22_5) ccode=3;
			else if(-TAN22_5 < m && m <=  TAN22_5) ccode=4;
			else if( TAN22_5 < m && m <=  TAN67_5) ccode=5;
			else								   ccode=6;//TAN22_5 < m
		}
	}
	return ccode;
}
