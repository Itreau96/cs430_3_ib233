#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ib_3dmath.h"
#include "raycast.h"
#include "parser.h"

// Forward declarations
void create_node(obj *data, linked_list *list);
void add_rgb(rgb *data, rgb_list *list);
void render(int *width, int *height, linked_list *objs, rgb_list *color_buff);
void sphere_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t);
void plane_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t);
void write_file(rgb_list *colors, int *width, int *height, char *file_name);
float clamp(float value, float min, float max);

// Object arrays
obj *objects;
obj *lights;

int main(int argc, char* argv[])
{
   // Variable declarations
   int width;
   int height;
   linked_list objs = { malloc(sizeof(obj_node)), malloc(sizeof(obj_node)), malloc(sizeof(obj_node)), 0 };
   rgb_list color_buff = { malloc(sizeof(rgb_node)), malloc(sizeof(rgb_node)), 0 };
   int run_result;

   // Relay error message if incorrect number of arguments were entered
   if (argc < MIN_ARGS)
   {
      // Subtract 1 from arguments entered, since 1st argument is the executable
      fprintf(stderr, "Error: Incorrect number of arguments. You entered %d. You must enter 4. (err no. %d)\n", argc - 1, INPUT_INVALID);

      // Return error code
      return RUN_FAIL;
   }
   // If correct number of arguments supplied, validate the remaining arguments
   else
   {
      // Store width and height
      width = atoi(argv[1]);
      height = atoi(argv[2]);

      // Start by parsing file input
      parse(&objs, argv[3], &run_result);

      // Raycast objects if parse was successful
      if (run_result == RUN_SUCCESS)
      {  
         // Calculate rgb values at each pixel
         render(&width, &height, &objs, &color_buff);
         // Write the output to the file
         write_file(&color_buff, &width, &height, argv[4]);
      }
      // If parse was unsuccessful, display error message and code
      else
      {
         fprintf(stderr, "Error: There was a problem parsing your input file. Please correct the file and try again. (err no. %d)\n", run_result);
      }
   }
   
   return RUN_SUCCESS;
}

