
#ifndef __ENTITIES_H__
#define __ENTITIES_H__
    #include <stdlib.h>
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>


    #define DEBUG_PRINT 0
    #define PRINTF if(DEBUG_PRINT)printf
    // Define error_id if it's not already defined
    #ifndef error_id
        typedef int error_id;

        #define ENT_ERR_OK -1
        #define ENT_ALLOC_ERROR -2
        #define ENT_NULL_INPUT -3
        #define ENT_SHAPE_NOT_FOUND -4
    #endif

    typedef struct shape_s shape_t;
    typedef struct scene_s scene_t;


    typedef struct projectile_data_s projectile_data_t;
    typedef struct ship_data_s ship_data_t;


    typedef union shape_data_u shape_date_u;



    typedef struct projectile_data_s {
        /*owned    */  SDL_Point speed;        // 8 bytes
        /*owned    */  int mass;               // 4 bytes
        /*borrowed */  SDL_Point previous_pos; // 8 bytes
    } projectile_data_t;

    typedef struct ship_data_s {
        /*owned    */  SDL_Point speed;                  // 8 bytes
        /*owned    */  int mass;                         // 4 bytes
        /*owned    */  int life;                         // 4 bytes
        /*borrowed */  SDL_Point previous_pos;           // 8 bytes
        /*borrowed */  long int previous_firing_time;    // 8 bytes
        /*borrowed */  long int firing_frequency;        // 8 bytes
        /*owned    */  SDL_Point nominal_impulse_thrust; // 8 bytes
    } ship_data_t;

    typedef union shape_data_u{
        projectile_data_t projectile_props;
        ship_data_t ship_props;
        char char_array[256];
        unsigned int int_array[64];
        unsigned long long_array[32];
    } shape_data_t;

    // Function pointer types
    typedef void (*physics_fn)(scene_t* scene, shape_t* shape, SDL_Renderer* renderer);
    typedef void (*drawing_fn)(scene_t* scene, shape_t* shape, SDL_Renderer* renderer);
    typedef void (*collision_fn)(scene_t* scene, shape_t* self, shape_t* other, SDL_Renderer* renderer);

    typedef enum shape_type_e{
        BACKGROUND=1,
        SHIP=2,
        PROJECTILE=3
    } shape_type_enum;
    // Structure definitions
    typedef struct shape_s {
        /*owned  */ shape_type_enum type;
        /*owned  */ SDL_Point anchor;
        /*owned  */ int is_dead;
        /*borowed*/ SDL_Texture* tex;
        /*owned  */ SDL_Rect sprite_source;
        /*borowed*/ physics_fn physics;
        /*borowed*/ drawing_fn draw;
        /*borowed*/ collision_fn collision;
    //  /*owned  */ SDL_Point* contour;
        /*owned  */ shape_data_t memory;
    } shape_t;

    typedef struct scene_s {
        /*owned  */ shape_t** shapes; // null terminated array
        /*owned  */ unsigned long shape_count;
        /*owned  */ char keys_state[SDL_NUM_SCANCODES];
        /*owned  */ SDL_Point mouse_position; // never touch the reference, just update fields
        /*owned  */ long int time_start;
        /*owned  */ long int time_physics;
        /*owned  */ long int time_draw;
        /*owned  */ long int time_collisions;
        /*owned  */ long int time_cleanup;
        /*owned  */ long int time_end;
    } scene_t;



    // Function prototypes
    shape_t* shape__alloc();
    error_id shape__init(shape_t* shp, physics_fn physics, drawing_fn draw);
    SDL_Texture * shape__static__init_texture(SDL_Renderer* rend,const char * filename);
    error_id shape__init_sprite_with_texture(shape_t* shp, SDL_Renderer* rend,physics_fn physics, SDL_Texture * tex, SDL_Rect *sprite_source);
    error_id shape__init_sprite(shape_t* shp, SDL_Renderer* rend,physics_fn physics, const char * filename, SDL_Rect *sprite_source);
    void shape__draw(scene_t* scene,shape_t* shp, SDL_Renderer* renderer);
    void shape__physics(shape_t* shp, scene_t* scene, SDL_Renderer* renderer);
    int shape__free(shape_t* shp);
    SDL_Rect shape__get_bounding_box(shape_t* shp);
    int shape__collides_with(shape_t* a, shape_t* b);

    void shape__static__draw_sprite(scene_t* scene, shape_t* shape, SDL_Renderer* renderer);



    scene_t* scene__alloc();
    unsigned long scene__shape_count(scene_t* scene);
    error_id scene__add_shape(scene_t* scene, shape_t* shp);
    error_id scene__remove_shape(scene_t* scene, shape_t* shp);
    void scene__draw(scene_t* scene, SDL_Renderer* renderer);
    void scene__physics(scene_t* scene, SDL_Renderer* renderer);
    void scene__execute_collisions(scene_t* scene,SDL_Renderer *renderer);
    void scene__clear_dead_shapes(scene_t* scene);
    error_id scene__free(scene_t* scene);

    // Implementations
    shape_t* shape__alloc() {
        shape_t* shp=(shape_t*)malloc(sizeof(shape_t));
        // for(char k=0;k<256;k++){
        //     shp->memory[k]=0;
        // }
        return shp;
    }



    error_id shape__init(shape_t* shp, physics_fn physics, drawing_fn draw) {
        if (!shp || !physics || !draw ){
            PRINTF("// Invalid arguments %d\n",0);
            return -1; // Invalid arguments
        }
        (*shp).anchor.x =  0;
        (*shp).anchor.y =  0;
        PRINTF("// Initialize anchor to NULL\n");
        shp->is_dead = 0; // Initialize is_dead to 0
        PRINTF("// Initialize is_dead to 0\n");
        shp->physics = physics;
        shp->draw = draw;

        /// // Calculate the size of the contour array
        /// PRINTF("// Calculate the size of the contour array\n");
        /// unsigned long contour_size = sizeof(shp->contour)/sizeof(SDL_Point);
        /// PRINTF("// The size of the contour array is %lu\n",contour_size);

        /// // Allocate memory for contour
        /// PRINTF("// Allocate memory for contour\n");
        /// shp->contour = malloc((contour_size + 1) * sizeof(SDL_Point)); // +1 for the terminating (-1, -1)
        /// if (!shp->contour)
        ///     return -1; // Memory allocation failed

        /// // Copy contour
        /// PRINTF("// Copy contour\n");
        /// for (size_t i = 0; i < contour_size; i++)
        ///     shp->contour[i] = contour[i];

        // Mark end of contour with (-1, -1)
        PRINTF("// Mark end of contour with (-1, -1)\n");
        /// shp->contour[contour_size].x = -1;
        /// shp->contour[contour_size].y = -1;
        PRINTF("// Setup end of contour (-1, -1)\n");

        return 0; // Success
    }

    SDL_Texture * shape__static__init_texture(SDL_Renderer* rend,const char * filename) {

        SDL_Surface* surface = IMG_Load(filename);
        if (!surface)
        {
            return NULL;
        }

        // load the image data into the graphics hardware's memory
        SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
        SDL_FreeSurface(surface);
        if (!tex)
        {
            PRINTF("error initializing the texture %s",filename);
            return NULL;
        }

        return tex;
    }

    error_id shape__init_sprite_with_texture(shape_t* shp, SDL_Renderer* rend,physics_fn physics, SDL_Texture * tex, SDL_Rect *sprite_source) {
        if (!shp || !physics ){
            PRINTF("// Invalid arguments\n");
            return -1; // Invalid arguments
        }
        (*shp).anchor.x =  0;
        (*shp).anchor.y =  0;
        PRINTF("// Initialize anchor to NULL\n");
        shp->is_dead = 0; // Initialize is_dead to 0
        PRINTF("// Initialize is_dead to 0\n");
        // load the image into memory using SDL_image library function
        // SDL_Surface* surface = IMG_Load("resources/hello.png");
        shp->draw=&shape__static__draw_sprite;
        shp->physics=physics;
        // struct to hold the position and size of the sprite
        shp->sprite_source.x=(*sprite_source).x;
        shp->sprite_source.y=(*sprite_source).y;
        shp->sprite_source.w=(*sprite_source).w;
        shp->sprite_source.h=(*sprite_source).h;
        shp->tex=tex;
        PRINTF("initialized sprite entity with texture %lu at %d %d measuring %d %d\n\n",(unsigned long)shp->tex,shp->sprite_source.x,shp->sprite_source.y,shp->sprite_source.w,shp->sprite_source.h);
        return 0;
    }

    error_id shape__init_sprite(shape_t* shp, SDL_Renderer* rend,physics_fn physics, const char * filename, SDL_Rect *sprite_source) {
        if (!shp || !physics ){
            PRINTF("// Invalid arguments\n");
            return -1; // Invalid arguments
        }
        (*shp).anchor.x =  0;
        (*shp).anchor.y =  0;
        PRINTF("// Initialize anchor to NULL\n");
        shp->is_dead = 0; // Initialize is_dead to 0
        PRINTF("// Initialize is_dead to 0\n");
        // load the image into memory using SDL_image library function
        // SDL_Surface* surface = IMG_Load("resources/hello.png");
        shp->draw=&shape__static__draw_sprite;
        shp->physics=physics;

        SDL_Surface* surface = IMG_Load(filename);
        if (!surface)
        {
            return -1;
        }

        // load the image data into the graphics hardware's memory
        SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surface);
        SDL_FreeSurface(surface);
        if (!tex)
        {
            PRINTF("error initializing the texture %s",filename);
            return -2;
        }

        // struct to hold the position and size of the sprite
        shp->sprite_source.x=(*sprite_source).x;
        shp->sprite_source.y=(*sprite_source).y;
        shp->sprite_source.w=(*sprite_source).w;
        shp->sprite_source.h=(*sprite_source).h;
        shp->tex=tex;
        PRINTF("initialized sprite entity with texture %lu at %d %d measuring %d %d\n\n",(unsigned long)shp->tex,shp->sprite_source.x,shp->sprite_source.y,shp->sprite_source.w,shp->sprite_source.h);
        return 0;
    }

    void shape__draw(scene_t* scene,shape_t* shp, SDL_Renderer* renderer) {
        PRINTF("start drawing : shape_t* %lu %lu, scene_t* %lu %lu,SDL_Renderer* %lu %lu\n",(unsigned long)shp,sizeof(*shp),(unsigned long)scene,sizeof(*scene),(unsigned long)renderer,sizeof(renderer));
        
        if (shp == NULL || renderer == NULL)
            return; // Invalid shape or renderer, or shape is dead
        //PRINTF("// Valid shape and scene, and shape is not dead");
        PRINTF("executing draw fn pointer %lu\n",(unsigned long)(shp->draw));
        // return;
        (*shp).draw(scene,shp, renderer);
        PRINTF("// finished drawing");
    }

    void shape__physics(shape_t* shp, scene_t* scene, SDL_Renderer* renderer) {
        //PRINTF("// starting physics for shape is_dead:%d\n",s.is_dead);
        if (!shp || !scene || shp->is_dead)
            return;
        // Invalid shape or scene, or shape is dead
        //PRINTF("// Valid shape and scene, and shape is not dead");
        
        (*shp).physics(scene, shp,renderer);
        //PRINTF("// finished physics");
    }

    int shape__free(shape_t* shp) {
        if (!shp)
            return -1; // Invalid shape
        
        free(shp);

        return ENT_ERR_OK; // Success
    }

    void shape__static__draw_sprite(scene_t* scene, shape_t* shape, SDL_Renderer* renderer){
        // draw the image to the window
        SDL_Rect source={shape->sprite_source.x,shape->sprite_source.y,shape->sprite_source.w,shape->sprite_source.h};
        SDL_Rect dest={shape->anchor.x,shape->anchor.y,shape->sprite_source.w,shape->sprite_source.h};
        PRINTF("\rdrawing sprite %lu at %d %d %d %d",(unsigned long)shape->tex,shape->anchor.x,shape->anchor.y,shape->sprite_source.w,shape->sprite_source.h);
        SDL_RenderCopy(renderer, (*shape).tex, &source, &dest);
    }

    SDL_Rect shape__get_bounding_box(shape_t* shp) {
        SDL_Rect rect = {0, 0, 0, 0};

        if (!shp || shp->is_dead)
            return rect; // Invalid shape or shape is dead
        
        return (shp->sprite_source);
    }

    int shape__collides_with(shape_t* a, shape_t* b) {
        if (!a || !b || a->is_dead || b->is_dead)
            return 0; // Invalid shapes or any of them is dead
        
        SDL_Rect ra = shape__get_bounding_box(a);
        SDL_Rect rb = shape__get_bounding_box(b);
        SDL_Rect rc;

        SDL_IntersectRect(&ra,&rb,&rc);

        return SDL_RectEmpty(&rc);

        /// return ENT_ERR_OK; // Return 1 if collides, 0 otherwise
    }


