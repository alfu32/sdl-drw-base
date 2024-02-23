
#ifndef __ENTITIES_H__
#define __ENTITIES_H__
    #include <stdlib.h>
    #include <SDL2/SDL.h>
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

    // Function pointer types
    typedef void (*physics_fn)(scene_t* scene, shape_t* shape, SDL_Renderer* renderer);
    typedef void (*drawing_fn)(scene_t* scene, shape_t* shape, SDL_Renderer* renderer);

    // Structure definitions
    typedef struct shape_s {
        SDL_Point anchor;
        int is_dead;
        physics_fn physics;
        drawing_fn draw;
        SDL_Point* contour;
    } shape_t;

    typedef struct scene_s {
        shape_t** shapes; // null terminated array
        unsigned long shape_count;
        char keys_state[SDL_NUM_SCANCODES];
        SDL_Point mouse_position; // never touch the reference, just update fields
        long int time_start;
        long int time_physics;
        long int time_draw;
        long int time_collisions;
        long int time_cleanup;
    } scene_t;

    // Function prototypes
    shape_t* shape__alloc();
    error_id shape__init(shape_t* shp, physics_fn physics, drawing_fn draw, const SDL_Point contour[]);
    void shape__draw(scene_t* scene,shape_t* shp, SDL_Renderer* renderer);
    void shape__physics(shape_t* shp, scene_t* scene, SDL_Renderer* renderer);
    int shape__free(shape_t* shp);
    SDL_Rect shape__get_bounding_box(shape_t* shp);
    int shape__collides_with(shape_t* a, shape_t* b);

    scene_t* scene__alloc();
    unsigned long scene__shape_count(scene_t* scene);
    error_id scene__add_shape(scene_t* scene, shape_t* shp);
    error_id scene__remove_shape(scene_t* scene, shape_t* shp);
    void scene__draw(scene_t* scene, SDL_Renderer* renderer);
    void scene__physics(scene_t* scene, SDL_Renderer* renderer);
    void scene__execute_collisions(scene_t* scene);
    void scene__clear_dead_shapes(scene_t* scene);
    error_id scene__free(scene_t* scene);

    // Implementations
    shape_t* shape__alloc() {
        return (shape_t*)malloc(sizeof(shape_t));
    }

    error_id shape__init(shape_t* shp, physics_fn physics, drawing_fn draw, const SDL_Point contour[]) {
        if (!shp || !physics || !draw || !contour){
            printf("// Invalid arguments\n");
            return -1; // Invalid arguments
        }
        (*shp).anchor.x =  0;
        (*shp).anchor.y =  0;
        printf("// Initialize anchor to NULL\n");
        shp->is_dead = 0; // Initialize is_dead to 0
        printf("// Initialize is_dead to 0\n");
        shp->physics = physics;
        shp->draw = draw;

        // Calculate the size of the contour array
        printf("// Calculate the size of the contour array\n");
        unsigned long contour_size = sizeof(shp->contour)/sizeof(SDL_Point);
        printf("// The size of the contour array is %lu\n",contour_size);

        // Allocate memory for contour
        printf("// Allocate memory for contour\n");
        shp->contour = malloc((contour_size + 1) * sizeof(SDL_Point)); // +1 for the terminating (-1, -1)
        if (!shp->contour)
            return -1; // Memory allocation failed

        // Copy contour
        printf("// Copy contour\n");
        for (size_t i = 0; i < contour_size; i++)
            shp->contour[i] = contour[i];

        // Mark end of contour with (-1, -1)
        printf("// Mark end of contour with (-1, -1)\n");
        /// shp->contour[contour_size].x = -1;
        /// shp->contour[contour_size].y = -1;
        printf("// Setup end of contour (-1, -1)\n");

        return 0; // Success
    }

    void shape__draw(scene_t* scene,shape_t* shp, SDL_Renderer* renderer) {
        printf("start drawing : shape_t* %lu %lu, scene_t* %lu %lu,SDL_Renderer* %lu %lu\n",(unsigned long)shp,sizeof(*shp),(unsigned long)scene,sizeof(*scene),(unsigned long)renderer,sizeof(renderer));
        
        if (shp == NULL || renderer == NULL)
            return; // Invalid shape or renderer, or shape is dead
        //printf("// Valid shape and scene, and shape is not dead");
        printf("executing draw fn pointer %lu\n",(unsigned long)(shp->draw));
        // return;
        (*shp).draw(scene,shp, renderer);
        printf("// finished drawing");
    }

    void shape__physics(shape_t* shp, scene_t* scene, SDL_Renderer* renderer) {
        shape_t s=(*shp);
        //printf("// starting physics for shape is_dead:%d\n",s.is_dead);
        if (!shp || !scene || shp->is_dead)
            return;
        // Invalid shape or scene, or shape is dead
        //printf("// Valid shape and scene, and shape is not dead");
        
        (*shp).physics(scene, shp,renderer);
        //printf("// finished physics");
    }

    int shape__free(shape_t* shp) {
        if (!shp)
            return -1; // Invalid shape
        
        free(shp);

        return ENT_ERR_OK; // Success
    }

    SDL_Rect shape__get_bounding_box(shape_t* shp) {
        SDL_Rect rect = {0, 0, 0, 0};

        if (!shp || shp->is_dead)
            return rect; // Invalid shape or shape is dead
        
        // Logic to calculate bounding box
        // ...

        return rect;
    }

    int shape__collides_with(shape_t* a, shape_t* b) {
        if (!a || !b || a->is_dead || b->is_dead)
            return 0; // Invalid shapes or any of them is dead
        
        // Logic to check collision
        // ...

        return ENT_ERR_OK; // Return 1 if collides, 0 otherwise
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
    printf("scene count before adding shape : %lu\n",count);
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
    printf("scene count after adding shape : %lu\n",count);
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
    printf("start drawing : scene_t* %lu %lu,SDL_Renderer* %lu %lu\n",(unsigned long)scene,sizeof(*scene),(unsigned long)renderer,sizeof(renderer));
    if (!scene || !renderer)
        return; // Invalid scene or renderer
    
    printf("counting shapes\n");
    unsigned long count = scene__shape_count(scene);
    printf("found %lu shapes\n",count);
    for (unsigned long i = 0; i < count; i++){
        printf("drawing shape %lu\n",i);
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

    void scene__execute_collisions(scene_t* scene) {
        if (!scene)
            return; // Invalid scene
        
        unsigned long count = scene->shape_count;
        for (unsigned long i = 0; i < count; i++) {
            for (unsigned long j = i + 1; j < count; j++) {
                if (shape__collides_with(scene->shapes[i], scene->shapes[j])) {
                    // Handle collision
                    // ...
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
        printf("// Free each shape\n");
        unsigned long count = scene->shape_count;
        for (unsigned long i = 0; i < count; i++)
            shape__free(scene->shapes[i]);
        
        // Free scene itself
        printf("// Free scene itself\n");
        free(scene);

        return ENT_ERR_OK; // Success
    }


#endif