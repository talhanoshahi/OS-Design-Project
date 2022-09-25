#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// funtions for selecting which algorithm to use
int select_algoritm ( );
bool algorithm_redirect ( const int algorithm_index , int * pages , int * frame , const int page_size, const int frame_size );

// algorithm functions
int fifo ( int* pages , int * frame , const int page_size, const int frame_size );
int lru ( int* pages , int * frame , const int page_size, const int frame_size ); // this algorithm is even faster than all of the algorithms that can be found on the internet
int lfu ( int* pages , int * frame , const int page_size, const int frame_size );
int optimal ( int* pages , int * frame , const int page_size, const int frame_size );

// random important functions
int find_page ( const int * frame , const int frame_size , const int page_to_find );
int occur_index ( const int * frame , const int start_index , const int end_index , const int page_to_find );
int min_occur ( const int * occur_array , const int array_size );
bool move_top ( int * frame , const int frame_size , const int move_from );
bool move_end ( int * frame , const int frame_size , const int move_from );
bool frame_init ( int * frame , const int frame_size , const int init_value );
bool move_up ( int * frame , const int frame_size , const int move_from );

int main ()
{
	int page_size = 0;
	int frame_size = 0;

	printf ( "Enter the page size: " );
	scanf ( "%d" , &page_size );

	printf ( "Enter the frame size: " );
	scanf ( "%d" , &frame_size );

	int pages[ page_size ] = {0};
	int frame[ frame_size ];
	frame_init ( frame , frame_size , -1 );

	printf ( "Enter the page data: \n" );
	for ( int i = 0 ; i < page_size ; i++ )
	{
		printf ( "\tpage[%d]: " , i );
		scanf ( "%d" , &pages[i] );
	}
	printf( "\n" );

	algorithm_redirect ( select_algoritm() , pages , frame , page_size , frame_size );

	return 0;
}

int select_algoritm ()
{
	int choice = 0;
	printf ( "Select your algorithm from the following:\n" );

	printf( "\t1. First In First Out \n" );
	printf( "\t2. Least Recently Used\n" );
	printf( "\t3. Least Frequently Used\n" );
	printf( "\t4. Optimal\n" );

	printf( "\n\tChoice: " );
	scanf ( "%d" , &choice );

	printf( "\n" );
	return choice;
}

bool algorithm_redirect ( const int algorithm_index , int * pages , int * frame , const int page_size, const int frame_size )
{
	switch ( algorithm_index )
	{
		case 1:
			fifo ( pages , frame , page_size , frame_size );
			break;
		case 2:
			lru ( pages , frame , page_size , frame_size );
			break;
		case 3:
			lfu ( pages , frame , page_size , frame_size );
			break;
		case 4:
			optimal ( pages , frame , page_size , frame_size );
			break;
		default:
			fprintf ( stderr , "No matching algoritm\n" );
			return false;
			break;
	}

	return true;

}

int fifo ( int* pages , int * frame , const int page_size, const int frame_size )
{
	int hits = 0;
	int misses = 0;

	for ( int i = 0 , frame_index = 0 ; i < page_size ; i = i + 1 )
	{
		if ( frame_index == frame_size )
			frame_index = 0;

		if ( pages[i] == frame[ frame_index ] )
		{
			hits = hits + 1;
			continue;
		}

		if ( find_page ( frame , frame_size , pages[i] ) >= 0 )
		{
			hits = hits + 1;
			continue;
		}

		misses = misses + 1;
		if ( misses < frame_size )
		{
			frame[ misses - 1 ] = pages[i];
			frame_index = frame_index + 1;
			continue;
		}
		frame[frame_index] = pages[i];
		frame_index = frame_index + 1;
		continue;
	}

	printf ( "The number of page faults using First In First Out algorithm is: %d\n" , misses );
	printf ( "The number of page hits using First In First Out algorithm is: %d\n" , hits );
	printf ( "The percentage of page faults using First In First Out algorithm is: %f\n" , ( ( ( (float) misses ) / ( ( float ) page_size ) ) * 100 ) );

	return misses;
}

