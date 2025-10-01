#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BODY_WIDTH 20
#define BODY_HEIGHT 100

#define SPEED_FACTOR 1.5

#define BALL_INIT_SPEED 1.5

struct Player {
    int y;
    SDL_Rect body;
};

struct Ball {
    int x;
    int y;
    SDL_Rect body;
    double speed;
    double rho;
    double theta;
};

void initBall(struct Ball *b) {
    b->x = (int)(WINDOW_WIDTH/2);
    b->y = (int)(WINDOW_HEIGHT/2);
    b->body.x = b->x;
    b->body.y = b->y;
    b->body.w = 5;
    b->body.h = 5;

    b->speed = BALL_INIT_SPEED;
    b->rho = b->speed;
    srand(time(NULL));
    b->theta = ((double)rand() / RAND_MAX) * 2 * M_PI;

    // limit angles 
    // if (b->theta < 3 * M_PI / 4 && b->theta > M_PI/4) {
    //     b->theta = 3 * M_PI / 4;
    // }
    // if (b->theta < - 3 * M_PI / 4 && b->theta > - M_PI/4) {
    //     b->theta = - 3 * M_PI / 4;
    // }

    b->theta = M_PI/2; // for testing

   printf("init theta: %f\n", b->theta);
}

void checkCollision(struct Ball *b, struct Player *p1, struct Player *p2) {
    // check collision with top and bottom walls
    if (b->body.y <= 0 ) {
        if (b->theta < 0) {
            b->theta = -M_PI - b->theta;
        } else {
            b->theta = M_PI - b->theta;
        }
    } else if (b->body.y + b->body.h >= WINDOW_HEIGHT) {
        if (b->theta > 0) {
            b->theta = M_PI - b->theta;
        } else {
            b->theta = -M_PI - b->theta;
        }
    }

    // check collision with players
    if (SDL_HasIntersection(&(b->body), &(p1->body))) {
        b->theta = -(M_PI - b->theta);
        b->rho += 0.1;
        b->speed = b->rho;
        printf("Collision with P1 - th: %f; rho: %f; speed: %f\n", b->theta, b->rho, b->speed);
    } else if (SDL_HasIntersection(&(b->body), &(p2->body))) {
        b->theta = -(M_PI - b->theta);
        b->rho += 0.1;
        b->speed = b->rho;
        printf("Collision with P2 - th: %f; rho: %f; speed: %f\n", b->theta, b->rho, b->speed);
    }
}

void updateBallPos(struct Ball *b, struct Player *p1, struct Player *p2) {
    checkCollision(b, p1, p2);

    b->body.x += (int)(SPEED_FACTOR * sin(b->theta));
    b->body.y += (int)(SPEED_FACTOR * cos(b->theta));

    printf("update x: %d; y: %d\n", b->body.x, b->body.y);
}

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
    initBall(&b);    

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
        
        updateBallPos(&b, &p1, &p2);

        SDL_Delay(16);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &(p1.body));
        SDL_RenderFillRect(renderer, &(p2.body));
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &(b.body));

        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
