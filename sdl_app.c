

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <unistd.h>

#include "sdl_app_lib.h"
#include "entities.h"
#include "entities-behaviour.h"
#include "sdl_primitives.h"
#define NUMBER_PI 3.14

#ifndef M_PI
#define M_PI=3.14
#endif

struct projectile_data_s {
    SDL_Point speed;
} projectile_data_s;

typedef struct projectile_data_s projectile_data_t;

int loop_running=1;

SDL_Rect shape__align_viewport(shape_t* shape,SDL_Renderer* renderer);
int shape__restore_viewport(SDL_Rect* vp,SDL_Renderer* renderer);
void player_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void cursor_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void cursor_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void projectile_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void projectile_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
shape_t* create_projectile(SDL_Point anchor,SDL_Point speed) ;
void handleEvents(scene_t* scene) ;


SDL_Rect shape__align_viewport(shape_t* shape,SDL_Renderer* renderer){
        SDL_Rect vp;
        SDL_RenderGetViewport(renderer,&vp);
        SDL_Rect viewport = {(*shape).anchor.x, (*shape).anchor.y, 200, 200}; // Example translation: move drawing area to (100, 100)
        SDL_RenderSetViewport(renderer, &viewport);
        return vp;

}
int shape__restore_viewport(SDL_Rect* vp,SDL_Renderer* renderer){
        SDL_RenderSetViewport(renderer, vp);
        return 0;
}

void player_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){

        int circle_x = shape->anchor.x, circle_y = shape->anchor.y, radius = 50;

        // Draw rectangle
        SDL_Rect rect = {circle_x+10, circle_y+10, 50, 50};
        SDL_RenderDrawRect(renderer, &rect);

        // Draw filled rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue
        SDL_RenderFillRect(renderer, &rect);
}

void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    ON_KEY(scene,SDL_SCANCODE_D,(*shape).anchor.x+=1;)
    ON_KEY(scene,SDL_SCANCODE_A,(*shape).anchor.x-=1;)
    ON_KEY(scene,SDL_SCANCODE_W,(*shape).anchor.y-=1;)
    ON_KEY(scene,SDL_SCANCODE_S,(*shape).anchor.y+=1;)
    ON_KEY(scene,SDL_SCANCODE_SPACE,
        SDL_Point speed={3,3};
        scene__add_shape(scene,create_projectile((*shape).anchor,speed));
    );
    PRINTF("player %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}

void cursor_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
        // Draw circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        sdl_draw_circle_filled_next(renderer,scene->mouse_position.x,scene->mouse_position.y,25);
        //shape__restore_viewport(&vp,renderer);
}

void cursor_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    shape->anchor.x=scene->mouse_position.x;
    shape->anchor.y=scene->mouse_position.y;
    PRINTF("cursor %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}

void projectile_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
        int circle_x = shape->anchor.x, circle_y = shape->anchor.y, radius = 5;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        sdl_draw_circle_filled_next(renderer,shape->anchor.x,shape->anchor.y,5);
        //shape__restore_viewport(&vp,renderer);
}