int lru ( int* pages , int * frame , const int page_size, const int frame_size )
{
	int hits = 0;
	int misses = 0;

	for ( int i = 0 , frame_index = 0 ; i < page_size ; i = i + 1 )
	{
		if ( frame_index == frame_size )
			frame_index = 0;

		if ( pages[i] == frame[ frame_index ] )
		{
			hits = hits + 1;
			move_top ( frame , frame_size , frame_index );
			continue;
		}

		int found = find_page ( frame , frame_size , pages[i] );
		if ( found >= 0 )
		{
			hits = hits + 1;
			move_top ( frame , frame_size , found );
			continue;
		}

		misses = misses + 1;
		if ( misses < frame_size )
		{
			frame[ misses -1 ] = pages[i];
			move_top( frame, frame_size, ( misses -1 ));
			frame_index = frame_index + 1;
			continue;
		}

		move_top ( frame , frame_size , ( frame_size - 1 ) );
		frame[0] = pages[i];
		frame_index = frame_index + 1;
		continue;
	}

	printf ( "The number of page faults using Least Recently Used algorithm is: %d\n" , misses );
	printf ( "The number of page hits using Least Recently Used algorithm is: %d\n" , hits );
	printf ( "The percentage of page faults using Least Recently Used algorithm is: %f\n" , ( ( ( (float) misses ) / ( ( float ) page_size ) ) * 100 ) );

	return misses;
}

int lfu ( int* pages , int * frame , const int page_size, const int frame_size )
{
	int hits = 0;
	int misses = 0;

	int most_used[ frame_size ];
	frame_init ( most_used , frame_size , 0 );

	for ( int i = 0 , frame_index = 0 ; i < page_size ; i = i + 1 )
	{
		if ( frame_index == frame_size )
			frame_index = 0;

		if ( pages[i] == frame[ frame_index ] )
		{
			hits = hits + 1;
			most_used[ frame_index ] = most_used[ frame_index ] + 1;
			if ( frame_index != 0 )
			{
				for ( int j = frame_index ; most_used[ j ] > most_used [ j - 1 ] && j > 0 ; j++ )
				{
					move_up ( most_used , frame_size , j );
					move_up ( frame , frame_size , j );
				}
			}

			continue;
		}

		int found = find_page ( frame , frame_size , pages[i] );
		if ( found >= 0 )
		{
			hits = hits + 1;
			most_used[ found ] = most_used[ found ] + 1;
			if ( found != 0 )
			{
				for ( int j = found ; most_used[ j ] > most_used [ j - 1 ] && j > 0 ; j++ )
				{
					move_up ( most_used , frame_size , j );
					move_up ( frame , frame_size , j );
				}
			}
			continue;
		}

		misses = misses + 1;
		if ( misses < frame_size )
		{
			frame[ misses -1 ] = pages[i];
			most_used[ misses -1 ] = 1;
			most_used[ found ] = most_used[ found ] + 1;
			if ( found != 0 )
			{
				for ( int j = found ; most_used[ j ] > most_used [ j - 1 ] && j > 0 ; j++ )
				{
					move_up ( most_used , frame_size , j );
					move_up ( frame , frame_size , j );
				}
			}
			continue;
		}

		frame[ frame_size -1 ] = pages[i];
		most_used[ frame_size -1 ] = 1;
		most_used[ found ] = most_used[ found ] + 1;
		if ( found != 0 )
		{
			for ( int j = found ; most_used[ j ] > most_used [ j - 1 ] && j > 0 ; j++ )
			{
				move_up ( most_used , frame_size , j );
				move_up ( frame , frame_size , j );
			}
		}
		frame_index = frame_index + 1;
		continue;
	}

	printf ( "The number of page faults using Least Frequently Used algorithm is: %d\n" , misses );
	printf ( "The number of page hits using Least Frequently Used algorithm is: %d\n" , hits );
	printf ( "The percentage of page faults using Least Frequently Used algorithm is: %f\n" , ( ( ( (float) misses ) / ( ( float ) page_size ) ) * 100 ) );

	return misses;
}

