#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
#include <time.h> //time

#define RANGE 10000

typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point)
{
	printf("#points\n");
	for (int i = 0; i < num_point; i++)
		printf("points(%d,%d)\n", points[i].x, points[i].y);
}

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line)
{
	printf("#line segments\n");
	for (int i = 0; i < num_line; i++)
		printf("segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
}

// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of line segments that forms the convex hull
// return value : set of line segments that forms the convex hull
t_line *convex_hull( t_point *points, int num_point, int *num_line)
{
	*num_line = 0;
	int capacity = 10;
	t_line* lines = (t_line*)malloc(sizeof(t_line) * capacity);

	for (int i = 0; i < num_point - 1; i++) {
		t_point p1 = points[i];

		for (int j = i+1; j < num_point; j++) {
			t_point p2 = points[j];
			int a = p2.y - p1.y;
			int b = p1.x - p2.x;
			int c = p1.x * p2.y - p1.y * p2.x;

			int is_convex = 1; // 0 == false, 1 == true
			int line;

			for (int k = 0; k < num_point; k++) {
				if (k == i || k == j)
					continue;

				line = a * points[k].x + b * points[k].y - c;
				if (line)
					break;
			}

			if (line > 0) {
				for (int k = 0; k < num_point; k++) {
					if (a * points[k].x + b * points[k].y < c) {
						is_convex = 0;
						break;
					}
				}
			}
			else if (line < 0) {
				for (int k = 0; k < num_point; k++) {
					if (a * points[k].x + b * points[k].y > c) {
						is_convex = 0;
						break;
					}
				}
			}

			if (is_convex == 1) {
				if (*num_line == capacity) {
					capacity += 10;
					lines = (t_line*)realloc(lines, sizeof(t_line) * capacity);
					fprintf( stderr, "realloc! (%d)\n", capacity);
				}

				lines[*num_line].from = p1;
				lines[(*num_line)++].to = p2;
			}
		}
	}

	return lines;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line; // number of lines
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
		
	t_line *lines;

	// making n points
	srand( time(NULL));
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		points[i].x = x;
		points[i].y = y;
 	}

	fprintf( stderr, "%d points created!\n", num_point);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	lines = convex_hull( points, num_point, &num_line);

	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
	
	print_footer();
	
	free( points);
	free( lines);
	
	return 0;
}