scene_t* scene__alloc() {
    scene_t* scene = malloc(sizeof(scene_t));
    if (scene) {
        scene->shapes = malloc(sizeof(shape_t*)); // Allocate memory for the shapes array
        if (scene->shapes){
            scene->shapes[0] = NULL; // Initialize the first element to NULL

            for(int i=0;i<SDL_NUM_SCANCODES;i++){
                (*scene).keys_state[i]=0;
            }
            scene->shape_count=0;
            scene->mouse_position.x=0;
            scene->mouse_position.y=0;
        }else {
            free(scene); // Free scene if memory allocation failed for shapes
            return NULL;
        }
    }
    return scene;
}

unsigned long scene__shape_count(scene_t* scene) {
    if (!scene || !scene->shapes)
        return 0; // Invalid scene or shapes array
    
    unsigned long count = 0;
    while (scene->shapes[count] != NULL)
        count++;
    
    return count;
}

error_id scene__add_shape(scene_t* scene, shape_t* shp) {
    if (!scene || !shp)
        return -1; // Invalid scene or shape
    
    // Calculate the number of shapes in the scene
    unsigned long count = scene__shape_count(scene);
    PRINTF("scene count before adding shape : %lu\n",count);
    scene->shape_count=count;

    // Reallocate memory for shapes array to accommodate the new shape
    shape_t** new_shapes = realloc(scene->shapes, (count + 2) * sizeof(shape_t*)); // +1 for the new shape, +1 for the NULL terminator
    if (!new_shapes)
        return -1; // Memory reallocation failed

    // Update scene's shapes array with the new memory address
    scene->shapes = new_shapes;

    // Add the new shape to the end of the shapes array
    scene->shapes[count] = shp;
    scene->shapes[count + 1] = NULL; // Null-terminate the array
    

    count = scene__shape_count(scene);
    PRINTF("scene count after adding shape : %lu\n",count);
    scene->shape_count=count;

    return 0; // Success
}

