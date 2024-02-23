#ifndef __SDL_PRIMITIVES__
#define __SDL_PRIMITIVES__


#ifndef M_PI
#define M_PI=3.14
#endif
#define SDL_DRAW_rectangle \
// Draw rectangle\
SDL_Rect rect = {100, 100, 200, 150};\
SDL_RenderDrawRect(renderer, &rect);\


#define SDL_DRAW_RECTANGLE \
/* Draw filled rectangle*/\
SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); /*blue*/\
SDL_RenderFillRect(renderer, &rect);\


#define SDL_DRAW_CIRCLE_FILLED(RENDERER,X,Y,R) \
/* Draw circle*/\
double d = SDL_acos((R-0.5)/R)*180/M_PI;\
int capacity=(int)360.0/d;\
SDL_Point points[capacity+1];\
for (int i = 0; i <= capacity; i += 1) {\
    float x = X + R * cos(d*i * M_PI / 180);\
    float y = Y + R * sin(d*i * M_PI / 180);\
    SDL_Point p={x,y};\
    points[i]=p;\
}\
SDL_Point p={X+R,0};\
points[capacity]=p;\
SDL_RenderDrawLines(RENDERER,points,capacity);\

#define SDL_DRAW_CIRCLE(X,Y,R) \
/* Draw circle*/\
double d = SDL_acos((R-0.5)/R)*180/M_PI;\
printf("\n +++ using angle increment %f for radius %d\n",d,R);\
for (float angle = 0; angle <= 360; angle += d) {\
    float x = X + R * cos(angle * M_PI / 180);\
    float y = Y + R * sin(angle * M_PI / 180);\
    SDL_RenderDrawPoint(renderer, (int)x, (int)y);\
    printf("{%d,%d}",(int)(R * cos(angle * M_PI / 180)),(int)(R * sin(angle * M_PI / 180)));\
}\
printf("\n");\


#define SDL_DRAW_ELLIPSE(X,Y,R1,R2) \
/*Draw ellipse*/\
SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); /* yellow*/\
int ellipse_x = X, ellipse_y = Y, ellipse_w = 2*R, ellipse_h = 2*R2;\
for (float angle = 0; angle <= 360; angle += 0.1) {\
    float x = ellipse_x + ellipse_w * cos(angle * M_PI / 180);\
    float y = ellipse_y + ellipse_h * sin(angle * M_PI / 180);\
    SDL_RenderDrawPoint(renderer, (int)x, (int)y);\
}\

#define SDL_DRAW_POLYLINE(POINTS) \
/* Draw polyline*/\
SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); /*magenta*/\
SDL_Point points[] = POINTS;/*{{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};*/\
SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));\

#define SDL_DRAW_TEXT(N,X,Y,FONT,COLOR,TEXT, ...) \
/* Render text*/\
char buffer_##N[1000];\
snprintf(buffer_##N,1000,TEXT,__VA_ARGS__);\
SDL_Surface *text_surface_##N = TTF_RenderText_Blended(FONT, buffer_##N, COLOR);\
SDL_Texture *text_texture_##N = SDL_CreateTextureFromSurface(renderer, text_surface_##N);\
SDL_Rect text_rect_##N = {X, Y, text_surface_##N->w, text_surface_##N->h};\
SDL_RenderCopy(renderer, text_texture_##N, NULL, &text_rect_##N);\


#endif