// Used to render the scene given parsed objects
void render(int *width, int *height, linked_list *objs, rgb_list *color_buff)
{
   // Variable declarations
   int rows;
   int cols;
   ib_v3 rd;
   rgb cur_rgb = { 0, 0, 0 };
   float cam_width = objs->main_camera->obj_ref.width;
   float cam_height = objs->main_camera->obj_ref.height;
   double px_width = cam_width / *width;
   double px_height = cam_height / *height;
   ib_v3 r0 = { 0.0, 0.0, 0.0 }; // Initialize camera position
   float pz = -1; // Given distance from camera to viewport (negative z axis)
   float py;
   float px;
   float t = INFINITY;
   float cur_t = t;
   obj_node *cur_obj = objs->first;
   obj_node closest;
   int closest_index = 0;

   // Loop for as many columns in image
   for (cols = *height - 1; cols >= 0 ; cols-=1) // Start from top to bottom. Makes +y axis upward direction.
   {
      // Calculate py first
      py = CENTER_XY - cam_height  / 2.0 + px_height * (cols + 0.5);

      // Loop for as many rows as possible
      for (rows = 0; rows < *width; rows+=1)
      {
         // Calculate px
         px = CENTER_XY - cam_width / 2.0 + px_width * (rows + 0.5);

         // Combine variables into rd vector
         rd.x = px;
         rd.y = py;
         rd.z = pz;

         // Normalize the vector
         ib_v3_normalize(&rd);

         // Loop through each object and test for intersections
         for (int index = 0; index < objs->size; index+=1)
         {
            // Call intersection based on object type
            if (cur_obj->obj_ref.type == SPHERE)
            {
               sphere_intersection(&r0, &rd, &(cur_obj->obj_ref), &cur_t);

               // If current t is smaller than current smallest, set values
               if (cur_t < t)
               {
                  //cur_rgb = cur_obj->obj_ref.color;
                  t = cur_t;
                  closest = *cur_obj;
                  closest_index = index;
               }
            }
            else if (cur_obj->obj_ref.type == PLANE)
            {
               plane_intersection(&r0, &rd, &(cur_obj->obj_ref), &cur_t);

               // If current t is smaller than current smallest, set values
               if (cur_t < t && cur_t > 0)
               {
                  //cur_rgb = cur_obj->obj_ref.color;
                  t = cur_t;
                  closest = *cur_obj;
                  closest_index = index;
               }
            }

            // Move to next object
            cur_obj = cur_obj->next;
         }

         // Reset traverser
         cur_obj = objs->first;        

         // Determine if color data is necessary
         if (t != INFINITY && t > 0)
         {
            // Loop through lights in array
            for (int index = 0; index < objs->size; index+=1)
            {
               // Make sure it is a light object
               if (cur_obj->obj_ref.type == LIGHT)
               {
                  // Do test for shadows
                  ib_v3 ro;
                  ro.x = (t * rd.x) + r0.x;
                  ro.y = (t * rd.y) + r0.y;
                  ro.z = (t * rd.z) + r0.z;

                  ib_v3 rdn;
                  rdn.x = cur_obj->obj_ref.position.x - ro.x;
                  rdn.y = cur_obj->obj_ref.position.y - ro.y;
                  rdn.z = cur_obj->obj_ref.position.z - ro.z;

                  float dist;
                  ib_v3_len(&dist, &rdn);
                  ib_v3_normalize(&rdn);

                  float closest_shadow_dist = INFINITY;
                  bool shadow = FALSE;

                  obj_node *shadow_node = objs->first;
                  
                  // Check for shadows
                  for (int index2 = 0; index2 < objs->size; index2++)
                  {
                     if (closest_index == index2)
                     {
                        shadow_node = shadow_node->next;
                        continue;
                     }

                     // Check if intersection with plane
                     if (shadow_node->obj_ref.type == PLANE)
                     {
                        plane_intersection(&ro, &rdn, &(shadow_node->obj_ref), &closest_shadow_dist);
                     }
                     // Check if intersection with sphere
                     else if (shadow_node->obj_ref.type == PLANE)
                     {
                        sphere_intersection(&ro, &rdn, &(shadow_node->obj_ref), &closest_shadow_dist);
                     }
                     // Otherwise, skip iteration because not object
                     else
                     {
                        shadow_node = shadow_node->next;
                        continue;
                     }

                     if (closest_shadow_dist > dist)
                     {
                        shadow_node = shadow_node->next;
                        continue;
                     }

                     if (closest_shadow_dist < dist && closest_shadow_dist > 0.0)
                     {
                        shadow = TRUE;
                     }

                     // Traverse to next node
                     shadow_node = shadow_node->next;
                  }

                  if (shadow == FALSE)
                  {
                     // Determine N, L, R, and V light values
                     ib_v3 ni;
                     ib_v3 li;
                     ib_v3 ri;
                     ib_v3 vi;
                     rgb diff;
                     rgb spec;
                     
                     // Determine object type
                     if (closest.obj_ref.type == PLANE)
                     {
                        ni = closest.obj_ref.normal;
                     }
                     // Check if intersection with sphere
                     else if (closest.obj_ref.type == SPHERE)
                     {
                        ib_v3_sub(&ni, &ro, &(closest.obj_ref.position));
                        ib_v3_normalize(&ni);
                     }
                     
                     // Set the specular and diffuse colors
                     diff = closest.obj_ref.diffuse_color;
                     spec = closest.obj_ref.specular_color;
                     
                     // Set Li
                     li = rdn;
                     
                     float dot_val;
                     ib_v3_dot(&dot_val, &ni, &li);
                     ib_v3_scale(&ri, 2.0*dot_val, &ni);
                     ib_v3_sub(&ri, &ri, &li);
                     
                     ib_v3_scale(&vi, -1, &rd);
                     
                     
                     float frad = 1.0/(cur_obj->obj_ref.radial_a2*(dist*dist) + 
                     cur_obj->obj_ref.radial_a1*dist + 
                     cur_obj->obj_ref.radial_a0);

                     float fang;
                     ib_v3 vli = cur_obj->obj_ref.direction;
                     ib_v3_normalize(&vli);
                     
                     // Determine if point light
                     if(cur_obj->obj_ref.theta == 0 || cur_obj->obj_ref.angular_a0 == 0)
                     {
                        fang = 1.0;
                     }
                     // Otherwise, it is a spot light
                     else
                     {
                        float target = cos(cur_obj->obj_ref.theta * 3.14159265 / 180.0);
                        float cur_dot;
                        ib_v3_dot(&cur_dot, &rdn, &vli);
                     
                        if(target > cur_dot)
                        {
                          fang = 0.0;
                        }
                        else
                        {
                          fang = pow(cur_dot, cur_obj->obj_ref.angular_a0);
                        }
                     }
                     
                     ib_v3 diffuse_calc;
                     ib_v3 specular_calc;
                     float nl_dot = 0;
                     ib_v3_dot(&nl_dot, &ni, &li);
                     float vr_dot = 0;
                     ib_v3_dot(&vr_dot, &vi, &ri);
                     
                     if(nl_dot > 0)
                     {
                        diffuse_calc.x = cur_obj->obj_ref.color.r * diff.r;
                        diffuse_calc.y = cur_obj->obj_ref.color.g * diff.g;
                        diffuse_calc.z = cur_obj->obj_ref.color.b * diff.b;
        
                        ib_v3_scale(&diffuse_calc, nl_dot, &diffuse_calc);

                        if(vr_dot > 0)
                        {
                           specular_calc.x = cur_obj->obj_ref.color.r * spec.r;
                           specular_calc.y = cur_obj->obj_ref.color.g * spec.g;
                           specular_calc.z = cur_obj->obj_ref.color.b * spec.b;
                           
                           ib_v3_scale(&specular_calc, pow(vr_dot,20), &specular_calc);
                        }
                        else
                        {
                           specular_calc.x = 0;
                           specular_calc.y = 0;
                           specular_calc.z = 0;
                        }
                     }
                     
                    	cur_rgb.r += frad * fang * clamp(diffuse_calc.x + specular_calc.x, 0, 1);
                    	cur_rgb.g += frad * fang * clamp(diffuse_calc.y + specular_calc.y, 0, 1);
                    	cur_rgb.b += frad * fang * clamp(diffuse_calc.z + specular_calc.z, 0, 1);
                  }
               }
               
               // Traverse to next object
               cur_obj = cur_obj->next;
            }
         }

         // Scale color value
         cur_rgb.r = cur_rgb.r * 255;
         cur_rgb.g = cur_rgb.g * 255;
         cur_rgb.b = cur_rgb.b * 255;

         // Now append the color value to the buffer
         add_rgb(&cur_rgb, color_buff);
         
         // Clear rgb and t values
         cur_rgb.r = 0;
         cur_rgb.g = 0;
         cur_rgb.b = 0;
         t = INFINITY;
         cur_t = INFINITY;

         // Reset current object
         cur_obj = objs->first;
      }
   }
}