int optimal ( int* pages , int * frame , const int page_size, const int frame_size )
{
	int hits = 0;
	int misses = 0;
	int change_index = 0;

	int future_occurance[ frame_size ];
	frame_init ( future_occurance , frame_size , 0 );

	for ( int i = 0 , frame_index = 0 ; i < page_size ; i = i + 1 )
	{
		if ( frame_index == frame_size )
			frame_index = 0;

		if ( pages[i] == frame[ frame_index ] )
		{
			hits = hits + 1;
			continue;
		}

		int found = find_page ( frame , frame_size , pages[i] );
		if ( found >= 0 )
		{
			hits = hits + 1;
			continue;
		}

		misses = misses + 1;
		if ( misses < frame_size )
		{
			frame[ misses -1 ] = pages[i];
			continue;
		}

		for ( int j = 0 ; j < frame_size ; j++ )
			future_occurance[ j ] = occur_index ( pages , i , ( page_size - 1 ) , frame[ j ] );

		change_index = min_occur ( future_occurance , frame_size );

		frame[ change_index ] = pages[i];
		frame_index = frame_index + 1;
		continue;
	}

	printf ( "The number of page faults using Optimal algorithm is: %d\n" , misses );
	printf ( "The number of page hits using Optimal algorithm is: %d\n" , hits );
	printf ( "The percentage of page faults using Optimal algorithm is: %f\n" , ( ( ( (float) misses ) / ( ( float ) page_size ) ) * 100 ) );

	exit ( EXIT_SUCCESS );

	return misses;
}


int find_page ( const int * frame , const int frame_size , const int page_to_find )
{
	for ( int i = 0 ; i < frame_size ; i = i + 1 )
		if ( frame[i] == page_to_find )
			return i;

	return -1;
}

bool move_top ( int * frame , const int frame_size , const int move_from )
{
	if ( move_from >= frame_size || move_from < 1 )
		return false;

	int temp;
	for ( int i = move_from ; i > 0 ; i = i - 1 )
	{
		temp = frame[i];
		frame[i] = frame[ i - 1 ];
		frame [ i - 1 ] = temp;
	}

	return true;
}

bool move_end ( int * frame , const int frame_size , const int move_from )
{
	if ( move_from >= frame_size || move_from < 0 )
		return false;

	int temp;
	for ( int i = move_from ; i < frame_size -1 ; i = i + 1 )
	{
		temp = frame[i];
		frame[i] = frame[ i + 1 ];
		frame [ i + 1 ] = temp;
	}

	return true;
}

bool move_up ( int * frame , const int frame_size , const int move_from )
{
	if ( move_from >= frame_size || move_from < 1 )
		return false;

	int temp;
	temp = frame[ move_from ];
	frame[ move_from ] = frame[ move_from - 1 ];
	frame [ move_from - 1 ] = temp;

	return true;
}

bool frame_init ( int * frame , const int frame_size , const int init_value )
{
	for ( int i = 0 ; i < frame_size ; i++ )
		frame[ frame_size ] = init_value;

	return true;
}

int occur_index ( const int * frame , const int start_index , const int end_index , const int page_to_find )
{
	int occured = 0;
	for ( int i = start_index ; i < end_index ; i = i + 1 )
		if ( frame[i] == page_to_find )
			return i;

	return -1;
}

int min_occur ( const int * occur_array , const int array_size )
{
	int min = 0;

	for ( int i = 0 ; i < array_size ; i++ )
	{
		if ( i == 0 )
		{
			min  = occur_array[ i ];
			continue;
		}

		if ( min < occur_array[ i ] )
			min = occur_array[ i ];

	}

	return min;
}

