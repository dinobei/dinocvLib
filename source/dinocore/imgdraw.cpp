#include "../common/def_os_selector_export.h"
#include "imgdraw.h"

/*
 * 기능 : truecolor 이미지에 지정한 color로 지정한 rect를 그린다.
 * 주의 : 직접적으로 호출되지 않고 dinocv_draw_rect 함수에서 호출되는 함수이다.
 */
void _dinocv_draw_rect_bmp32(IMAGE_D *img, RECT_D *rect, COLOR_D *color, int thick){
	register uint_d i, j;
	int left, top, right, bottom;
	uint_d leftInner, topInner, rightInner, bottomInner;
	uint_d leftOuter, topOuter, rightOuter, bottomOuter;
	int adt, halfsize, r,g,b;
	int rect_width, rect_height;
	int draw_width, draw_height;
	uchar_d **bmp32 = img->source;

	rect_width = rect->right-rect->left;
	rect_height = rect->bottom-rect->top;
	left = rect->left;
	top  = rect->top;
	right = rect->right;
	bottom = rect->bottom;
	r = color->r;
	g = color->g;
	b = color->b;
	
	if(thick%2)	adt=0;
	else		adt=1;

	halfsize = thick>>1;
	//outter point
	leftOuter = left-halfsize;
	topOuter = top-halfsize;
	rightOuter = right+halfsize;
	bottomOuter = bottom+halfsize;
	//inner point
	leftInner = left+halfsize-adt;
	topInner = top+halfsize-adt;
	rightInner = right-halfsize+adt;
	bottomInner = bottom-halfsize+adt;

	if(leftOuter < 0 ) leftOuter = 0;
	if(topOuter < 0 ) topOuter = 0;
	if(rightOuter >= img->width) rightOuter = img->width-1;
	if(bottomOuter >= img->height) bottomOuter = img->height-1;

	draw_width = rightOuter-leftOuter;
	draw_height = bottomOuter-topOuter;
	if(rect_width <= thick || rect_height <= thick){
		for(i = leftOuter ; i < rightOuter ; i++){
			for(j = topOuter ; j < bottomOuter ; j++){
				bmp32[j][i*4+0] = b;  //B
				bmp32[j][i*4+1] = g;  //G
				bmp32[j][i*4+2] = r;  //R
				bmp32[j][i*4+3] = r;  //Alph
			}
		}
		return;
	}

	for(i=leftInner; i<=rightInner;i++)
	{
		// above horizontal line
		for(j=topOuter; j<=topInner; j++)
		{
			bmp32[j][i*4+0] = b;  //B
			bmp32[j][i*4+1] = g;  //G
			bmp32[j][i*4+2] = r;  //R
			bmp32[j][i*4+3] = 0;  //Alph
		}

		// below horizontal line
		for(j=bottomInner; j<=bottomOuter; j++)
		{
			bmp32[j][i*4+0] = b;  //B
			bmp32[j][i*4+1] = g;  //G
			bmp32[j][i*4+2] = r;  //R
			bmp32[j][i*4+3] = 0;  //Alph
		}
	}

	for(i=topOuter; i<=bottomOuter; i++)
	{
		for(j=leftOuter; j<=leftInner;j++)
		{
			bmp32[i][j*4+0] = b;  //B
			bmp32[i][j*4+1] = g;  //G
			bmp32[i][j*4+2] = r;  //R
			bmp32[i][j*4+3] = 0;  //Alph
		}
		for(j=rightInner; j<=rightOuter; j++)
		{
			bmp32[i][j*4+0] = b;  //B
			bmp32[i][j*4+1] = g;  //G
			bmp32[i][j*4+2] = r;  //R
			bmp32[i][j*4+3] = 0;  //Alph
		}
	}
}

