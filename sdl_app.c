

#include <stdlib.h>
#include <SDL2/SDL.h>

#include "sdl_app_lib.h"
#include "entities.h"
#include "entities-behaviour.h"

SDL_Rect shape__align_viewport(shape_t* shape,SDL_Renderer* renderer){
        SDL_Rect vp;
        SDL_RenderGetViewport(renderer,&vp);
        SDL_Point a = *(shape->anchor);
        SDL_Rect viewport = {a.x, a.y, 200, 200}; // Example translation: move drawing area to (100, 100)
        SDL_RenderSetViewport(renderer, &viewport);
        return vp;

}
int shape__restore_viewport(SDL_Rect* vp,SDL_Renderer* renderer){
        SDL_RenderSetViewport(renderer, vp);
        return 0;
}

void player_draw(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){

        // Draw circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        SDL_Rect vp=shape__align_viewport(shape,renderer);

        int circle_x = 400, circle_y = 200, radius = 50;
        for (float angle = 0; angle <= 360; angle += 0.1) {
            float x = circle_x + radius * cos(angle * M_PI / 180);
            float y = circle_y + radius * sin(angle * M_PI / 180);
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }
        shape__restore_viewport(&vp,renderer);
}
void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    ON_KEY(scene,SDL_SCANCODE_D,shape->anchor->x+=1)
    ON_KEY(scene,SDL_SCANCODE_A,shape->anchor->x-=1)
}

int loop_running=1;

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    scene_t *scene = scene__alloc();

    shape_t* player = shape__alloc();
    const SDL_Point contour[] = {{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};
    shape__init(player,player_physics,player_draw,contour);
    scene__add_shape(scene,player);

    while (loop_running) {
        // Set drawing color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

        // Clear the screen
        SDL_RenderClear(renderer);

        // Set drawing color
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red

        // Draw rectangle
        SDL_Rect rect = {100, 100, 200, 150};
        SDL_RenderDrawRect(renderer, &rect);

        // Draw filled rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // blue
        SDL_RenderFillRect(renderer, &rect);

        // Draw circle
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // green
        int circle_x = 400, circle_y = 200, radius = 50;
        for (float angle = 0; angle <= 360; angle += 0.1) {
            float x = circle_x + radius * cos(angle * M_PI / 180);
            float y = circle_y + radius * sin(angle * M_PI / 180);
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }

        // Draw ellipse
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // yellow
        int ellipse_x = 500, ellipse_y = 300, ellipse_w = 100, ellipse_h = 50;
        for (float angle = 0; angle <= 360; angle += 0.1) {
            float x = ellipse_x + ellipse_w * cos(angle * M_PI / 180);
            float y = ellipse_y + ellipse_h * sin(angle * M_PI / 180);
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }

        // Draw polyline
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); // magenta
        SDL_Point points[] = {{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};
        SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));

        // Render text
        /// SDL_Color text_color = {0, 0, 0, 255}; // black
        /// TTF_Font *font = TTF_OpenFont("arial.ttf", 24);
        /// SDL_Surface *text_surface = TTF_RenderText_Blended(font, "Hello, SDL!", text_color);
        /// SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        /// SDL_Rect text_rect = {50, 400, text_surface->w, text_surface->h};
        /// SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Wait for quit event
        SDL_Event event;
        if(SDL_WaitEvent(&event)){
            switch(event.type){
                case SDL_KEYDOWN:
                    printf("key down  : %d\n",event.key.keysym.scancode);
                    scene->keys_state[event.key.keysym.scancode]=1;
                    break;
                case SDL_KEYUP:
                    printf("key up    : %d\n",event.key.keysym.scancode);
                    scene->keys_state[event.key.keysym.scancode]=0;
                    break;
                case SDL_MOUSEMOTION:
                    printf("mouse move: %d %d\n",event.motion.x,event.motion.y);
                    //scene->pointer_position->x = event.motion.x;
                    //scene->pointer_position->y = event.motion.y;
            }
            if (event.type == SDL_QUIT || scene->keys_state[SDL_SCANCODE_ESCAPE]==1 ) {
                    printf("quitting\n");
                loop_running=0;
                break;
            }
        }
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
