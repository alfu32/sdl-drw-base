
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
        SDL_Point* anchor;
        int is_dead;
        physics_fn physics;
        drawing_fn draw;
        SDL_Point contour[];
    } shape_t;

    typedef struct scene_s {
        shape_t** shapes; // null terminated array
        char keys_state[SDL_NUM_SCANCODES];
        SDL_Point* mouse_position; // never touch the reference, just update fields
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
        if (!shp || !physics || !draw || !contour)
            return ENT_NULL_INPUT; // Invalid arguments
        
        shp->anchor = NULL; // Initialize anchor to NULL
        shp->is_dead = 0; // Initialize is_dead to 0
        shp->physics = physics;
        shp->draw = draw;
        

        // Copy contour
        size_t i = 0;
        while (contour[i].x != -1 && contour[i].y != -1) {
            shp->contour[i] = contour[i];
            i++;
        }
        shp->contour[i].x = -1; // Mark end of contour with (-1, -1)

        return ENT_ERR_OK; // Success
    }

    void shape__draw(scene_t* scene,shape_t* shp, SDL_Renderer* renderer) {
        if (!shp || !renderer || shp->is_dead)
            return; // Invalid shape or renderer, or shape is dead
        
        shp->draw(scene,shp, renderer);
    }

    void shape__physics(shape_t* shp, scene_t* scene, SDL_Renderer* renderer) {
        if (!shp || !scene || shp->is_dead)
            return; // Invalid shape or scene, or shape is dead
        
        shp->physics(scene, shp,renderer);
    }

    int shape__free(shape_t* shp) {
        if (!shp)
            return -1; // Invalid shape
        
        free(shp->anchor);
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
        return (scene_t*)malloc(sizeof(scene_t));
    }

    unsigned long scene__shape_count(scene_t* scene) {
        if (!scene)
            return 0; // Invalid scene
        
        unsigned long count = 0;
        while (scene->shapes[count] != NULL)
            count++;
        
        return count;
    }

    error_id scene__add_shape(scene_t* scene, shape_t* shp) {
        printf("checking shape");
        if (!scene || !shp)
            return ENT_NULL_INPUT; // Invalid scene or shape
        printf("adding shape");
        
        unsigned long count = scene__shape_count(scene);
        scene->shapes[count] = shp;
        scene->shapes[count + 1] = NULL; // Null-terminate the array

        return ENT_ERR_OK; // Success
    }

    error_id scene__remove_shape(scene_t* scene, shape_t* shp) {
        if (!scene || !shp)
            return ENT_NULL_INPUT; // Invalid scene or shape
        
        unsigned long count = scene__shape_count(scene);
        unsigned long i;
        for (i = 0; i < count; i++) {
            if (scene->shapes[i] == shp) {
                // Found the shape, now remove it by shifting the array
                while (i < count) {
                    scene->shapes[i] = scene->shapes[i + 1];
                    i++;
                }
                return ENT_ERR_OK; // Success
            }
        }

        return ENT_SHAPE_NOT_FOUND; // Shape not found
    }

    void scene__draw(scene_t* scene, SDL_Renderer* renderer) {
        if (!scene || !renderer)
            return; // Invalid scene or renderer
        
        unsigned long count = scene__shape_count(scene);
        for (unsigned long i = 0; i < count; i++)
            shape__draw(scene,scene->shapes[i], renderer);
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
        
        unsigned long count = scene__shape_count(scene);
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
        
        unsigned long count = scene__shape_count(scene);
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
        
        // Free each shape
        unsigned long count = scene__shape_count(scene);
        for (unsigned long i = 0; i < count; i++)
            shape__free(scene->shapes[i]);
        
        // Free scene itself
        free(scene);

        return ENT_ERR_OK; // Success
    }


#endif