void projectile_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    projectile_data_t* data=(projectile_data_t*)shape->memory;
    shape->anchor.x+=data->speed.x;
    shape->anchor.y+=data->speed.y;

    if(shape->anchor.x>640){
        shape->anchor.x=640;
        data->speed.x*=-1;
    }
    if(shape->anchor.x<0){
        shape->anchor.x=0;
        data->speed.x*=-1;
    }
    if(shape->anchor.y>480){
        shape->anchor.y=480;
        data->speed.y*=-1;
    }
    if(shape->anchor.y<0){
        shape->anchor.y=0;
        data->speed.y*=-1;
    }
    PRINTF("projectile %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}
shape_t* create_projectile(SDL_Point anchor,SDL_Point speed) {
    shape_t* projectile = shape__alloc();
    const SDL_Point contour_player2[] = {{5,0},{4,2},{3,3},{1,4},{-1,4},{-3,3},{-4,2},{-4,0},{-4,-2},{-3,-3},{-1,-4},{1,-4},{3,-3},{4,-2}};
    shape__init(projectile,&projectile_physics,&projectile_draw,contour_player2);
    projectile_data_t* data=(projectile_data_t*)projectile->memory;
    data->speed.x=speed.x;data->speed.y=speed.y;
    projectile->anchor.x=anchor.x;
    projectile->anchor.y=anchor.y;
    return projectile;
}
// Function that handles events
void handleEvents(scene_t* scene) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_KEYDOWN:
                //PRINTF(" key down  : %d",event.key.keysym.scancode);
                scene->keys_state[event.key.keysym.scancode]=1;
                break;
            case SDL_KEYUP:
                //PRINTF(" key up    : %d ",event.key.keysym.scancode);
                scene->keys_state[event.key.keysym.scancode]=0;
                break;
            case SDL_MOUSEMOTION:
                //PRINTF(" mouse move: %d %d ",event.motion.x,event.motion.y);
                scene->mouse_position.x = event.motion.x;
                scene->mouse_position.y = event.motion.y;
        }
        if (event.type == SDL_QUIT || scene->keys_state[SDL_SCANCODE_ESCAPE]==1 ) {
                PRINTF("\nquitting\n");
            loop_running=0;
            break;
        }
    }
}

int main(int argc, const char** argv) {
    int pt_size=24;
    const char* font_name="fonts/Arcade.ttf";
    if(argc>=2){
        font_name=(argv[1]);
    }
    if(argc>=3){
        pt_size=atoi(argv[2]);
    }
    printf("using font %s, size: %d\n",argv[1],pt_size);
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    scene_t *scene = scene__alloc();

    shape_t* player = shape__alloc();
    const SDL_Point contour_player[] = {{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};
    shape__init(player,&player_physics,&player_draw,contour_player);
    scene__add_shape(scene,player);

    shape_t* cursor = shape__alloc();
    const SDL_Point contour_cursor[] = {{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};
    shape__init(cursor,&cursor_physics,&cursor_draw,contour_cursor);
    scene__add_shape(scene,cursor);
    

    SDL_Point origin= {20,30};
    SDL_Point speed={3,3};
    shape_t* projectile = create_projectile(origin,speed);
    scene__add_shape(scene,projectile);

    scene->time_start = SDL_GetTicks64();
    TTF_Font *font = TTF_OpenFont(font_name, pt_size);

    while (loop_running) {
        handleEvents(scene);
        if(loop_running){
            // Set drawing color
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

            // Clear the screen
            SDL_RenderClear(renderer);

            scene->time_start = SDL_GetTicks64();
            
            scene__physics(scene,renderer);
            scene->time_physics = SDL_GetTicks64();
            scene__draw(scene,renderer);
            scene->time_draw = SDL_GetTicks64();
            scene->time_collisions = SDL_GetTicks64();
            scene->time_cleanup = SDL_GetTicks64();

            SDL_Color color={200,100,0,255};
            SDL_DRAW_TEXT(1,10,10,font,color,"entities %05lu, frame %03lu ms",
                scene->shape_count,
                (scene->time_cleanup-scene->time_start)*20
            )
            SDL_DRAW_TEXT(2,10,440,font,color,"physics: %03lums, draw: %03lu ms",
                (scene->time_physics-scene->time_start)*20,
                (scene->time_draw-scene->time_physics)*20
            )

            // Update the screen
            SDL_RenderPresent(renderer);

            // Free text_texture after rendering
            SDL_DestroyTexture(text_texture_1);
            SDL_DestroyTexture(text_texture_2);

            // Free text_surface after creating texture
            SDL_FreeSurface(text_surface_1);
            SDL_FreeSurface(text_surface_2);
        }
        usleep(1*1000);
    }

    // Clean up
    /// SDL_DestroyTexture(text_texture);
    /// SDL_FreeSurface(text_surface);
    /// TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    scene__free(scene);

    return 0;
}