void _dinocv_draw_rect_bmp24(IMAGE_D *img, RECT_D *rect, COLOR_D *color, int thick){
	register uint_d i, j;
	int left, top, right, bottom;
	uint_d halfsize, leftInner, topInner, rightInner, bottomInner, leftOuter, topOuter, rightOuter, bottomOuter;
	int r,g,b;
	int adt;
	uchar_d **bmp24 = img->source;
	int rect_width, rect_height;
	int draw_width, draw_height;

	left = rect->left;
	top  = rect->top;
	right = rect->right;
	bottom = rect->bottom;
	r = color->r;
	g = color->g;
	b = color->b;

	rect_width = rect->right-rect->left;
	rect_height = rect->bottom-rect->top;

	if(thick%2)	adt=0;
	else		adt=1;

	halfsize = thick>>1;
	//outter point
	leftOuter = left-halfsize;
	topOuter = top-halfsize;
	rightOuter = right+halfsize;
	bottomOuter = bottom+halfsize;
	//inner point
	leftInner = left+halfsize-adt;
	topInner = top+halfsize-adt;
	rightInner = right-halfsize+adt;
	bottomInner = bottom-halfsize+adt;

	if(leftOuter < 0 ) leftOuter = 0;
	if(topOuter < 0 ) topOuter = 0;
	if(rightOuter >= img->width) rightOuter = img->width-1;
	if(bottomOuter >= img->height) bottomOuter = img->height-1;

	draw_width = rightOuter-leftOuter;
	draw_height = bottomOuter-topOuter;
	if(rect_width <= thick || rect_height <= thick){
		for(i = leftOuter ; i < rightOuter ; i++){
			for(j = topOuter ; j < bottomOuter ; j++){
				bmp24[j][i*3+0] = b;  //B
				bmp24[j][i*3+1] = g;  //G
				bmp24[j][i*3+2] = r;  //R
			}
		}
		return;
	}

	for(i=leftInner; i<=rightInner;i++)
	{
		// above horizontal line
		for(j=topOuter; j<=topInner; j++)
		{
			bmp24[j][i*3+0] = b;  //B
			bmp24[j][i*3+1] = g;  //G
			bmp24[j][i*3+2] = r;  //R
		}

		// below horizontal line
		for(j=bottomInner; j<=bottomOuter; j++)
		{
			bmp24[j][i*3+0] = b;
			bmp24[j][i*3+1] = g;
			bmp24[j][i*3+2] = r;
		}
	}

	for(i=topOuter; i<=bottomOuter; i++)
	{
		for(j=leftOuter; j<=leftInner;j++)
		{
			bmp24[i][j*3+0] = b;
			bmp24[i][j*3+1] = g;
			bmp24[i][j*3+2] = r;
		}
		for(j=rightInner; j<=rightOuter; j++)
		{
			bmp24[i][j*3+0] = b;
			bmp24[i][j*3+1] = g;
			bmp24[i][j*3+2] = r;
		}
	}
}

