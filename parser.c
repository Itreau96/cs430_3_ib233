#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ib_3dmath.h"
#include "raycast.h"
#include "parser.h"

// Forward declarations
void store_obj_properties(obj *cur_obj, FILE *file, char *c, int *result);
void get_next_word(char *word, char delim, int max_len, FILE *file, char *c);
void get_camera(obj *cur_obj, FILE *file, char *c, int *result);
void get_sphere(obj *cur_obj, FILE *file, char *c, int *result);
void get_plane(obj *cur_obj, FILE *file, char *c, int *result);
void get_light(obj *cur_obj, FILE *file, char *c, int *result);

// Method used to parse out objects in the input file
void parse(linked_list *list, char *file_name, int *result)
{
   // Variable declarations
   FILE *file;
   char c;
   obj cur_obj;

   // Start by attemping to open the file
   file = fopen(file_name, "r");

   // Check if file exists
   if (file)
   {
      // Read file character by character until file ends
      while ((c = fgetc(file)) != EOF)
      {
         // Store object properties
         store_obj_properties(&cur_obj, file, &c, result);

         // Confirm that store operation worked correctly
         if (*result == RUN_SUCCESS)
         {
            // Create node and append to list
            create_node(&cur_obj, list);

            // If the current node is a camera, set the main camera value
            if (cur_obj.type == CAMERA)
            {
               list->main_camera = list->last;
            }
         }

         // If not, execution halts
      }

      // Close file
      fclose(file);
   }
   // If not, return error code
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store object properties 
void store_obj_properties(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char obj_type[TYPE_LEN];

   // Start by retrieving object type
   get_next_word(obj_type, VALUE_SEP, TYPE_LEN, file, c);

   // Store data based on object type
   if (strcmp(obj_type, "camera") == 0)
   {
      // Store object type
      cur_obj->type = CAMERA;

      // Store object properties
      get_camera(cur_obj, file, c, result);
   }
   else if (strcmp(obj_type, "sphere") == 0)
   {
      // Store object type
      cur_obj->type = SPHERE;

      // Store object variables
      get_sphere(cur_obj, file, c, result);
   }
   else if (strcmp(obj_type, "plane") == 0)
   {
      // Store object type
      cur_obj->type = PLANE;

      // Store object variables
      get_plane(cur_obj, file, c, result);
   }
   else if (strcmp(obj_type, "light") == 0)
   {
      // Store object type
      cur_obj->type = LIGHT;

      // Store object variables
      get_light(cur_obj, file, c, result);
   }
   else
   {
      *result = INPUT_INVALID;
   }   
}

// Helper method used to store camera object variables
void get_camera(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];
   bool width_found = FALSE;
   bool height_found = FALSE;
   int prop_count = 0;

   // Store object type
   cur_obj->type = CAMERA;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "width") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->width = atof(value);

         // Set boolean value
         width_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else if (strcmp(property, "height") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->height = atof(value);

         // Set boolean value
         height_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if height or width was not found
   if (width_found == TRUE && height_found == TRUE && *result != INPUT_INVALID && prop_count <= CAM_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store sphere object variables
void get_sphere(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];

   bool r_found = FALSE;
   bool g_found = FALSE;
   bool b_found = FALSE;

   bool x_found = FALSE;
   bool y_found = FALSE;
   bool z_found = FALSE;

   bool sr_found = FALSE;
   bool sg_found = FALSE;
   bool sb_found = FALSE;

   bool dr_found = FALSE;
   bool dg_found = FALSE;
   bool db_found = FALSE;

   bool radius_found = FALSE;
   int prop_count = 0;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the r color value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure r is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.r = atof(value);

               // Set boolean value
               r_found = TRUE;
            }
         }

         // Get the g color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure g is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.g = atof(value);

               // Set boolean value
               g_found = TRUE;
            }
         }

         // Get the b color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Make sure b is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.b = atof(value);

               // Set boolean value
               b_found = TRUE;
            }
         }

         // Make sure if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "position") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.x = atof(value);

            // Set boolean value
            x_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.y = atof(value);

            // Set boolean value
            y_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.z = atof(value);

            // Set boolean value
            z_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "diffuse_color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the dr value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.r = atof(value);

            // Set boolean value
            dr_found = TRUE;
         }

         // Get the dg value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.g = atof(value);

            // Set boolean value
            dg_found = TRUE;
         }

         // Get the db value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.b = atof(value);

            // Set boolean value
            db_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "specular_color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the sr value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.r = atof(value);

            // Set boolean value
            sr_found = TRUE;
         }

         // Get the sg value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.g = atof(value);

            // Set boolean value
            sg_found = TRUE;
         }

         // Get the sb value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.b = atof(value);

            // Set boolean value
            sb_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "radius") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->radius = atof(value);

         // Make sure radius is between 0 and 1
         if (atof(value) >= 0)
         {
            // Set boolean value
            radius_found = TRUE;
         }

         // Increment the prop count
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if required values are not found
   if (r_found == TRUE && g_found == TRUE && b_found == TRUE &&
       x_found == TRUE && y_found == TRUE && z_found == TRUE &&
       dr_found == TRUE && dg_found == TRUE && db_found == TRUE &&
       sr_found == TRUE && sg_found == TRUE && sb_found == TRUE &&
       radius_found == TRUE && *result != INPUT_INVALID && prop_count <= SPHERE_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store plane object variables
void get_plane(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];

   bool r_found = FALSE;
   bool g_found = FALSE;
   bool b_found = FALSE;

   bool x_found = FALSE;
   bool y_found = FALSE;
   bool z_found = FALSE;

   bool n1_found = FALSE;
   bool n2_found = FALSE;
   bool n3_found = FALSE;

   bool sr_found = FALSE;
   bool sg_found = FALSE;
   bool sb_found = FALSE;

   bool dr_found = FALSE;
   bool dg_found = FALSE;
   bool db_found = FALSE;

   int prop_count = 0;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the r color value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure r is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.r = atof(value);

               // Set boolean value
               r_found = TRUE;
            }
         }

         // Get the g color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure g is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.g = atof(value);

               // Set boolean value
               g_found = TRUE;
            }
         }

         // Get the b color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Make sure b is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.b = atof(value);

               // Set boolean value
               b_found = TRUE;
            }
         }

         // Make sure if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "position") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.x = atof(value);

            // Set boolean value
            x_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.y = atof(value);

            // Set boolean value
            y_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.z = atof(value);

            // Set boolean value
            z_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "diffuse_color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the dr value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.r = atof(value);

            // Set boolean value
            dr_found = TRUE;
         }

         // Get the dg value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.g = atof(value);

            // Set boolean value
            dg_found = TRUE;
         }

         // Get the db value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->diffuse_color.b = atof(value);

            // Set boolean value
            db_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "specular_color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the sr value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.r = atof(value);

            // Set boolean value
            sr_found = TRUE;
         }

         // Get the sg value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.g = atof(value);

            // Set boolean value
            sg_found = TRUE;
         }

         // Get the sb value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->specular_color.b = atof(value);

            // Set boolean value
            sb_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "normal") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the normal x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.x = atof(value);

            // Set boolean value
            n1_found = TRUE;
         }

         // Get the normal y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.y = atof(value);

            // Set boolean value
            n2_found = TRUE;
         }

         // Get the normal z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.z = atof(value);

            // Set boolean value
            n3_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if required values are not found
   if (r_found == TRUE && g_found == TRUE && b_found == TRUE &&
       x_found == TRUE && y_found == TRUE && z_found == TRUE &&
       n1_found == TRUE && n2_found == TRUE && n3_found == TRUE &&
       dr_found == TRUE && dg_found == TRUE && db_found == TRUE &&
       sr_found == TRUE && sg_found == TRUE && sb_found == TRUE &&
       *result != INPUT_INVALID && prop_count <= SPHERE_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}


