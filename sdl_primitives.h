#ifndef __SDL_PRIMITIVES__
#define __SDL_PRIMITIVES__

#define SDL_DRAW_rectangle \
// Draw rectangle\
SDL_Rect rect = {100, 100, 200, 150};\
SDL_RenderDrawRect(renderer, &rect);\


#define SDL_DRAW_rectangle \
/* Draw filled rectangle*/\
SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); /*blue*/\
SDL_RenderFillRect(renderer, &rect);\


#define SDL_DRAW_circle(X,Y,R) \
/* Draw circle*/\
SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); /*green*/\
int circle_x = X, circle_y = Y, radius = R;\
for (float angle = 0; angle <= 360; angle += 0.1) {\
    float x = X + R * cos(angle * M_PI / 180);\
    float y = X + R * sin(angle * M_PI / 180);\
    SDL_RenderDrawPoint(renderer, (int)x, (int)y);\
}\


#define SDL_DRAW_ellipse(X,Y,R1,R2) \
/*Draw ellipse*/\
SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); /* yellow*/\
int ellipse_x = X, ellipse_y = Y, ellipse_w = 2*R, ellipse_h = 2*R2;\
for (float angle = 0; angle <= 360; angle += 0.1) {\
    float x = ellipse_x + ellipse_w * cos(angle * M_PI / 180);\
    float y = ellipse_y + ellipse_h * sin(angle * M_PI / 180);\
    SDL_RenderDrawPoint(renderer, (int)x, (int)y);\
}\

#define SDL_DRAW_polyline(POINTS) \
/* Draw polyline*/\
SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); /*magenta*/\
SDL_Point points[] = POINTS;/*{{50, 50}, {100, 200}, {200, 150}, {250, 300}, {400, 250}};*/\
SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));\

#define SDL_DRAW_text(TEXT,COLOR) \
/* Render text*/\
SDL_Color text_color = COLOR; /*black*/\
TTF_Font *font = TTF_OpenFont("arial.ttf", 24);\
SDL_Surface *text_surface = TTF_RenderText_Blended(font, TEXT, text_color);\
SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);\
SDL_Rect text_rect = {50, 400, text_surface->w, text_surface->h};\
SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);\


#endif