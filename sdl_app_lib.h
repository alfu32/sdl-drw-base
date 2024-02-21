
#include <time.h>

#define INHERIT_SHAPE_T \
    SDL_Point* anchor;\
    int is_dead;\
    physics_fn physics;\
    drawing_fn draw;\

#define alloc(T) (T*)malloc(sizeof(T))
#define alloc_many(T,size) (T*)malloc(sizeof(T) * (size) )
#define alloc_grow(ptr,T,grow_size) (T*)realloc(ptr, sizeof(ptr) + sizeof(T) * (grow_size) )
#define alloc_shrink(ptr,T,shrink_size) (T*)realloc(ptr, sizeof(ptr) + sizeof(T) * (shrink_size) )
#define DECLARE_TEST(name,block) int test_##name(){\
    int t0=(int)time(NULL);\
    block\
    return (int)time(NULL) - t0;\
}
#define RUN_TEST(name) void test_##name()