// Helper method used to store light object variables
void get_light(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];

   bool r_found = FALSE;
   bool g_found = FALSE;
   bool b_found = FALSE;

   bool x_found = FALSE;
   bool y_found = FALSE;
   bool z_found = FALSE;

   bool dx_found = FALSE;
   bool dy_found = FALSE;
   bool dz_found = FALSE;

   bool theta_found = FALSE;
   bool radial_a0_found = FALSE;
   bool radial_a1_found = FALSE;
   bool radial_a2_found = FALSE;
   bool angular_a0_found = FALSE;

   int prop_count = 0;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the r color value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure r is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.r = atof(value);

               // Set boolean value
               r_found = TRUE;
            }
         }

         // Get the g color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Make sure g is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.g = atof(value);

               // Set boolean value
               g_found = TRUE;
            }
         }

         // Get the b color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Make sure b is between 0 and 1
            if (atof(value) <= 1 && atof(value) >= 0)
            {
               // Set property value
               cur_obj->color.b = atof(value);

               // Set boolean value
               b_found = TRUE;
            }
         }

         // Make sure if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "position") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.x = atof(value);

            // Set boolean value
            x_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.y = atof(value);

            // Set boolean value
            y_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.z = atof(value);

            // Set boolean value
            z_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "direction") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->direction.x = atof(value);

            // Set boolean value
            dx_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->direction.y = atof(value);

            // Set boolean value
            dy_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->direction.z = atof(value);

            // Set boolean value
            dz_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "theta") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->theta = atof(value);

         // Set boolean value
         theta_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else if (strcmp(property, "radial-a0") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Determine if valid value
         if (atof(value) >= 0 && atof(value) <= 1)
         {
            // Set property value
            cur_obj->radial_a0 = atof(value);

            // Set boolean value
            radial_a0_found = TRUE;

            // Increment the prop count
            prop_count++;
         }
      }
      else if (strcmp(property, "radial-a1") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Determine if valid value
         if (atof(value) >= 0 && atof(value) <= 1)
         {
            // Set property value
            cur_obj->radial_a1 = atof(value);

            // Set boolean value
            radial_a1_found = TRUE;

            // Increment the prop count
            prop_count++;
         }
      }
      else if (strcmp(property, "radial-a2") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Determine if valid value
         if (atof(value) >= 0 && atof(value) <= 1)
         {
            // Set property value
            cur_obj->radial_a2 = atof(value);

            // Set boolean value
            radial_a2_found = TRUE;

            // Increment the prop count
            prop_count++;
         }
      }
      else if (strcmp(property, "angular-a0") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Determine if valid value
         if (atof(value) >= 0 && atof(value) <= 1)
         {
            // Set property value
            cur_obj->angular_a0 = atof(value);

            // Set boolean value
            angular_a0_found = TRUE;

            // Increment the prop count
            prop_count++;
         }
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if required values are not found
   if (r_found == TRUE && g_found == TRUE && b_found == TRUE &&
       x_found == TRUE && y_found == TRUE && z_found == TRUE &&
       radial_a0_found == TRUE && radial_a1_found == TRUE && radial_a2_found == TRUE &&
       ((((theta_found == FALSE) || (theta_found == TRUE && cur_obj->theta == 0)) &&
       angular_a0_found == FALSE && dx_found == FALSE && dy_found == FALSE && dz_found == FALSE) ||
       ((theta_found == TRUE && cur_obj->theta != 0) && angular_a0_found == TRUE && 
       dx_found == TRUE && dy_found == TRUE && dz_found == TRUE)) &&
       *result != INPUT_INVALID && prop_count <= LIGHT_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to retrieve word from file
void get_next_word(char *word, char delim, int max_len, FILE *file, char *c)
{
   // Variable declarations
   int index = 0;

   // Start by stripping white-space from front of word
   while (isspace(*c))
   {
      *c = fgetc(file);
   }

   // While deliminator is not reached, continue searching for word
   while (*c != delim && index < max_len && *c != LINE_TERM && *c != EOF)
   {
      word[index] = *c;
      *c = fgetc(file);
      index++;
   }

   // If character is end of file, return error code
   if (*c != EOF && *c != LINE_TERM)
   {
      *c = fgetc(file);
   }

   // Set ending character
   word[index] = STR_END;
}


