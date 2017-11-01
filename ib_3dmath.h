// Ensure program is only compiled once
#ifndef IB_3D
#define IB_3D

#include <stdio.h>
#include <math.h>

// Type declarations
typedef struct ib_v3 ib_v3;

// Struct implementations
struct ib_v3
{
   float x, y, z;
};

// Add function for 2 vectors a and b
static inline void ib_v3_add(ib_v3 *out, ib_v3 *a, ib_v3 *b)
{
   // Set x, y, and z of output vector
   out->x = a->x + b->x;
   out->y = a->y + b->y;
   out->z = a->z + b->z;
}

// Subtract function for 2 vectors a and b
static inline void ib_v3_sub(ib_v3 *out, ib_v3 *a, ib_v3 *b)
{
   // Set x, y, and z of output vector
   out->x = a->x - b->x;
   out->y = a->y - b->y;
   out->z = a->z - b->z;
}

// Function used to scale input vector by scale value
static inline void ib_v3_scale(ib_v3 *out, float scale)
{
   // Scale x, y, and z of output vector by scale value
   out->x = out->x * scale;
   out->y = out->y * scale;
   out->z = out->z * scale;
}

// Function used to calculate dot product of two vectors
static inline void ib_v3_dot(float *out, ib_v3 *a, ib_v3 *b)
{
   // Return dot product
   *out = (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

// Function used to calculate the cross product of two vectors
static inline void ib_v3_cross(ib_v3 *out, ib_v3 *a, ib_v3 *b)
{
   // Return cross between vectors a and b
   out->x = (a->y*b->z) - (a->z*b->y);
   out->y = (a->z*b->x) - (a->x*b->z);
   out->z = (a->x*b->y) - (a->y*b->x);
}

// Function used to normalize a given vector
static inline void ib_v3_normalize(ib_v3 *v)
{
   // Caculate length of vector and divide each dimmension by this value
   float length = sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
   v->x = v->x / length;
   v->y = v->y / length;
   v->z = v->z / length;
}

#endif
