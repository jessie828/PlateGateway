#include <stdio.h>
#include <stdlib.h>


void ImageScale2x(unsigned char InputImage[],unsigned char OutputImage[],int iw,int ih)
{
	int i = 0;
	int j = 0;
	int h = 0;
	int k = 0;

	int f_gray;
	int h_gray;
	int e_gray;

	h=0;
	k=2*iw;
	for (i=0;i<ih;i++)
	{
		for (j=0;j<iw;j++)
		{
			e_gray=InputImage[i*(iw)+j];
			f_gray=InputImage[i*(iw)+j+1];
			h_gray=InputImage[(i+1)*(iw)+j];

			OutputImage[h]	 = (e_gray);
			OutputImage[h+1] = ((e_gray+f_gray)/2);
			OutputImage[k]	 = ((e_gray+h_gray)/2); 
			OutputImage[k+1] = ((f_gray+h_gray)/2);
	
			h+=2;
			k+=2;
		}

		h+=2*iw;
		k+=2*iw;
	}
}

void rgb256yiq(unsigned char *rgb, float *yiq)
{
  int row,col;
  float rgb2yiq[3][3]={{.299,  .587,  .114},
		       {.596, -.275, -.321},
		       {.212, -.528,  .311}};

  for(row=0; row<3; row++)
    {
      yiq[row]=0;
      for(col=0; col<3; col++)
	yiq[row] += rgb2yiq[row][col] * (float)rgb[col];
    }
}

void yiq256rgb(float *yiq, unsigned char *rgb)
{
  int row,col;
  float term;
  float yiq2rgb[3][3]= {{1.00309,   0.95485,   0.61786},
			{0.99678,  -0.27071,  -0.64479},
			{1.00850,  -1.11049,   1.69957}};

  for(row=0; row<3; row++)
    {
      term=0.0;
      for(col=0; col<3; col++)
	term += yiq2rgb[row][col] * yiq[col];
      rgb[row]=(unsigned char)(term<256.0?(term>=0.0?term:0):255);
    }
}


#define C0 0.4829629131445341
#define C1 0.8365163037378079
#define C2 0.2241438680420134
#define C3 -0.1294095225512604

void daub4(float a[], int n, int isign)
{
	float wksp[1024];
	unsigned long nh,nh1,i,j;

	if (n < 4) 
		return;

	nh1=(nh=n >> 1)+1;
	
	if (isign >= 0)
	{	
		for (i=1,j=1;j<=n-3;j+=2,i++) 
		{
			wksp[i]=C0*a[j]+C1*a[j+1]+C2*a[j+2]+C3*a[j+3];
			wksp[i+nh]=C3*a[j]-C2*a[j+1]+C1*a[j+2]-C0*a[j+3];
		}
		
		wksp[i]=C0*a[n-1]+C1*a[n]+C2*a[1]+C3*a[2];
		wksp[i+nh]=C3*a[n-1]-C2*a[n]+C1*a[1]-C0*a[2];
	} 
	else //inverse
	{
		wksp[1]=C2*a[nh]+C1*a[n]+C0*a[1]+C3*a[nh1];
		wksp[2]=C3*a[nh]-C0*a[n]+C1*a[1]-C2*a[nh1];
		
		for (i=1,j=3;i<nh;i++) 
		{
			wksp[j++]=C2*a[i]+C1*a[i+nh]+C0*a[i+1]+C3*a[i+nh1];
			wksp[j++]=C3*a[i]-C0*a[i+nh]+C1*a[i+1]-C2*a[i+nh1];
		}
	}
	
	for (i=1;i<=n;i++) 
		a[i]=wksp[i];
}

#undef C0
#undef C1
#undef C2
#undef C3

