#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BODY_WIDTH 20
#define BODY_HEIGHT 100

#define SPEED_FACTOR 1.5

struct Player {
    int y;
    SDL_Rect body;
};

struct Ball {
    int x;
    int y;
    SDL_Rect body;
};

void initPlayer(struct Player *p, int x) {

    p->y = WINDOW_HEIGHT/2;
    p->body.x = x;
    p->body.y = p->y;
    p->body.w = BODY_WIDTH;
    p->body.h = BODY_HEIGHT;
}

bool insideHeightMargins(int y) {
    return (y > 0) && (y < WINDOW_HEIGHT);
}

void updatePlayerPos(struct Player *p, bool isUp) {
    
    if (isUp && insideHeightMargins(p->body.y - 2)) {
        p->body.y -= 2;
    } else if (!isUp && insideHeightMargins(p->body.y + 2 + BODY_HEIGHT)) {
        p->body.y += 2; 
    }
}

int main(int argc, char* argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Cannot start SDL. Error %s\n", SDL_GetError());
        return 0;
    }

    #if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
        if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
        {
            printf("SDL can not disable compositor bypass!\n");
            return 0;
        }
    #endif

    SDL_Window *window = SDL_CreateWindow("Pong Game",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Error setting window. Error: %s\n", SDL_GetError());
        return 0;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        printf("Error setting renderer. Error: %s\n", SDL_GetError());
        return 0;
    }
    
    struct Player p1, p2;
    initPlayer(&p1, 10);
    initPlayer(&p2, (int) (WINDOW_WIDTH) - 30); // 10 offset + 20 of body width

    struct Ball b;
    

    bool run = true;
    SDL_Event event;
    while(run) {

        SDL_PumpEvents();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);

        // player 1
        if (keystates[SDL_SCANCODE_W]) {updatePlayerPos(&p1, true);}
        if (keystates[SDL_SCANCODE_S]) {updatePlayerPos(&p1, false);}
        
        // Player 2
        if (keystates[SDL_SCANCODE_UP]) {updatePlayerPos(&p2, true);}
        if (keystates[SDL_SCANCODE_DOWN]) {updatePlayerPos(&p2, false);}
        
        SDL_Delay(16);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &(p1.body));
        SDL_RenderFillRect(renderer, &(p2.body));
//        SDL_RenderFillRect(renderer, &(ball.body));

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
