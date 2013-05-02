/*******************************************************************************
*
*    $Id: $
*
*    Randal C. Burns
*    Department of Computer Science
*    Johns Hopkins University
*
*    $Source: $
*    $Date: $
*    $Revision: $
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

/*******************************************************************************
 *  NNSmoother: Nearest Neighbor Smoother
 *
 *  This routine implements a simple kernel smoother based on the mean of all 
 *  points within the kernel.
 *
 *  http://en.wikipedia.org/wiki/Kernel_smoother
 *
 *  For a grid of width = mxn the smoothed region is only well defined for 
 *  interior square defined by the corners 
 *      (width,width) and  (m-width-1,n-width-1) 
 *  i.e. only for points far enough inside the boundary for which all neighbors
 *  inside the kernel are defined.
 *  (There were other options, e.g. make the domain cylic or
 *   compute only on the available neighbors.  I chose the simplest.)
 *
 ********************************************************************************/

/*
 *  Function:  NNSmother
 * 
 *  Calculate the smoothed value for an individual point.
 *   This function could used arbitrarily more complex algorithms.
 *   We chose the simplest.
 *
 *  This functions implements the mean value kernel smoother.
 *
 *  Inputs:
 *     dim -- the dimension the kernel (2*width+1)
 *     buf -- contains a dim*dim contiguous array of values.
 *  
 *  Returns: the mean value
 */
float NNSmoother ( unsigned dim, float* buf )
{
  float value = 0;

  for ( unsigned j=0; j<dim; j++ )
  {
    for ( unsigned i=0; i<dim; i++ )
    {
      value += buf[j*dim + i];
    }
  }
  value /= dim*dim;
  return value;
}

/*
 *  Function; cut
 *
 *  Cut out an array of size (2*width+1)^2 
 *   from inField centered at x,y  and put it into outField
 */
void cut ( unsigned x, unsigned y, unsigned dim, unsigned width, float* inField, float* outField )
{
  unsigned cutdim = 2*width + 1; 

  for ( unsigned j=0; j<cutdim; j++ )
  {
    for ( unsigned i=0; i<cutdim; i++ )
    {
      outField[ cutdim*j + i ] = inField[ dim*(j+y-width) + x+i-width ];
    }
  }
  return;
}


/*
 *  Function: SmoothField
 *
 *  Smooth a field of dimension dim, dim using a kernel of width width
 *  pFieldIn should point to an input array of size dim*dim
 *  pFieldOut should point to an output array of size dim*dim.
 *     One should initialize(memset) pFieldOut to 0 so that the border
 *     do not contain non-zero values
 */
bool SmoothField ( unsigned dim, unsigned width, float* pFieldIn, float* pFieldOut ) 
{ 
  float* buf = (float *) malloc ((2*width+1)*(2*width+1)*sizeof(float) );
  memset (buf, 0, (2*width+1)*(2*width+1)*sizeof(float));

  for ( unsigned j=width; j<dim-width; j++ )
  {
    for ( unsigned i=width; i<dim-width; i++ )
    {
      cut ( i, j, dim, width, pFieldIn, buf);      
      pFieldOut[j*dim+i] = NNSmoother ( 2*width+1, buf );
    }
  }
  free(buf);
  return true;
}


/*
 * Function: InitField
 *
 *  Initiliaze the input field to well known values for debuggined 
 *  and visualization purposes.
 */
void InitField ( unsigned dim, float* pField )
{
  for ( unsigned j=0; j<dim; j++ )
  {
    for ( unsigned i=0; i<dim; i++ )
    {
      pField[j*dim+i] = j + i;
    }
  }
}

/*
 * Function: main
 *
 *  See inline comments describing steps.
 */
int main ()
{
  // Set the parameters of the kernel smoother.
  //  the valid region is from 
  //   corner (kernelwidth, kernelwidth) to
  //   corner (dimension-kernelwidth-1, dimension-kernelwidth-1)
    unsigned dimension = 2064;
    unsigned kernelwidth = 8; 

//  These are good small values to use.
  //unsigned dimension = 8;
  //unsigned kernelwidth = 1; 

  // Create the input field
  float *field = (float *) malloc ( dimension * dimension * sizeof(float));
  InitField ( dimension, field );

  // Create the output field
  float *out = (float *) malloc ( dimension * dimension * sizeof(float));
  memset ( out, 0, dimension * dimension *sizeof(float));

  // Collect timing information
  struct timeval ta, tb;
  gettimeofday ( &ta, NULL );

  // Invoke the kernel smoother
  SmoothField ( dimension, kernelwidth, field, out );

  // Report timing information
  gettimeofday ( &tb, NULL );

  if ( ta.tv_usec < tb.tv_usec )
  {
    printf ("Elapsed total time (s/m): %ld:%d\n", tb.tv_sec - ta.tv_sec, tb.tv_usec - ta.tv_usec );
  } else {
    printf ("Elapsed total time (s/m): %ld:%d\n", tb.tv_sec - ta.tv_sec - 1, 1000000 - tb.tv_usec + ta.tv_usec );
  }

  // See what happened.
  for ( unsigned j=dimension; j< dimension; j++ )
  {
    for ( unsigned i=0; i< dimension; i++ )
    {
      printf ("%4.4f, ", out[j*dimension + i]);
    }
    printf ("\n");
  }

  // Free the allocated fields
  free(field);
  free(out);
}  
