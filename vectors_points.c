// vector_points.c: vector_add, rotate, mult, etc
#include <math.h>
#include "vectors_points.h"

double vector_length(double ax, double ay, double az)
{ // start of sub
	return (sqrt(ax * ax + ay * ay + az * az));
} // end of sub

void points_to_vector(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz)
// vector starts at point "a" and ends at point "b"
{ // start of sub
	*cx = bx - ax;
	*cy = by - ay;
	*cz = bz - az;
} // end of sub

void vector_add(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz)
// vector c = vector a + vector b
{ // start of sub
	*cx = ax + bx;
	*cy = ay + by;
	*cz = az + bz;
} // end of sub


double vector_angle(double ax, double ay, double az, double bx, double by, double bz)
{
	// angle between 2 vectors: cos(angle) = (dot product of a and b) / ((length of a) x (length of b))
	// So angle = arccos( (dot product of a and b) / ((length of a) * (length of b)))
	double dot_a_b; // dot product of a and b
	double length_a, length_b; // length of vectors a and b
	double temp; // before arccosin(temp)
	double rad_deg = 57.29577951; // used to convert from radians to degrees
	double diff_x, diff_y, diff_z; // difference between unit vectors A and B, only used when close to zero angle
	double uax, uay, uaz, ubx, uby, ubz; // unit vectors A and B, only used when close to zero angle
	double length_diff; // length of difference between tips of unit vectors A and B
	double angle; // angle between vectors

	dot_a_b = dot_product(ax, ay, az, bx, by, bz);
	length_a = vector_length(ax, ay, az);
	length_b = vector_length(bx, by, bz);
	if (length_a == 0) return(0);
	if (length_b == 0) return(0);
	temp = dot_a_b / (length_a*length_b);
	if (temp > .9999) { // too close to discontinuity so use atan() function instead
		unit_vector(ax, ay, az, &uax, &uay, &uaz); // turn vector A into a unit vector
		unit_vector(bx, by, bz, &ubx, &uby, &ubz); // turn vector B into a unit vector
		diff_x = uax - ubx; diff_y = uay - uby; diff_z = uaz - ubz; // get difference between tips of vectors
		length_diff = vector_length(diff_x, diff_y, diff_z);
		angle = atan(length_diff); // tan(angle) = length_diff / 1.0
	} else angle = acos(temp);
	angle = angle * rad_deg;
	return(angle);
}

void vector_rotate(double vx, double vy, double vz, double kkx, double kky, double kkz, double angle, double *vrx, double *vry, double *vrz)
{ // start of sub
	// Use Rodrigues formula to rotate a vector "V" around a unit vector "K" by "angle" radians
	// V(rot) = Vcos(angle) + (K X V)sin(angle) + K(K.V)(1-cos(angle))
	// first change vector "kk" into a unit vector "K"
	double kx, ky, kz;							 // unit vector "k" derived from normalizing vector "kk"
	double vkv_x, vkv_y, vkv_z;				 // cross product vector derived from KXV
	double dot_kv;									 // dot product of K V
	double vcos_x, vcos_y, vcos_z;			 // vector v * cos(angle)
	double vkvsin_x, vkvsin_y, vkvsin_z;	 // vector vkv * sin(angle)
	double kdotcos_x, kdotcos_y, kdotcos_z; // vector k * (dot product of K and V) * (1 - cos(angle))
	double ax, ay, az;							 // intermediate calc. vector
	double bx, by, bz;							 // 2nd intermediate calc. vector

	unit_vector(kkx, kky, kkz, &kx, &ky, &kz);							// convert vector "kk" to unit vector "k"
	cross_product(kx, ky, kz, vx, vy, vz, &vkv_x, &vkv_y, &vkv_z); // vector "vkv" is cross product of vectors "k" and "v"
	dot_kv = dot_product(kx, ky, kz, vx, vy, vz);
	vector_mult(vx, vy, vz, cos(angle), &vcos_x, &vcos_y, &vcos_z);					 // multiply vector "v" by cos(angle) to get vector "vcos"
	vector_mult(vkv_x, vkv_y, vkv_z, sin(angle), &vkvsin_x, &vkvsin_y, &vkvsin_z); // multiply vector "vkv" by sin(angle) to get vector "vkvsin"
	vector_mult(kx, ky, kz, dot_kv * (1 - cos(angle)), &kdotcos_x, &kdotcos_y, &kdotcos_z);
	vector_add(vcos_x, vcos_y, vcos_z, vkvsin_x, vkvsin_y, vkvsin_z, &ax, &ay, &az); // add v*cos(angle) to vkv*sin(angle) store result in vector "a"
	vector_add(ax, ay, az, kdotcos_x, kdotcos_y, kdotcos_z, &bx, &by, &bz);
	*vrx = bx; // store into rotated vector and exit
	*vry = by;
	*vrz = bz;
} // end of sub

