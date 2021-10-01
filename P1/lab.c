/**
 * Name: Jane Doe
 * Email: janedoe@u.boisestate.edu
 * Github Username: jdoe
 */
#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>

#define STRUCTSIZE 8
int thread_Count = 0;

struct parallel_mergesort_args
{
	int *B; //4 bytes
	int P;	//2 bytes
	int R;
	int threads; //2 bytes
};

//struct for pthread arguments

/*
 * insertion_sort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void insertion_sort(int A[], int p, int r)
{
	int j;

	for (j = p + 1; j <= r; j++)
	{
		int key = A[j];
		int i = j - 1;
		while ((i > p - 1) && (A[i] > key))
		{
			A[i + 1] = A[i];
			i--;
		}
		A[i + 1] = key;
	}
}

/*
 * serial_mergesort(int A[], int p, int r):
 *
 * description: Sort the section of the array A[p..r].
 */
void serial_mergesort(int A[], int p, int r)
{
	//INSERTION_SORT_THRESHOLD = 100
	if (r - p + 1 <= INSERTION_SORT_THRESHOLD)
	{
		insertion_sort(A, p, r);
	}
	else
	{
		int q = r + (p - r) / 2;
		serial_mergesort(A, p, q);
		serial_mergesort(A, q + 1, r);
		merge(A, p, q, r);
	}
}

/*
 * merge(int A[], int p, int q, int r):
 *
 * description: Merge two sorted sequences A[p..q] and A[q+1..r]
 *              and place merged output back in array A. Uses extra
 *              space proportional to A[p..r].
 */

void *parallel_mergesort(void *args)
{	

	// casting args to parallel_mergesort_args type
	struct parallel_mergesort_args parallel_thread_Arguments[2];
	struct parallel_mergesort_args *in = (struct parallel_mergesort_args *)args;

	//get the arguments from the struct to be parsed compared and processed.
	int *A = in->B;
	int p = in->P;
	int r = in->R;
	int numthreads = in->threads;

	//thread holds upto 2 threads
	pthread_t thread[2];

	//its a multi-threaded program
	//The list needs to be divided based on the number of threads.

	//if there is only one thread than we can run it like any other program.
	//also works as a base ccase.
	if (numthreads == 1 || numthreads <= thread_Count)
	{
		serial_mergesort(A, p, r);
	}
	else
	{
		//args for thread1 >> recursion
		parallel_thread_Arguments[0].B = A;
		parallel_thread_Arguments[0].P = p;
		parallel_thread_Arguments[0].R = (p + r) / 2;
		parallel_thread_Arguments[0].threads = numthreads;

		//args for thread2 >> recursion
		parallel_thread_Arguments[1].B = A;
		parallel_thread_Arguments[1].P = ((p + r) / 2) + 1;
		parallel_thread_Arguments[1].R = r;
		parallel_thread_Arguments[1].threads = numthreads;

		//increment the threadcount
		thread_Count = thread_Count + 2;

		// Creates the thread.
		pthread_create(&thread[0], NULL, parallel_mergesort, &parallel_thread_Arguments[0]);
		pthread_create(&thread[1], NULL, parallel_mergesort, &parallel_thread_Arguments[1]);

		//join the thread
		pthread_join(thread[0], NULL);
		pthread_join(thread[1], NULL);

		//merges the list.
		merge(A, p, (p + r) / 2, r);
	}
	return NULL;
}

void merge(int A[], int p, int q, int r)
{
	int *B = (int *)malloc(sizeof(int) * (r - p + 1));

	int i = p;
	int j = q + 1;
	int k = 0;
	int l;

	// as long as both lists have unexamined elements
	// this loop keeps executing.
	while ((i <= q) && (j <= r))
	{
		if (A[i] < A[j])
		{
			B[k] = A[i];
			i++;
		}
		else
		{
			B[k] = A[j];
			j++;
		}
		k++;
	}

	// now only at most one list has unprocessed elements.

	if (i <= q)
	{
		// copy remaining elements from the first list
		for (l = i; l <= q; l++)
		{
			B[k] = A[l];
			k++;
		}
	}
	else
	{
		// copy remaining elements from the second list
		for (l = j; l <= r; l++)
		{
			B[k] = A[l];
			k++;
		}
	}

	// copy merged output from array B back to array A
	k = 0;
	for (l = p; l <= r; l++)
	{
		A[l] = B[k];
		k++;
	}

	free(B);
}
