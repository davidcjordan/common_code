#ifndef vectors_points_h
#define vectors_points_h
// #include <stdbool.h>
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
void interp_float(float x1, float x2, float x3, float y1, float y2, float *y3);
void unit_vector_float(float ax, float ay, float az, float *bx, float *by, float *bz);

#endif