// Method used to find sphere intersection
void sphere_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t)
{
   // Variable declarations
   float a;
   float b;
   float c;
   float d;
   float t0;
   float t1;

   // Calculate a, b, and c values
   a = (rd->x * rd->x) + (rd->y * rd->y) + (rd->z * rd->z);
   b = 2 * (rd->x * (r0->x - cur_obj->position.x) + rd->y * (r0->y - cur_obj->position.y) + rd->z * (r0->z - cur_obj->position.z));
   c = ((r0->x - cur_obj->position.x) * (r0->x - cur_obj->position.x) + 
        (r0->y - cur_obj->position.y) * (r0->y - cur_obj->position.y) + 
        (r0->z - cur_obj->position.z) * (r0->z - cur_obj->position.z)) - (cur_obj->radius * cur_obj->radius);

   // Calculate descriminate value
   d = (b * b - 4 * a * c);

   // Only if descriminate is positive do we calculate intersection
   if (d > 0)
   {
      // Calculate both t values
      t0 = (-b + sqrtf(b * b - 4 * c * a)) / (2 * a);
      t1 = (-b - sqrtf(b * b - 4 * c * a)) / (2 * a);

      // Determine which t value to return
      if (t1 > 0)
      {
         *t = t1;
      }
      else if (t0 > 0)
      {
         *t = t0;
      }
   }
}

