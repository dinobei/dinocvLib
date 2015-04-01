#include "../common/def_os_selector_export.h"
#include "../dinocore/ds_sort.h"

void dinocv_sort_insert(int *d, int n)
{
	register int i, j;
	int temp;

	for(i = 1 ; i < n ; i++)
	{
		temp=d[i];
		j=i-1;

		while(j>-1 && d[j] > temp)
		{
			d[j+1] = d[j];
			j--;
		}

		d[j+1]=temp;

	}

}
