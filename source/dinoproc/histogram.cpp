#include "../common/def_os_selector_export.h"
#include "histogram.h"

int *dinocv_get_histogram(int num, double min, double max, double *arr, int len)
{
	int i, j;
	double breadth;
	double low;
	double high;
	int *ret;

	// return data
	ret = (int *)malloc(sizeof(int)*num);
	memset(ret, 0, sizeof(int)*num);

	// breadth of value
	breadth = (max-min)/num;
	
	// set return data
	low = min;
	high = min+breadth;
	for(i = 0 ; i < len ; i++){
		for(j = 0 ; j < num ; j++){
			
			if((j+1) == num){
				if(d_limit_cc(arr[i], low, high)){
					ret[j]++;
					break;
				}

			}
			else if(d_limit_co(arr[i], low, high)){
				// find suitable range for current index
				ret[j]++;
				break;
			}
			low +=breadth;
			high +=breadth;
		}
		low = min;
		high = min+breadth;
	}

	return ret;
}
