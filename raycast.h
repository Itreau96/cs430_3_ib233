#ifndef RAYCAST
#define RAYCAST

#include "ib_3dmath.h"

#define RUN_SUCCESS 0
#define RUN_FAIL 1
#define MIN_ARGS 5
#define TRUE 1
#define FALSE 0

#define CAMERA 0
#define SPHERE 1
#define PLANE 2
#define LIGHT 3

#define CENTER_XY 0.0

// Type definitions
typedef int bool;
typedef struct rgb rgb;
typedef struct obj obj;
typedef struct obj_node obj_node;
typedef struct rgb_node rgb_node;
typedef struct linked_list linked_list;
typedef struct rgb_list rgb_list;

// Color in rgb format
struct rgb
{
   float r;
   float g;
   float b;
};

// Create one central object that contains all variables for any object
struct obj
{
   int type;
   float width;
   float height;
   rgb color;
   ib_v3 position;
   float radius;
   ib_v3 normal;
   float radial_a0;
   float radial_a1;
   float radial_a2;
   float theta;
   float angular_a0;
   ib_v3 direction;
   rgb diffuse_color;
   rgb specular_color;
   float ns;
};

// Struct used used as a linked list node
struct obj_node
{
   obj_node *next;
   obj obj_ref;
};

// Struct used as a node for an rgb value
struct rgb_node
{
   rgb_node *next;
   rgb color;
};

// Linked list structure
struct linked_list
{
   obj_node *first;
   obj_node *last;
   obj_node *main_camera;
   int size;
};

// Rgb list structure
struct rgb_list
{
   rgb_node *first;
   rgb_node *last;
   int size;
};

// Forward declarations
void create_node(obj *data, linked_list *list);

#endif
