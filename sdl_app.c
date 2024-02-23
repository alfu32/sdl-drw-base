

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "sdl_app_lib.h"
#include "entities.h"
#include "entities-behaviour.h"
#define NUMBER_PI 3.14

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
        SDL_Rect rect = {circle_x+10, circle_y+10, 200, 150};
        SDL_RenderDrawRect(renderer, &rect);

        // Draw filled rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue
        SDL_RenderFillRect(renderer, &rect);

        // Draw circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        //SDL_Rect vp=shape__align_viewport(shape,renderer);

        for (float angle = 0; angle <= 360; angle += 0.1) {
            float x = circle_x + radius * cos(angle * NUMBER_PI / 180);
            float y = circle_y + radius * sin(angle * NUMBER_PI / 180);
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }
        //shape__restore_viewport(&vp,renderer);
}

void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    ON_KEY(scene,SDL_SCANCODE_D,(*shape).anchor.x+=1)
    ON_KEY(scene,SDL_SCANCODE_A,(*shape).anchor.x-=1)
    ON_KEY(scene,SDL_SCANCODE_W,(*shape).anchor.y-=1)
    ON_KEY(scene,SDL_SCANCODE_S,(*shape).anchor.y+=1)
    printf("player %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}

void cursor_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){

        int circle_x = scene->mouse_position.x, circle_y = scene->mouse_position.y, radius = 50;

        // Draw rectangle
        SDL_Rect rect = {circle_x+10, circle_y+10, 200, 150};
        SDL_RenderDrawRect(renderer, &rect);

        // Draw filled rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue
        SDL_RenderFillRect(renderer, &rect);

        // Draw circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        //SDL_Rect vp=shape__align_viewport(shape,renderer);

        for (float angle = 0; angle <= 360; angle += 0.1) {
            float x = circle_x + radius * cos(angle * NUMBER_PI / 180);
            float y = circle_y + radius * sin(angle * NUMBER_PI / 180);
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }
        //shape__restore_viewport(&vp,renderer);
}

void cursor_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    shape->anchor.x=scene->mouse_position.x;
    shape->anchor.y=scene->mouse_position.y;
    printf("cursor %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}
int loop_running=1;
// Function that handles events
void handleEvents(scene_t* scene) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_KEYDOWN:
                //printf(" key down  : %d",event.key.keysym.scancode);
                scene->keys_state[event.key.keysym.scancode]=1;
                break;
            case SDL_KEYUP:
                //printf(" key up    : %d ",event.key.keysym.scancode);
                scene->keys_state[event.key.keysym.scancode]=0;
                break;
            case SDL_MOUSEMOTION:
                //printf(" mouse move: %d %d ",event.motion.x,event.motion.y);
                scene->mouse_position.x = event.motion.x;
                scene->mouse_position.y = event.motion.y;
        }
        if (event.type == SDL_QUIT || scene->keys_state[SDL_SCANCODE_ESCAPE]==1 ) {
                printf("\nquitting\n");
            loop_running=0;
            break;
        }
    }
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

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
    
    shape_t* player2 = shape__alloc();
    const SDL_Point contour_player2[] = {{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};
    shape__init(player2,&player_physics,&player_draw,contour_player2);
    scene__add_shape(scene,player2);

    scene->time_start = SDL_GetTicks64();

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

            // Update the screen
            SDL_RenderPresent(renderer);
        }
        usleep(100000);
    }

    // Clean up
    /// SDL_DestroyTexture(text_texture);
    /// SDL_FreeSurface(text_surface);
    /// TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    scene__free(scene);

    return 0;
}
