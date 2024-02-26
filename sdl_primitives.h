#ifndef __SDL_PRIMITIVES__
#define __SDL_PRIMITIVES__


#ifndef M_PI
#define M_PI 3.14
#endif
#define SDL_DRAW_rectangle \
/*Draw rectangle*/\
SDL_Rect rect = {100, 100, 200, 150};\
SDL_RenderDrawRect(renderer, &rect);\


#define SDL_DRAW_RECTANGLE \
/* Draw filled rectangle*/\
SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); /*blue*/\
SDL_RenderFillRect(renderer, &rect);\

#define SDL_DRAW_CIRCLE_FILLED_NEXT(RENDERER,X,Y,R) \
/* Draw circle*/\
double d = SDL_acos((R-0.5)/R)*180/M_PI;\
int points_per_arc=((int)(90.0/d + 1));\
if(points_per_arc<2){\
    points_per_arc=2;\
}\
SDL_Point points[points_per_arc*4+1];\
SDL_Point sh[points_per_arc*4+1];\
for (int i = 0; i <= points_per_arc; i += 1) {\
    float c=R*cos(d*i * M_PI / 180);\
    float s=R*sin(d*i * M_PI / 180);\
    points[0*(points_per_arc+1)+i].x=(int)(X + c);points[0*(points_per_arc+1)+i].y=(int)(Y + s);\
    points[1*(points_per_arc+1)+i].x=(int)(X - s);points[1*(points_per_arc+1)+i].y=(int)(Y + c);\
    points[2*(points_per_arc+1)+i].x=(int)(X - c);points[2*(points_per_arc+1)+i].y=(int)(Y - s);\
    points[3*(points_per_arc+1)+i].x=(int)(X + s);points[3*(points_per_arc+1)+i].y=(int)(Y - c);\
    sh[0*(points_per_arc)+i].x=(int)(+ c);sh[0*(points_per_arc)+i].y=(int)(+ s);\
    sh[1*(points_per_arc)+i].x=(int)(- s);sh[1*(points_per_arc)+i].y=(int)(+ c);\
    sh[2*(points_per_arc)+i].x=(int)(- c);sh[2*(points_per_arc)+i].y=(int)(- s);\
    sh[3*(points_per_arc)+i].x=(int)(+ s);sh[3*(points_per_arc)+i].y=(int)(- c);\
}\
SDL_Point p={X+R,0};\
points[capacity*4]=p;\
sh[capacity*4].x=R;sh[capacity*4].y=0;\
printf("CIRCLE(c%d,x%d,y%d,r%d){",capacity,X,Y,R);\
for(int k=0;k<capacity*4+1;k++){\
    printf("[%d]{%d,%d},",k,sh[k].x,sh[k].y);\
};\
printf("}\n");\
SDL_RenderDrawLines(RENDERER,points,capacity);\

void sdl_draw_circle_filled_next(SDL_Renderer* renderer, int center_x, int center_y, int radius) {
    double d = SDL_acos((radius - 0.5) / radius) * 180 / M_PI;
    int quarter_capacity = (int)(90.0 / d) + 1; // Calculate points only for the first quarter
    int total_capacity = 4 * quarter_capacity; // Total points for the full circle
    SDL_Point points[total_capacity]; // Adjusted array size for all points

    // Calculate points for the first quarter
    for (int i = 0; i < quarter_capacity; i++) {
        float angle = d * i * M_PI / 180;
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;

        // First quarter
        points[i] = (SDL_Point){center_x + x, center_y - y};

        // Second quarter (reflect over y-axis)
        points[2*quarter_capacity - i - 1] = (SDL_Point){center_x - x, center_y - y};

        // Third quarter (reflect over x-axis)
        points[2*quarter_capacity + i] = (SDL_Point){center_x - x, center_y + y};

        // Fourth quarter (reflect over y-axis again)
        points[4*quarter_capacity - i - 1] = (SDL_Point){center_x + x, center_y + y};
    }

    // Draw the circle
    SDL_RenderDrawLines(renderer, points, total_capacity);
}

void sdl_draw_circle_filled(SDL_Renderer* renderer, int center_x,int center_y,int radius){

    double d = SDL_acos((radius-0.5)/radius)*180/M_PI;
    int capacity=(int)360.0/d;
    SDL_Point points[capacity+1];
    for (int i = 0; i <= capacity; i += 1) {
        float x = center_x + radius * cos(d*i * M_PI / 180);
        float y = center_y + radius * sin(d*i * M_PI / 180);
        SDL_Point p={x,y};
        points[i]=p;
    }
    SDL_Point p={center_x+radius,0};
    points[capacity]=p;
    SDL_RenderDrawLines(renderer,points,capacity);
}

#define SDL_DRAW_CIRCLE_FILLED(RENDERER,X,Y,R) \
/* Draw circle*/\
double arc_size = SDL_acos((R-0.5)/R)*1000;\
int capacity=((int)(2000*M_PI/arc_size+1));\
SDL_Point points[capacity];\
for (int i = 0; i < capacity; i += 1) {\
    float c=R*cos(arc_size*i/1000);\
    float s=R*sin(arc_size*i/1000);\
    points[i].x=X + c;\
    points[i].y=Y + s;\
}\
/*SDL_Point p={X+R,0};*/\
points[capacity].x=X+R;\
points[capacity].y=0;\
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