#include "../common/def_os_selector_export.h"
#include "dp.h"

// distance square from (x, y) to the line segment AB
double point2SegDist2(int x, int y, POINT_D2 A, POINT_D2 B){
	int dx = B[0]-A[0];
	int dy = B[1]-A[1];
	double lenAB2 = dx*dx + dy*dy;
	int du = x-A[0];
	int dv = y-A[1];
	double dot = dx*du + dy*dv;
	if(lenAB2 == 0.0)
		return du*du + dv*dv;
	if(dot <= 0.0)
		return du*du + dv*dv;
	else if(dot >=lenAB2){
		du = x-B[0];
		dy = y-B[1];
		return du*du + dv*dv;
	}
	else{
		double slash = du*dy - dv*dx;
		return slash*slash/lenAB2;
	}
}

void dinocv_douglas_peucker(double tolerance, POINT_D2 *Vertex, int istart, int iend, int *mark)
{
	register int i=0;

	//end condition
	if(iend <= istart+1)
		return;
	
	int ibreak = istart;
	double maxdist2 = 0.0;
	double tol2 = tolerance * tolerance;

	for(i = istart+1 ; i < iend ; i++){
		double dist2 = point2SegDist2(Vertex[i][0], Vertex[i][1], Vertex[istart], Vertex[iend]);
		if(dist2 <= maxdist2) continue;
		ibreak = i;
		maxdist2 = dist2;
	}
	if(maxdist2 > tol2){
		mark[ibreak] = 1;

		dinocv_douglas_peucker(tolerance, Vertex, istart, ibreak, mark);
		dinocv_douglas_peucker(tolerance, Vertex, ibreak, iend, mark);
	}
	return;
}