void _dinocv_draw_rect_bmp24_2(IMAGE_D *img, RECT_D *rect, COLOR_D *color, int thick){
	register uint_d i;
	uint_d left, top, right, bottom;
	int r,g,b;
	uchar_d **bmp24 = img->source;
	
	left = rect->left < 0 ? 0 : rect->left;
	top  = rect->top < 0 ? 0 : rect->top;
	right = rect->right >=  (int)img->width ? (int)img->width-1 : rect->right;
	bottom = rect->bottom >= (int)img->height ? (int)img->height-1 : rect->bottom;
	r = color->r;
	g = color->g;
	b = color->b;


	/* 중복된 곳을 그리지 않으려고, 즉 속도 향상을 위한 조건검사 */
	if(top <= bottom &&
		d_limit_co(top, 0, img->height) &&
		d_limit_co(bottom, 0, img->height))
	{
		for(i = left*3 ; i <= right*3 ; i+=3)
		{
			bmp24[top][i+0] = b;
			bmp24[top][i+1] = g;
			bmp24[top][i+2] = r;

			bmp24[bottom][i+0] = b;
			bmp24[bottom][i+1] = g;
			bmp24[bottom][i+2] = r;
		}
	}

	if(left <= right &&
		d_limit_co(left, 0, img->width) &&
		d_limit_co(right, 0, img->width))
	{
		for(i = top ; i <= bottom ; i++)
		{
			bmp24[i][left*3+0] = b;
			bmp24[i][left*3+1] = g;
			bmp24[i][left*3+2] = r;

			bmp24[i][right*3+0] = b;
			bmp24[i][right*3+1] = g;
			bmp24[i][right*3+2] = r;
		}
	}

	/*
	for(i = left*3 ; i <= right*3 ; i+=3)
	{
		bmp24[top][i+0] = b;
		bmp24[top][i+1] = g;
		bmp24[top][i+2] = r;

		bmp24[bottom][i+0] = b;
		bmp24[bottom][i+1] = g;
		bmp24[bottom][i+2] = r;
	}

	for(i = top ; i <= bottom ; i++)
	{
		bmp24[i][left*3+0] = b;
		bmp24[i][left*3+1] = g;
		bmp24[i][left*3+2] = r;

		bmp24[i][right*3+0] = b;
		bmp24[i][right*3+1] = g;
		bmp24[i][right*3+2] = r;
	}
	*/
}

/*
 * 기능 : grayscale 이미지에 지정한 color로 지정한 rect를 그린다.
 * 주의 : 직접적으로 호출되지 않고 dinocv_draw_rect 함수에서 호출되는 함수이다.
 *        추후 구현 예정.
 */
void _dinocv_draw_rect_bmp16(IMAGE_D *img, RECT_D *rect, COLOR_D *color, int thick){
	//uchar_d **bmp8;
	/* current not support */
}

/*
 * 기능 : grayscale 이미지에 지정한 color로 지정한 rect를 그린다.
 * 주의 : 직접적으로 호출되지 않고 dinocv_draw_rect 함수에서 호출되는 함수이다.
 */
void _dinocv_draw_rect_bmp8(IMAGE_D *img, RECT_D *rect, COLOR_D *color, int thick){
	register int i;
	int left, top, right, bottom;
	int clr;
	uchar_d **bmp8 = img->source;

	left	= rect->left;
	top		= rect->top;
	right	= rect->right;
	bottom	= rect->bottom;

	clr = (int)((color->r+color->g+color->b)/3.);

	for(i = left ; i <= right ; i++){
		// above horizontal line
		bmp8[top][i] = clr;

		// below horizontal line
		bmp8[bottom][i] = clr;
	}

	for(i = top+1 ; i <= bottom-1 ; i++){
		// left vertical line
		bmp8[i][left] = clr;

		// right vertical line
		bmp8[i][right] = clr;
	}
}


void dinocv_draw_rect(IMAGE_D *image, RECT_D *rect, COLOR_D *color, int thick)
{
	void (*dr_lut[3])(IMAGE_D*, RECT_D*, COLOR_D*, int) = {_dinocv_draw_rect_bmp8, _dinocv_draw_rect_bmp16, _dinocv_draw_rect_bmp24_2};
	register int i;

	if(thick==1)
	{
		dr_lut[(image->bpp>>3)-1](image, rect, color, thick);
	}
	else
	{
		int a = thick>>1;

		rect->left -= a;
		rect->top -= a;
		rect->right += a;
		rect->bottom += a;
		dr_lut[(image->bpp>>3)-1](image, rect, color, thick);
		for(i = 1 ; i < thick ; i++)
		{
			rect->left++;
			rect->top++;
			rect->right--;
			rect->bottom--;
			dr_lut[(image->bpp>>3)-1](image, rect, color, thick);
		}

	}
}

