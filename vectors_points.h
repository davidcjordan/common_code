#ifndef vectors_points_h
#define vectors_points_h
// #include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include "defines.h" //point and line structs

void cross_product(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz);
double dot_product(double ax, double ay, double az, double bx, double by, double bz);
void points_to_vector(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz);
void vector_add(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz);
void vector_mult(double ax, double ay, double az, double scalar, double *bx, double *by, double *bz);
void vector_rotate(double vx, double vy, double vz, double kkx, double kky, double kkz, double angle, double *vrx, double *vry, double *vrz);
double vector_length(double ax, double ay, double az);
double vector_angle(double ax, double ay, double az, double bx, double by, double bz);
void unit_vector(double ax, double ay, double az, double *bx, double *by, double *bz);

double distance_between_points(double ax, double ay, double az, double bx, double by, double bz);

void get_intersection(double ax, double ay, double bx, double by, double cx, double cy,
							 double dx, double dy, double *ix, double *iy);

// This subroutine doesn't work if the lines are vertical (slope = infinity)
void get_intersection_line(line_t a, line_t b, point_t *intersection);

float interpolate_y_from_x(float x1, float x2, float x3, float y1, float y2);
//NOTE: the order of parameters is different than from interpolate_y_from_x
float interpolate_x_from_y(float m1, float a1, float m2, float a2, float a3);

void unit_vector_float(float ax, float ay, float az, float *bx, float *by, float *bz);

float best_fit_sum_of_squares(float x[],float y[],int elements,float y_goal);
//maps the input range proportionally onto the output range
float map(float input,float in_low, float in_high, float out_low, float out_high);

#ifndef fequal
#  define fequal(f1,f2)  ((fabs(f1-f2)) < (.005) ? 1 : 0)
#endif

int16_t range_rand(int16_t low, int16_t hi);

#endif