void unit_vector(double ax, double ay, double az, double *bx, double *by, double *bz)
{						// start of sub
	double length; // length of original vector "a"
	length = vector_length(ax, ay, az);
	*bx = ax / length;
	*by = ay / length;
	*bz = az / length;
} // end of sub

void vector_mult(double ax, double ay, double az, double scalar, double *bx, double *by, double *bz)
{							 // start of sub
	*bx = ax * scalar; // multiply all components by scalar and return in vector "b"
	*by = ay * scalar;
	*bz = az * scalar;
} // end of sub

// cross product subroutine
// cross_product(bx,ay,az,bx,by,bz,&cx,&cy,&cz)
// c = a X b (cross product of 2 3-D vectors) c is a vector perpendicular to vectors a and b
void cross_product(double ax, double ay, double az, double bx, double by, double bz, double *cx, double *cy, double *cz)
{ // start of sub
	*cx = ay * bz - az * by;
	*cy = az * bx - ax * bz;
	*cz = ax * by - ay * bx;
} // end of sub

double dot_product(double ax, double ay, double az, double bx, double by, double bz)
{													  // start of sub
	return (ax * bx + ay * by + az * bz); // scalar dot product
} // end of sub


double distance_between_points(double ax, double ay, double az, double bx, double by, double bz)
{
	return (sqrt((ax - bx) * (ax - bx) + (ay - by) * (ay - by) + (az - bz) * (az - bz)));
}

// This subroutine doesn't work if the lines are vertical (slope = infinity)
void get_intersection(double ax, double ay, double bx, double by, double cx, double cy,
							 double dx, double dy, double *ix, double *iy)
{
	double m1; // slope of line AB
	double m2; // slope of line CD
	double b1; // constant for line AB
	double b2; // constant for line CD

	if ((ax - bx) > -.01 && (ax - bx) < .01)
	{
		// too close to divide by zero, return 0
		*ix = *iy = 0;
		return;
	}
	if ((cx - dx) > -.01 && (cx - dx) < .01)
	{
		// too close to divide by zero, return 0
		*ix = *iy = 0;
		return;
	}
	m1 = (ay - by) / (ax - bx);
	m2 = (cy - dy) / (cx - dx);
	b1 = ay - m1 * ax;
	b2 = cy - m2 * cx;
	*ix = (b2 - b1) / (m1 - m2);
	*iy = *ix * m1 + b1;
}

// This subroutine doesn't work if the lines are vertical (slope = infinity)
void get_intersection_line(line_t a, line_t b, point_t *intersection)
{
  float line_a_slope= (a.end[0]->axis[Y] - a.end[1]->axis[Y]) / (a.end[0]->axis[X] - a.end[1]->axis[X]);
  float line_b_slope= (b.end[0]->axis[Y] - b.end[1]->axis[Y]) / (b.end[0]->axis[X] - b.end[1]->axis[X]);
  float line_a_constant= a.end[0]->axis[Y] - line_a_slope * a.end[0]->axis[X];
  float line_b_constant= b.end[0]->axis[Y] - line_b_slope * b.end[0]->axis[X];
  intersection->axis[X]= (line_b_constant - line_a_constant) / (line_a_slope - line_b_slope);
  intersection->axis[Y]= intersection->axis[X] * line_a_slope + line_a_constant;
}

void interp_float(float x1, float x2, float x3, float y1, float y2, float *y3){
  *y3 = y1 + (x3 - x1)*(y2 - y1) / (x2 - x1);
}

void unit_vector_float(float ax, float ay, float az, float *bx, float *by, float *bz){
  float length = sqrt(ax*ax + ay*ay + az*az);
  *bx = ax / length;
  *by = ay / length;
  *bz = az / length;
}
