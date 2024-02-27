

#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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

//// definition 
int loop_running=1;
SDL_Texture *ship_svg_texture;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int canvas_width = 800;
int canvas_height = 640;


void handleResize(int new_width, int new_height);

SDL_Rect shape__align_viewport(shape_t* shape,SDL_Renderer* renderer);
int shape__restore_viewport(SDL_Rect* vp,SDL_Renderer* renderer);
void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void cursor_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
void projectile_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer);
shape_t* create_projectile(SDL_Point anchor,SDL_Point speed,SDL_Renderer *renderer,SDL_Rect *tex_size) ;
void handleEvents(scene_t* scene) ;


//// implementation


void handleResize(int new_width, int new_height) {
    // Update canvas size
    canvas_width = new_width;
    canvas_height = new_height;

    // Update renderer viewport
    SDL_RenderSetLogicalSize(renderer, canvas_width, canvas_height);
}

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

void player_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
        (*shape).memory.projectile_props.previous_pos.x=(*shape).anchor.x;
        (*shape).memory.projectile_props.previous_pos.y=(*shape).anchor.y;
    ON_KEY(scene,SDL_SCANCODE_D,
        (*shape).anchor.x+=1;
    )
    ON_KEY(scene,SDL_SCANCODE_A,
        (*shape).anchor.x-=1;
    )
    ON_KEY(scene,SDL_SCANCODE_W,
        (*shape).anchor.y-=1;
    )
    ON_KEY(scene,SDL_SCANCODE_S,
        (*shape).anchor.y+=1;
    )
    (*shape).memory.projectile_props.speed.x=(*shape).anchor.x-(*shape).memory.projectile_props.previous_pos.x;
    (*shape).memory.projectile_props.speed.y=(*shape).anchor.y-(*shape).memory.projectile_props.previous_pos.y;

    ON_KEY(scene,SDL_SCANCODE_SPACE,
        if(scene->time_start - shape->memory.ship_props.previous_firing_time > shape->memory.ship_props.firing_frequency ){
            SDL_Point speed={(*shape).memory.projectile_props.speed.x*2,(*shape).memory.projectile_props.speed.y*2};
            SDL_Rect projectile_source_rect={90,0,20,20};
            scene__add_shape(scene,create_projectile((*shape).anchor,speed,renderer,&projectile_source_rect));
            (&(shape->memory.ship_props))->previous_firing_time = scene->time_start;
        }
    );
    PRINTF("player %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}

void cursor_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    shape->anchor.x=scene->mouse_position.x-shape->sprite_source.w;
    shape->anchor.y=scene->mouse_position.y-shape->sprite_source.h;
    PRINTF("cursor %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}


void projectile_physics(scene_t* scene,shape_t* shape,SDL_Renderer* renderer){
    projectile_data_t *data=&(shape->memory.projectile_props);
    shape->anchor.x+=data->speed.x;
    shape->anchor.y+=data->speed.y;
    int maxw,maxh;
    SDL_GetRendererOutputSize(renderer,&maxw,&maxh);
    SDL_Rect bb=shape__get_bounding_box(shape);

    if(shape->anchor.x>(maxw-bb.w)){
        shape->anchor.x=(maxw-bb.w);
        data->speed.x*=-1;
    }
    if(shape->anchor.x<0){
        shape->anchor.x=0;
        data->speed.x*=-1;
    }
    if(shape->anchor.y>(maxh-bb.y-50)){
        shape->anchor.y=maxh-bb.y-50;
        data->speed.y*=-1;
    }
    if(shape->anchor.y<50){
        shape->anchor.y=50;
        data->speed.y*=-1;
    }
    PRINTF("projectile %lu %d %d",(unsigned long)shape,shape->anchor.x,shape->anchor.y);
}
void collision__handler(scene_t* scene,shape_t* self,shape_t* other,SDL_Renderer* renderer){
    int flag = (int)(self->type)*10 + (int)(other->type);

    switch(flag){
        case 11:
        case 21:
        case 31:
        case 12:
        case 13:break;
        case 22:// ship to ship
            self->memory.ship_props.life-=1;
            if(self->memory.ship_props.life <= 0) {
                self->is_dead=1;
            }
            other->memory.ship_props.life-=1;
            if(other->memory.ship_props.life <= 0) {
                other->is_dead=1;
            }
        break;
        case 23:// projectile to ship
            other->memory.ship_props.life-=1;
            if(other->memory.ship_props.life <= 0) {
                other->is_dead=1;
            }
            self->is_dead=1;
        break;
        case 32:// ship to projectile
            self->memory.ship_props.life-=1;
            if(self->memory.ship_props.life <= 0) {
                self->is_dead=1;
            }
            other->is_dead=1;
        break;
        case 33:// projectile to projectile
            self->is_dead=1;
            other->is_dead=1;
        break;
    }
}
shape_t* create_projectile(SDL_Point anchor,SDL_Point speed,SDL_Renderer *renderer,SDL_Rect *tex_size) {
    shape_t* projectile = shape__alloc();

    // TODO reuse texture
    shape__init_sprite_with_texture(projectile,renderer,&projectile_physics,ship_svg_texture,tex_size);
    projectile->memory.projectile_props.speed.x=speed.x;
    projectile->memory.projectile_props.speed.y=speed.y;
    projectile->anchor.x=anchor.x;
    projectile->anchor.y=anchor.y;
    projectile->type=PROJECTILE;
    projectile->collision=&collision__handler;
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
        } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            handleResize(event.window.data1, event.window.data2);
        }
    }
}