void wtd(float a[], int w, int h, int c,
	 int isign, void (*rowtxfn)(float [], int, int))
{ 
	int i,j,k,nt;  float wksp[1024];
  
	for (k=0; k<c; k++)
    {
      if (w>=4) //do rows
		for (j=0; j<h; j++)
		{
			for (i=0; i<w; i++)
				wksp[i]=a[c*(w*j+i)+k];

			if (isign >= 0)
			for(nt=w;nt>=4;nt >>= 1)
				(*rowtxfn)(wksp-1,nt,isign);
			else
			for(nt=4;nt<=w;nt <<= 1)
				(*rowtxfn)(wksp-1,nt,isign);
		    
			for (i=0; i<w; i++)
				a[c*(w*j+i)+k]=wksp[i];
		}
      
      if(h>=4) // do cols
		for (i=0; i<w; i++)
		{
			for (j=0; j<h; j++)
			wksp[j]=a[c*(w*j+i)+k];

			if (isign >= 0)
			for(nt=h;nt>=4;nt >>= 1)
				(*rowtxfn)(wksp-1,nt,isign);
			else
			for(nt=4;nt<=h;nt <<= 1)
				(*rowtxfn)(wksp-1,nt,isign);
		    
			for (j=0; j<h; j++)
				a[c*(w*j+i)+k]=wksp[j];
		}
    }
}

void wtliketargd(float a[], int w, int h, int c,
		 int isign,int iterations, void (*rowtxfn)(float [], int, int))
{
	int i,j,k,nt;  float wksp[1024];

	int counter=0;

	for(nt= (isign>0?w:4); nt>= (isign>0?4:w) && counter<iterations;  nt = (isign>0)?nt>>1:nt<<1 )
    {
		for(k=0; k<c; k++)
		{
			for(j=0; j<nt; j++)
			{
				for(i=0; i<nt; i++)
					wksp[i]=a[c*(w*j+i)+k];
			      
				(*rowtxfn)(wksp,nt,isign);

				for(i=0; i<nt; i++)
					a[c*(w*j+i)+k]=wksp[i];
			}

			for(i=0; i<nt; i++)
			{
				for(j=0; j<nt; j++)
					wksp[j]=a[c*(w*j+i)+k];

				(*rowtxfn)(wksp,nt,isign);
		      
				for(j=0; j<nt; j++)
					a[c*(w*j+i)+k]=wksp[j];
			}
		}

		counter++;
    }
}

unsigned char tmp_image[352*288];

void DoWavelet(unsigned char image_in[],unsigned char image_out[],unsigned char image_sum[],
			   int w, int h, int c, int thshold, int iterations)
{
	int i;

	int max_dim=0;

	if(w>h)
		max_dim=w;
	else
		max_dim=h;

	unsigned char *arr = new unsigned char [max_dim*max_dim*c];
    unsigned char *image_sum_tmp = new unsigned char [max_dim*max_dim];

	float *coeffs=(float *)calloc(max_dim*max_dim*c,sizeof(float));

    for(i=0; i<w*h; i++)
	{
		rgb256yiq(image_in+c*i,coeffs+c*i);
	}

	wtliketargd(coeffs,w,h,c,1,iterations,daub4);

	if(w>h)
		h=w;
	else
		w=h;

	for(i=0; i<w*h; i++)
	{
		yiq256rgb(coeffs+c*i,arr+c*i);
	}

	int counter=0;
	for(i=0; i<c*w*h; i+=c)
	{
		image_out[counter] = (int)(arr[i]*0.3 + arr[i+1]*0.59+ arr[i+2]*0.11);
		counter++;
	}

	for(i=0; i<w*h; i++)
	{
		if(image_out[i]>25)
		{
			image_out[i]=0;
		}
		else
		{
			image_out[i]=255;
		}
	}

	
	counter=0;
	for(i=(max_dim*max_dim)/2+max_dim/2; i<(max_dim*max_dim); i++)
	{
		if(i%(max_dim/2)==0)
			i+=max_dim/2;

		tmp_image[counter]=image_out[i];

		counter++;
	}

	counter=0;
	for(i=0; i<((max_dim*max_dim)/2); i++)
	{
		if((i%(max_dim))==0)
		{
			i+=max_dim/2;
		}

		if(tmp_image[counter]==0)
			image_out[i]=tmp_image[counter];

		counter++;
	}

	counter=0;
	for(i=0; i<((max_dim*max_dim)/2); i++)
	{
		if((i%(max_dim))==0)
		{
			i+=max_dim/2;
		}

		image_sum_tmp[counter]=image_out[i];

		counter++;
	}

	ImageScale2x(image_sum_tmp,image_sum,max_dim/2,max_dim/2);

	free (coeffs);

	delete []arr;
}