void dinocv_draw_line(IMAGE_D *img, POINT_D2 p1, POINT_D2 p2, COLOR_D *clr)
{
	/* 직선의 기울기에 의한 방법 
	int x1, y1, x2, y2;

	x1=p1[0];
	y1=p1[1];
	x2=p2[0];
	y2=p2[1];

	int dx=x2-x1, dy=y2-y1, steps,i;
	float xlnc, ylnc,x=(float)x1,y=(float)y1;

	if (abs(dx)>abs(dy))
		steps=abs(dx);
	else
		steps=abs(dy);

	xlnc=dx/(float)steps;
	ylnc=dy/(float)steps;
	if(img->bpp==24)
	{
		img->source[(int)y][(int)x*3+0]=clr->b;
		img->source[(int)y][(int)x*3+1]=clr->g;
		img->source[(int)y][(int)x*3+2]=clr->r;
	}
	else
	{
		img->source[(int)y][(int)x] = (clr->b+clr->g+clr->r)/3;
	}
	for (i=0; i<steps; i++)
	{
		x+=xlnc;
		y+=ylnc;
		if(img->bpp==24)
		{
			img->source[(int)y][(int)x*3+0]=clr->b;
			img->source[(int)y][(int)x*3+1]=clr->g;
			img->source[(int)y][(int)x*3+2]=clr->r;
		}
		else
		{
			img->source[(int)y][(int)x] = (clr->b+clr->g+clr->r)/3;
		}
	}
	*/

	/* Bres의 방법 */
	int x1, y1, x2, y2;

	x1=p1[0];
	y1=p1[1];
	x2=p2[0];
	y2=p2[1];

	int dx=d_abs(x1-x2),dy=d_abs(y1-y2);
	int p=2*dy-dx;
	int twody=2*dy, twodydx=2*(dy-dx);
	int x,y,xend;


	if(x1>x2){
		x=x2;
		y=y2;
		xend=x1;
	}
	else {
		x=x1;
		y=y1;
		xend=x2;
	}

	if(img->bpp==24)
	{
		img->source[y][x*3+0]=clr->b;
		img->source[y][x*3+1]=clr->g;
		img->source[y][x*3+2]=clr->r;
	}
	else
	{
		img->source[y][x] = (clr->b+clr->g+clr->r)/3;
	}

	while(x<xend) {
		x++;
		if(p<0)
			p+=twody;
		else {
			y++;
			p+=twodydx;
		}

		if(img->bpp==24)
		{
			img->source[y][x*3+0]=clr->b;
			img->source[y][x*3+1]=clr->g;
			img->source[y][x*3+2]=clr->r;
		}
		else
		{
			img->source[y][x] = (clr->b+clr->g+clr->r)/3;
		}
	}
}



void ellipseplotpoints(IMAGE_D *img, int xcent, int ycent, int x, int y, COLOR_D *clr)
{
	uchar_d **source;
	
	if(img->source)
		source = img->source;
	else return;

	if(xcent+x >= (int)img->width || ycent+y >= (int)img->height ||
		xcent-x < 0 || ycent-y < 0)
		return;
	if(img->bpp==24){
		img->source[ycent+y][(xcent+x)*3+0]=clr->b;
		img->source[ycent+y][(xcent+x)*3+1]=clr->g;
		img->source[ycent+y][(xcent+x)*3+2]=clr->r;

		img->source[ycent+y][(xcent-x)*3+0]=clr->b;
		img->source[ycent+y][(xcent-x)*3+1]=clr->g;
		img->source[ycent+y][(xcent-x)*3+2]=clr->r;

		img->source[ycent-y][(xcent+x)*3+0]=clr->b;
		img->source[ycent-y][(xcent+x)*3+1]=clr->g;
		img->source[ycent-y][(xcent+x)*3+2]=clr->r;

		img->source[ycent-y][(xcent-x)*3+0]=clr->b;
		img->source[ycent-y][(xcent-x)*3+1]=clr->g;
		img->source[ycent-y][(xcent-x)*3+2]=clr->r;
	}
	else if(img->bpp==8)
	{
		int gray_color = (clr->b+clr->g+clr->r)/3;
		img->source[ycent+y][xcent+x]=gray_color;
		img->source[ycent+y][xcent-x]=gray_color;
		img->source[ycent-y][xcent+x]=gray_color;
		img->source[ycent-y][xcent-x]=gray_color;
		
	}
}