int main(int argc, const char** argv) {
    Uint64 frequency = SDL_GetPerformanceFrequency();
    printf("Ticks per second: %lu, per usecond:%lu\n", frequency,frequency/1000/1000);

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

    window = SDL_CreateWindow("Resizable Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, canvas_width, canvas_height, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    scene_t *scene = scene__alloc();
    ship_svg_texture = shape__static__init_texture(renderer,"sprites.svg");

    shape_t* player = shape__alloc();
    player->type=SHIP;
    player->collision=&collision__handler;
    printf("allocated player\n");
    
    SDL_Rect player_source_rect={0,0,70,60};
    printf("initialized texture source rectangle\n");
    error_id err_init_player = shape__init_sprite_with_texture(player,renderer,&player_physics,ship_svg_texture,&player_source_rect);
    if (err_init_player) {
        printf("texture initialisation error %d sprite\n",err_init_player);
        loop_running=0;
    }
    printf("initialized sprite\n");
    player->memory.ship_props.previous_firing_time=SDL_GetTicks64();
    player->memory.ship_props.firing_frequency=10;
    scene__add_shape(scene,player);
    printf("added player to scene\n");

    shape_t* cursor = shape__alloc();
    cursor->type=BACKGROUND;
    SDL_Rect cursor_source_rect={70,40,20,20};
    
    error_id err_init_cursor = shape__init_sprite_with_texture(cursor,renderer,&cursor_physics,ship_svg_texture,&cursor_source_rect);
    if(err_init_cursor){
        printf("texture initialisation error %d sprite\n",err_init_player);
        loop_running=0;
    }
    scene__add_shape(scene,cursor);

    scene->time_start = SDL_GetTicks64();
    TTF_Font *font = TTF_OpenFont(font_name, pt_size);

    while (loop_running) {
        handleEvents(scene);
        if (loop_running) {

            int maxw,maxh;
            SDL_GetRendererOutputSize(renderer,&maxw,&maxh);

            // Set drawing color
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

            // Clear the screen
            SDL_RenderClear(renderer);

            scene->time_start = SDL_GetTicks64();
            
            scene__physics(scene,renderer);
            scene->time_physics = SDL_GetTicks64();
            scene__draw(scene,renderer);
            scene->time_draw = SDL_GetTicks64();
            scene__execute_collisions(scene,renderer);
            scene->time_collisions = SDL_GetTicks64();
            scene__clear_dead_shapes(scene);
            scene->time_cleanup = SDL_GetTicks64();

            SDL_Color color={200,100,0,255};
            SDL_DRAW_TEXT(1,10,10,font,color,"entities %05lu, frame %03lu ms, w:%d, h:%d",
                scene->shape_count,
                (scene->time_cleanup-scene->time_start),
                maxw,maxh
            )

            SDL_DRAW_TEXT(2,10,maxh-30,font,color,"physics: %03lums, draw: %03lu ms",
                (scene->time_physics-scene->time_start),
                (scene->time_draw-scene->time_physics)
            )

            // Update the screen
            SDL_RenderPresent(renderer);
            scene->time_end=SDL_GetTicks64();
        }
        unsigned long int total_time = scene->time_end - scene->time_start;
        if(total_time < 20000000) {
            usleep(40000-total_time/1000);
        }
        
    }

    // Clean up
    /// SDL_DestroyTexture(text_texture);
    /// SDL_FreeSurface(text_surface);
    /// TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyTexture(ship_svg_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    scene__free(scene);

    return 0;
}
