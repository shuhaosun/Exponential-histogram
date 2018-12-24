#include <stdio.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <string.h>
#include <ctime>
#include <time.h>
#include <iterator>
#include <math.h>
#include <vector>
#include <ctype.h>
#include <math.h>

using namespace std;

#define K 4
#define N_INSERT 6000000
#define WINDOW_SIZE 65536
#define N1 6554               // Maximum number of 1 in the window            
#define QUERY_TIME 10000
#define M 49                  // Specify the number of buckets allocated

typedef struct Node
{
	int exponent;
	int end;
	int start;
}Bucket;

double aae_sum = 0;   // The sum of absolute error
double are_sum = 0;   // The sum of relative error

int point = 0;
int total = 0;

double aae_edge = 0;

int compare(const void *a, const void *b)
{
    int *pa = (int*)a;
    int *pb = (int*)b;
    return (*pa )- (*pb);  
}
void Get_truecount(int first, int last, double *true_count, int *data_stream)
{
    int i = 0;
    int j;
    double in_window = 0;
    while(i < N_INSERT - 1200000)
    {
        if (i < WINDOW_SIZE)
        {
        	if (data_stream[i] == -1)
		    {
		        in_window++;
		        true_count[i] = in_window;
		    }
		    else
		    {
		    	true_count[i] = in_window;
		    }
		    i++;
        }
        else
        {
        	j = i - WINDOW_SIZE + 1;

	    	if (data_stream[first + j - 1] == -1 && data_stream[last + j] != -1)
	        {
	            in_window--;
	            true_count[i] = in_window;
	        }
	        else if (data_stream[last + j] == -1 && data_stream[first + j - 1] != -1)
	        {
	            in_window++;
	            true_count[i] = in_window;
	        }
	        else
	        {
	        	true_count[i] = in_window;
	        }
	        i++;        	
        }
    }	
}
void Get_query_time(int *query_time)
{
//	srand(time(NULL));
	int flag = 0;
	for (int i = 0; i < QUERY_TIME; ++i)
	{
		query_time[i] = rand() % (N_INSERT - 1200000);

		if (query_time[i] < 600000 || query_time[i] > 1000000)
		{
			flag = 1;
		}
		else
		{
			for (int j = 0; j < i; ++j)
			{
				if (query_time[i] == query_time[j])
				{
					flag = 1;
					break;
				}
			}
		}
		if (flag == 1)
		{
			i--;
		}
		flag = 0;
	}
	qsort(query_time, QUERY_TIME, sizeof(int), compare);
}
void Expire_bucket(int t, Bucket *bucket)
{
	if (bucket[0].end <= (t - WINDOW_SIZE) && bucket[0].end != 0)
	{
		total = total - bucket[0].exponent;
		for (int i = 0; i < point - 1; ++i)
		{
			bucket[i].exponent = bucket[i + 1].exponent;
			bucket[i].start = bucket[i + 1].start;
			bucket[i].end = bucket[i + 1].end;
		}
		bucket[point - 1].exponent = 0;
		bucket[point - 1].end = 0;
		bucket[point - 1].start = 0;
		point--;
	}
}
void query(int end_edge, int p, double *true_count, double *aae, double *are, Bucket *bucket, int m)
{
	double count;
	double Last;

	if (total == 1 || bucket[0].exponent == 1)
	{
		count = true_count[end_edge];
	}
	else
	{
		Last = 1.0/2.0 * (double)bucket[0].exponent;

		aae_edge = aae_edge + Last;
		count = total - Last;		
	}
	aae[p] = fabs(true_count[end_edge] - count);
	aae_sum = aae_sum + aae[p];
	if (true_count[end_edge] == 0)
	{
		are[p] = aae[p];
	}
	else
	{
		are[p] = fabs(true_count[end_edge] - count) / true_count[end_edge];
	}
	are_sum = are_sum + are[p];
}