// Method used to find plane intersection
void plane_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t)
{
   // Variable declarations
   float a;
   float b;
   float c;
   float dist;
   float den;
   ib_v3 normal = cur_obj->normal;
   
   // Normalize normal value and assign a, b, and c (for readability)
   ib_v3_normalize(&normal);
   a = cur_obj->normal.x;
   b = cur_obj->normal.y;
   c = cur_obj->normal.z; 

   // Calculate dist and den values
   dist = -(a * cur_obj->position.x + b * cur_obj->position.y + c * cur_obj->position.z);
   den = (a * rd->x + b * rd->y + c * rd->z);

   // If den = 0, return faulty t value
   if (den == 0)
   {
      *t = -1;
   }
   // Otherwise, calculate and return t
   else
   {
      *t = -(a * r0->x + b * r0->y + c * r0->z + dist) / den;
   }
}

// Helper method used to write output to file
void write_file(rgb_list *colors, int *width, int *height, char *file_name)
{
   // Variable declarations
   FILE *out_file;
   int index = 0;
   rgb_node *cur_color = colors->first;

   // Start by opening file
   if ((out_file = fopen(file_name, "wb")) != NULL)
   {
      // Start by writing header file
      fprintf(out_file, "%s\n%d %d\n%d\n", "P6", *width, *height, 255);
      
      // Loop through each character and put character value
      for (index = 0; index < colors->size; index++)
      {
         // Using the %c format converts the integer value to its ascii equivalent
         fprintf(out_file, "%c%c%c", (char)cur_color->color.r, (char)cur_color->color.g, (char)cur_color->color.b);

         cur_color = cur_color->next;
      }

      // Close file
      fclose(out_file);
   }
}

// Helper function used to append a new node to the end of a linked list
void create_node(obj *data, linked_list *list)
{
   // First, determine if this is first node in linked list
   if (list->size == 0)
   {
      // Last and first are both the same
      list->first = malloc(sizeof(obj_node));
      list->last = list->first;

      // Allocate size for next object
      list->last->next = malloc(sizeof(obj_node));
      
      // Set node data
      list->first->obj_ref = *data;
      
      // Increase size
      list->size = 1;
   }
   // If not first, simply append object
   else
   {
      // Apply data to new last node
      list->last->next->obj_ref = *data;
      list->last->next->next = malloc(sizeof(obj_node));

      // Increase size of linked list
      list->last = list->last->next;
      list->size = list->size + 1;
   }
}

// Helper function used to append a new color to the color buffer list
void add_rgb(rgb *data, rgb_list *list)
{
   // First, determine if this is first node in linked list
   if (list->size == 0)
   {
      // Last and first are both the same
      list->first = malloc(sizeof(rgb_node));
      list->last = list->first;

      // Allocate size for next object
      list->last->next = malloc(sizeof(rgb_node));
      
      // Set node data
      list->first->color = *data;
      
      // Increase size
      list->size = 1;
   }
   // If not first, simply append object
   else
   {
      // Apply data to new last node
      list->last->next->color = *data;
      list->last->next->next = malloc(sizeof(rgb_node));

      // Increase size of linked list
      list->last = list->last->next;
      list->size = list->size + 1;
   }
}

// Helper method used to return a clamped value between min and max
float clamp(float value, float min, float max)
{
   // First determine if value is greater than max
   if (value > max)
   {
      // Return max
      return max;
   }
   // Determine if value is less than min
   else if (value < min)
   {
      // Return min
      return min;
   }
   
   // If this point is reached, value is within min and max
   return value;
}


