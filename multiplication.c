/*
  Andy Shaw
  CSE2431 Lab4 
  View README for further details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include "matrix.h"

struct parm
{
  int start; // starting row
  int finish; // finishing row
};

struct timeval t0;
struct timeval t1;

int A[N][M];
int B[M][P];
int C[N][P];
int C_thread[N][P];

void *math(void *range);

/* check the accuracy of the matrices */
int check(int print)
{
  int i = 2, j = 2;
  int result = 0;
  
  //Compare first values
  if(C[0][0] != C_thread[0][0])
    {
      result = 1; // there is an error
    }
  if(print == 0)
    {
      printf("[%d][%d]\t\tC: %d\t\tC_thread: %d\n", 0, 0, C[0][0], C_thread[0][0]);
    }

  while(j < P)
    {
      // check values
      if(C[i][j] != C_thread[i][j])
	{
	  result = 1; // there is an error
	}
      if(print == 0)
	{
	  if(i < 10 || j < 10)
	    printf("[%d][%d]\t\tC: %d\tC_thread: %d\n", i, j, C[i][j], C_thread[i][j]);
	  else
	    printf("[%d][%d]\tC: %d\tC_thread: %d\n", i, j, C[i][j], C_thread[i][j]);
	}

      i = i + (N/20);
      j = j + (P/20);
    }

  i = N-1;
  j = P-1;
  //Compare last values
  if(C[i][j] != C_thread[i][j])
    {
      result = 1; // there is an error
    }
  if(print == 0)
    {
      printf("[%d][%d]\tC: %d\tC_thread: %d\n", i, j, C[i][j], C_thread[i][j]);
    }

  return result; // no error if result is unchanged
}

int main(void)
{
  /* initialize matrix A */
  int i, j;
  for(j = 0; j < M; j++)
    {
      for(i = 0; i < N; i++)
	{
	  // utilizing sequential locality for better speed
	  A[i][j] = i + j;
	}
    }

  /* initialize matrix B */
  for(j = 0; j < P; j++)
    {
      for(i = 0; i < M; i++)
	{
	  // utilizing sequential locality for better speed
	  B[i][j] = j;
	}
    }

  /* request thread numbers from the user */
  int threads = 1;
  printf("Enter number of threads to use.\n");
  scanf("%d", &threads);

  if(threads < 1)
    {
      printf("Invalid number of threads.\n");
      return(1);
    }
  printf("Starting calculations.\n");

  /* initialize threads */
  pthread_t thr_id [threads];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  
  /* perform multiplication without threads */
  int k;
  // start timing
  gettimeofday(&t0,0);

  // following algorithm in handout
  for(i = 0; i < N; i++)
    {
      for(j = 0; j < P; j++)
	{
	  for(k = 0; k < M; k++)
	    {
	      C[i][j] += A[i][k] * B[k][j];
	    }
	}
    }

  // finish time, and output times for 1 thread
  gettimeofday(&t1, 0);

  // start output table
  printf("\nThreads\t\tSeconds\n--------\t--------\n");

  // output time in seconds
  long time = (t1.tv_usec - t0.tv_usec)/10000;

  // was getting a negative time bug, think it's from format of tv_usec returns
  if(time < 0)
    {
      time = (t1.tv_usec*10 - t0.tv_usec)/1000;
    }
  printf("1\t\t%d.%d\n\n", (t1.tv_sec - t0.tv_sec), time);

  /* perform multiplication with each number of threads up to specified amount */
  int thr = 2;
  while(thr <= threads)
    {
      gettimeofday(&t0, 0);
      int rows = N/thr; // number of rows per thread

      // create threads and have them calculate their rows
      int i = 0, row;
      while(i < thr -1)
	{
	  row = i * rows;

	  // set start row and finish row
	  struct parm *data = (struct parm*) malloc(sizeof(struct parm));
	  data->start = row;
	  data->finish = row + rows - 1;

	  // create thread for that set of rows
	  pthread_create(&thr_id[i], &attr, math, data);

	  i++;
	}
      
      //last thread will always do remainder of the matrix
      row = i * rows;
      struct parm *data = (struct parm*) malloc(sizeof(struct parm));
      data->start = row;
      data->finish = N-1;

      pthread_create(&thr_id[thr -1], &attr, math, data);

      
      // increment thread count and join all threads and output
      for(i = 0; i < thr; i++)
	{
	  pthread_join(thr_id[i], NULL);
	}

      gettimeofday(&t1, 0);
      // output time in seconds
      long time = (t1.tv_usec - t0.tv_usec)/10000;

      // was getting a negative time bug, think it's from format of tv_usec returns
      if(time < 0)
	{
	  time = (t1.tv_usec*10 - t0.tv_usec)/1000;
	  if(time < 0)
	    {
	      time = (t1.tv_usec*100 - t0.tv_usec)/100;
	    }
	}
      printf("%d\t\t%d.%d\n", thr, (t1.tv_sec - t0.tv_sec), time);
      thr++;

      int print = 1; // print = 0 for output, 1 for no output
      char err[10];     
      if(check(print) == 0)
	{
	  strcpy(err, "No Errors");
	}
      else
	{
	  strcpy(err, "Error");
	}

      /* clear matrix */
      for(i = 0; i < N; i++)
	{
	  for(j = 0; j < P; j++)
	    {
	      C_thread[i][j] = 0;
	    }
	}
      //border and any errors
      printf("------------------------------\n");
      printf("Running %d threads: %s\n",thr -1, err); 
      printf("------------------------------\n\n");

      // terminate program if error occured
      if(strcmp("Error", err) == 0)
	{
	  return(1);
	}
    }
  printf("Program has exited normally.\n");
  return 0;
}

void *math(void* range)
{
  struct parm *prange = range;
  int start = prange->start;
  int finish = prange->finish;

  // perform multiplication on portion of matrix
  int i, j, k;
  for(i = start; i <= finish; i++)
    {
      for(j = 0; j < P; j++)
	{
	  for(k = 0; k < M; k++)
	    {
	      C_thread[i][j] += A[i][k] * B[k][j];
	    }
	}
    }


  // end thread
  pthread_exit(0);
}