#define ROUND(a) ((int)(a+0.5))
void dinocv_draw_ellipse(IMAGE_D *img, int xcent, int ycent, int rx, int ry, COLOR_D *clr)
{
	int rx2=rx*rx;
	int ry2=ry*ry;
	int tworx2 = 2*rx2;
	int twory2 = 2*ry2;
	int p;
	int x=0;
	int y=ry;
	int px=0;
	int py= tworx2 * y;


	ellipseplotpoints(img, xcent, ycent, x, y, clr);

	p = ROUND(ry2 - (rx2*ry)+(0.25*rx2));
	while (px<py) {
		x++;
		px += twory2;
		if (p<0)
			p+=ry2+px;
		else {
			y--;
			py -= tworx2;
			p += ry2 + px - py;
		}
		ellipseplotpoints(img, xcent, ycent, x, y, clr);
	}
	p= ROUND(ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2);
	while (y>0) {
		y--;
		py -= tworx2;
		if (p>0)
			p += rx2-py;
		else {
			x++;
			px += twory2;
			p += rx2 - py + px;
		}
		ellipseplotpoints (img, xcent, ycent, x, y, clr);
	}
}

void dinocv_draw_fill_polygon(IMAGE_D *img_polygon, POINT_D2 *pt_array, uint_d num, COLOR_D *clr)
{
	register unsigned int i, j;
	uchar_d **source = img_polygon->source;

	// Draw Contour
	for(i = 0 ; i < num-1 ; i++)
	{
		line_fast_truclr(img_polygon, pt_array[i][0], pt_array[i][1], pt_array[i+1][0],  pt_array[i+1][1], clr);
	}
	line_fast_truclr(img_polygon, pt_array[num-1][0], pt_array[num-1][1], pt_array[0][0],  pt_array[0][1], clr);
	
	// Fill Within The Polygon Pixel
	for(i = 1 ; i < img_polygon->height-1 ; i++)
	{
		for(j = 1 ; j < img_polygon->width-1 ; j++)
		{
			if(!source[i][j] && source[i-1][j] && source[i][j-1]){
				source[i][j]=255;
				if(source[i][j+1])
					break;
			}
		}
	}

}

void line_fast_gray(IMAGE_D *img, int x1, int y1, int x2, int y2, int color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      //plot_pixel(px,py,color);
	  img->source[py][px] = color;
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      //plot_pixel(px,py,color);
	  img->source[py][px] = color;
    }
  }
}


void line_fast_truclr(IMAGE_D *img, int x1, int y1, int x2, int y2, COLOR_D *color)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  //VGA[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
	  if(px>=0 && py>=0 && px < (int)img->width && py < (int)img->height)
	  {
			img->source[py][px*3+0] = color->r;
			img->source[py][px*3+1] = color->g;
			img->source[py][px*3+2] = color->b;
	  }
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      if(px>=0 && py>=0 && px < (int)img->width && py < (int)img->height)
	  {
			img->source[py][px*3+0] = color->r;
			img->source[py][px*3+1] = color->g;
			img->source[py][px*3+2] = color->b;
	  }
    }
  }
}

void dinocv_draw_polygon(IMAGE_D *img, POINT_D2 *pt_array, int num, COLOR_D *clr){
	register int i;
	for(i = 0 ; i < num-1 ; i++)
	{
		line_fast_truclr(img, pt_array[i][0], pt_array[i][1], pt_array[i+1][0],  pt_array[i+1][1], clr);
	}
	line_fast_truclr(img, pt_array[num-1][0], pt_array[num-1][1], pt_array[0][0],  pt_array[0][1], clr);

}