int main()
{ 
	char * filename_FlowTraffic = "DNS1.txt";
	Bucket *bucket;
	int *data_stream;
	double *true_count;
	char insert1[3000];
	string measure_flow = "www.baidu.com";

	int *query_time;
	double avg_aae_edge; // Average upper bound of absolute error
	double *aae;
	double *are;
	double avg_aae;     // Average absolute error
	double avg_are;     // Average relative error 


	int m = (K/2 + 1)*(log2(2*N1/K) + 2);
//	m = M;
	printf("\nThe number of buckets : %d\n", m);


	bucket = new Bucket[m];
	memset(bucket, 0, sizeof(int) * m);
	data_stream = new int[N_INSERT];
	memset(data_stream, 0, sizeof(int) * N_INSERT);
	true_count = new double[N_INSERT];
	memset(true_count, 0, sizeof(double) * N_INSERT);
	query_time = new int[QUERY_TIME];
	memset(query_time, 0, sizeof(int) * QUERY_TIME);
	are = new double[QUERY_TIME];
	memset(are, 0, sizeof(double) * QUERY_TIME);
	aae = new double[QUERY_TIME];
	memset(aae, 0, sizeof(double) * QUERY_TIME);



 	FILE *file_FlowTraffic = fopen(filename_FlowTraffic, "r");

    int i = 0;
    while(fgets(insert1, 3000, file_FlowTraffic) != NULL)
    {
        string jsonString = insert1;
        if(i >= 1200000 && jsonString.find(measure_flow) < jsonString.length())
		{
		    data_stream[i - 1200000] = -1;
		}  

		i++;

        if(i == N_INSERT)
            break;
    }
    fclose(file_FlowTraffic);


/*	filename_FlowTraffic = "DNS2.txt";

 	file_FlowTraffic = fopen(filename_FlowTraffic, "r");


    while(fgets(insert1, 3000, file_FlowTraffic) != NULL)
    {
        string jsonString = insert1;
        if(i >= 1200000 && jsonString.find(measure_flow) < jsonString.length())
		{
		    data_stream[i - 1200000] = -1;
		}  

		i++;

        if(i == N_INSERT)
            break;
    }
    fclose(file_FlowTraffic);
*/

    int first = 0;
	int last = WINDOW_SIZE - 1;

    Get_truecount(first, last, true_count, data_stream);
   
    Get_query_time(query_time);
 

	int flag = 0;
	int insert = 0;
	int count;
	int num = 0;
	int insert_k = 0;
	int insert_index;

	int query_time_p = 0;
	for (int i = 0; i < N_INSERT - 1200000; ++i)
	{
		Expire_bucket(i + 1, bucket);
		if (data_stream[i] == -1)
		{
			bucket[point].exponent = 1;
			bucket[point].start = i + 1;
			bucket[point].end = i + 1;
			point++;
			total++;			
		}
		int j = point - 1;
		while(j >= 0)
		{
			count = bucket[j].exponent;
			num++;
			j--;
			while(j >= 0 && count == bucket[j].exponent)
			{
				num++;
				j--;
			}
			if (count == 1 && num > K + 1)
			{
				int merge_index = j + 1;
				bucket[merge_index].exponent = bucket[merge_index].exponent + bucket[merge_index + 1].exponent;
				bucket[merge_index].end = bucket[merge_index + 1].end;
				bucket[merge_index + 2].start = bucket[merge_index].end;
				for (int z = merge_index + 1; z < point - 1; ++z)
				{
					bucket[z].exponent = bucket[z + 1].exponent;
					bucket[z].end = bucket[z + 1].end;
					bucket[z].start = bucket[z + 1].start;
				}
				bucket[point - 1].exponent = 0;
				bucket[point - 1].end = 0;
				bucket[point - 1].start = 0;
				point--;
				j++;								
			}
			else if (count != 1 && num > K/2.0 + 1)
			{
				int merge_index = j + 1;
				bucket[merge_index].exponent = bucket[merge_index].exponent + bucket[merge_index + 1].exponent;
				bucket[merge_index].end = bucket[merge_index + 1].end;
				bucket[merge_index + 2].start = bucket[merge_index].end;
				for (int z = merge_index + 1; z < point - 1; ++z)
				{
					bucket[z].exponent = bucket[z + 1].exponent;
					bucket[z].end = bucket[z + 1].end;
					bucket[z].start = bucket[z + 1].start;
				}
				bucket[point - 1].exponent = 0;
				bucket[point - 1].end = 0;
				bucket[point - 1].start = 0;
				point--;
				j++;
			}
			else
			{
				break;
			}
			num = 0;
		}
		num = 0;
		if (query_time[query_time_p] == i)
		{
			query(i, query_time_p, true_count, aae, are, bucket, m);
			if (query_time_p + 1 < QUERY_TIME)
			{
				query_time_p++;
			}
		}
	}
	avg_aae = aae_sum / (QUERY_TIME * 1.0);
	avg_are = are_sum / (QUERY_TIME * 1.0);
	avg_aae_edge = aae_edge / (QUERY_TIME * 1.0);

	printf("query times = : %d\n\n", QUERY_TIME);

	printf("The sum of absolute error = : %f\n", aae_sum);
	printf("The sum of relative error = : %f\n\n", are_sum);
	printf("Average upper bound of absolute error = : %f\n", avg_aae_edge);	 
	printf("Average absolute error = : %f\n", avg_aae);
	printf("Average relative error = : %f\n", avg_are);

	delete []data_stream;
	delete []bucket;
	delete []true_count;
	delete []query_time;
	delete []are;
	delete []aae;

	return 0;
}
