#include <stdio.h>
#include <stdlib.h>
#include "explicit.h"
#include "residuals.h"
#include "util2d.h"

// implements the efficient solution of Qn+1 = Qn - dt * R with a loop
// without actually solving Ax = b.
// This would be useful when time scale is very small in the time accurate 
// physical problems where we really need an efficiend matrix independent explicit 
// time marching scheme
// Q is input-output and will be updated with computed values after iteration ITR_MAX
// itr_per_msg is the number of iteration that should be passed before each status message is printed.
// other variabls are kina clear. If not refer to other functions.
int efficient_euler_explicit(double *Q, double *Q_inf, double gamma, double CFL, int ITR_MAX, int itr_per_msg, int nn, int neqs, double *x, double *y, int nt, int **tri_conn, int *bn_nodes)
{

  //locals
  int i,j;
  double *R = (double *)calloc((nn*neqs) , sizeof(double));
  int ITR = 0;
  double *int_uplusc_dl = (double *)calloc(nn , sizeof(double) );

  // main iteration loop
  for( ITR = 1; ITR <= ITR_MAX; ITR++)
    {

      //finding the residuals
      calc_residuals( Q, Q_inf, gamma, nn, neqs, x, y, nt, tri_conn, bn_nodes, R);

      // calculating line integral int( (|u_bar| + c) dl ) 
      calc_int_uplusc_dl( Q, gamma, neqs, nn, x, y, nt, tri_conn, bn_nodes, int_uplusc_dl);

      if(!(ITR % itr_per_msg)) // show status each itr_per_msg time
	printf("ITR = %d, max_abs(R[1,2,3,4]) = %17.17e\n", ITR, max_abs_array(R, (neqs*nn)));
      //updating Q
      for( i = 0; i < nn; i++)
	for( j = 0; j < neqs; j++)
	  Q[i*neqs + j] = Q[i*neqs + j] - CFL/int_uplusc_dl[i] * R[i*neqs + j];


    }

  printf("The infinity condition is:\n");
  print_array("Q_inf", Q_inf, 4);

  //clean-up
  free(R);
  free(int_uplusc_dl);

  //completed successfully!
  return 0;
}