error_id scene__remove_shape(scene_t* scene, shape_t* shp) {
    if (!scene || !shp)
        return -1; // Invalid scene or shape
    
    unsigned long count = scene__shape_count(scene);
    scene->shape_count=count;
    unsigned long i;
    for (i = 0; i < count; i++) {
        if (scene->shapes[i] == shp) {
            // Found the shape, now remove it by shifting the array
            while (i < count) {
                scene->shapes[i] = scene->shapes[i + 1];
                i++;
            }
            scene->shape_count=count-1;
            return 0; // Success
        }
    }

    return -1; // Shape not found
}

void scene__draw(scene_t* scene, SDL_Renderer* renderer) {
    PRINTF("start drawing : scene_t* %lu %lu,SDL_Renderer* %lu %lu\n",(unsigned long)scene,sizeof(*scene),(unsigned long)renderer,sizeof(renderer));
    if (!scene || !renderer)
        return; // Invalid scene or renderer
    
    PRINTF("counting shapes\n");
    unsigned long count = scene__shape_count(scene);
    PRINTF("found %lu shapes\n",count);
    for (unsigned long i = 0; i < count; i++){
        PRINTF("drawing shape %lu\n",i);
        shape__draw(scene, (*scene).shapes[i], renderer);
    }
}

void scene__physics(scene_t* scene, SDL_Renderer* renderer) {
    if (!scene)
        return; // Invalid scene
    
    unsigned long count = scene__shape_count(scene);
    for (unsigned long i = 0; i < count; i++)
        shape__physics(scene->shapes[i], scene,renderer);
}

    void scene__execute_collisions(scene_t* scene,SDL_Renderer *renderer) {
        if (!scene)
            return; // Invalid scene
        
        unsigned long count = scene->shape_count;
        for (unsigned long i = 0; i < count; i++) {
            for (unsigned long j = i + 1; j < count; j++) {
                if (shape__collides_with(scene->shapes[i], scene->shapes[j])) {
                    // Handle collision
                    // ...
                    if(scene->shapes[i]->collision!=NULL)scene->shapes[i]->collision(scene,scene->shapes[i],scene->shapes[j],renderer);
                    if(scene->shapes[j]->collision!=NULL)scene->shapes[j]->collision(scene,scene->shapes[j],scene->shapes[i],renderer);
                }
            }
        }
    }

    void scene__clear_dead_shapes(scene_t* scene) {
        if (!scene)
            return; // Invalid scene
        
        unsigned long count = scene->shape_count;
        for (unsigned long i = 0; i < count; i++) {
            if (scene->shapes[i]->is_dead) {
                shape__free(scene->shapes[i]);
                scene->shapes[i] = NULL;
            }
        }
    }

    error_id scene__free(scene_t* scene) {
        if (!scene)
            return -1; // Invalid scene

        //free(scene->mouse_position);
        
        // Free each shape
        PRINTF("// Free each shape\n");
        unsigned long count = scene->shape_count;
        for (unsigned long i = 0; i < count; i++)
            shape__free(scene->shapes[i]);
        
        // Free scene itself
        PRINTF("// Free scene itself\n");
        free(scene);

        return ENT_ERR_OK; // Success
    }


#endif