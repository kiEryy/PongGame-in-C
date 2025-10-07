#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BODY_WIDTH 20
#define BODY_HEIGHT 100

#define SPEED_FACTOR 1.5

#define BALL_INIT_SPEED 1.5
#define BALL_MAX_SPEED 5.0
#define BALL_MIN_SPEED 2.0
#define BALL_SPEED_STEP 0.1

#define BALL_ANGLE_STEP (M_PI / 36) 

enum gameState { PLAYING, P1SCORE, P2SCORE};

struct Player {
    int y;
    SDL_Rect body;
};

struct Ball {
    int x;
    int y;
    SDL_Rect body;
    double speed;
    double theta;
};

struct Score {
    TTF_Font* font;
    SDL_Color color;
    SDL_Surface* surfaceMessage;
    SDL_Texture* message;
    SDL_Rect messageRect;
    int p1Points;
    int p2Points;
};

void initScore(struct Score *s) {
    s->color.r = 0;
    s->color.g = 255;
    s->color.b = 0;
    s->color.a = 255;
    
    s->messageRect.y = 50;
    s->messageRect.w = 50;
    s->messageRect.h = 50;
    s->messageRect.x = (int)(WINDOW_HEIGHT/2) + s->messageRect.w;
    s->p1Points = 0;
    s->p2Points = 0;

    printf("Score initialized\n");
}

void updateScore(struct Score *s, SDL_Renderer * renderer, const char* fontFilename) {
    char scoreArr[8];
    sprintf(scoreArr, "%d-%d", s->p1Points, s->p2Points);

    s->font = TTF_OpenFont(fontFilename, 24);
    if (!s->font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        exit(1);
    }

    s->surfaceMessage = TTF_RenderText_Solid(s->font, scoreArr, s->color);
    if (!s->surfaceMessage) {
        printf("Failed to create surface message: %s\n", TTF_GetError());
        exit(1);
    }

    s->message = SDL_CreateTextureFromSurface(renderer, s->surfaceMessage);
    if (s->message == NULL) {
        printf("Failed to create texture from surface: %s\n", SDL_GetError());
        exit(1);
    }

    s->messageRect.w = s->surfaceMessage->w;
    s->messageRect.h = s->surfaceMessage->h;

    SDL_FreeSurface(s->surfaceMessage);

}

void initBall(struct Ball *b) {
    b->x = (int)(WINDOW_WIDTH/2);
    b->y = (int)(WINDOW_HEIGHT/2);
    b->body.x = b->x;
    b->body.y = b->y;
    b->body.w = 5;
    b->body.h = 5;

    b->speed = BALL_MIN_SPEED;
    b->theta = M_PI/4; // for testing

    printf("Ball initialized\n");
}

void resetBall(struct Ball *b) {
    b->body.x = (int)(WINDOW_WIDTH/2);
    b->body.y = (int)(WINDOW_HEIGHT/2);
    b->speed = BALL_MIN_SPEED;
    b->theta = M_PI/4; // for testing
}

enum gameState checkCollision(struct Ball *b, struct Player *p1, struct Player *p2) {
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
        b->theta = b->theta - M_PI/2;
        b->speed += BALL_SPEED_STEP;
        printf("Collision with P1 - th: %f; speed: %f\n", b->theta, b->speed);
    } else if (SDL_HasIntersection(&(b->body), &(p2->body))) {
        b->theta = b->theta - M_PI/2;
        b->speed += BALL_SPEED_STEP;
        printf("Collision with P2 - th: %f; speed: %f\n", b->theta, b->speed);
    }
    
    // check exceeding vertical limits to check for goal
    if (b->body.x <= 0) {
        return P2SCORE;
    } else if (b->body.x >= WINDOW_WIDTH) {
        return P1SCORE;
    }

    //  limit speed
    if (b->speed > BALL_MAX_SPEED) {
        b->speed = BALL_MAX_SPEED;
    } else if (b->speed < BALL_MIN_SPEED) {
        b->speed = BALL_MIN_SPEED;
    }

    return PLAYING;
}

void updateBallPos(struct Ball *b, struct Player *p1, struct Player *p2) {

    b->body.x += (int)round(b->speed * sin(b->theta));
    b->body.y += (int)round(b->speed * cos(b->theta));
}

void initPlayer(struct Player *p, int x) {

    p->y = WINDOW_HEIGHT/2;
    p->body.x = x;
    p->body.y = p->y;
    p->body.w = BODY_WIDTH;
    p->body.h = BODY_HEIGHT;

    printf("Player initialized\n");
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
    
    // Initialize TTF
    if (TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
        return 0;
    }

    struct Player p1, p2;
    initPlayer(&p1, 10);
    initPlayer(&p2, (int) (WINDOW_WIDTH) - 30); // 10 offset + 20 of body width

    struct Ball b;
    initBall(&b);    

    struct Score score;
    initScore(&score);
    const char* fontFilename = "fonts/OpenSans-Light.ttf";
    //updateScore(&score, renderer, fontFilename);

    enum gameState status;

    bool run = true;
    SDL_Event event;
    int iterations = 0;
    while(run) {

        SDL_PumpEvents();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }

        const Uint8* keystates = SDL_GetKeyboardState(NULL);

        // check for collisions
        status = checkCollision(&b, &p1, &p2);
        switch(status) {
            case PLAYING:                
                break;
            case P1SCORE:
                score.p1Points++;
                resetBall(&b);
                printf("P1 scored!\n");
                break;
            case P2SCORE:
                score.p2Points++;
                resetBall(&b);
                printf("P2 scored!\n");
                break;
        }

        // player 1
        if (keystates[SDL_SCANCODE_W]) {updatePlayerPos(&p1, true);}
        if (keystates[SDL_SCANCODE_S]) {updatePlayerPos(&p1, false);}
        
        // Player 2
        if (keystates[SDL_SCANCODE_UP]) {updatePlayerPos(&p2, true);}
        if (keystates[SDL_SCANCODE_DOWN]) {updatePlayerPos(&p2, false);}
        
        // Ball
        updateBallPos(&b, &p1, &p2);
        printf("[%d][Ball] x: %d; y: %d; theta: %f; speed: %f\n", iterations, b.body.x, b.body.y, b.theta, b.speed);

        // Update score
        updateScore(&score, renderer, fontFilename);

        // Update Bodies
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &(p1.body));
        SDL_RenderFillRect(renderer, &(p2.body));
        
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(renderer, &(b.body));

        // Render score
        SDL_RenderCopy(renderer, score.message, NULL, &(score.messageRect));
        SDL_RenderPresent(renderer);

        // .
        SDL_Delay(16);
        iterations++;
    }
    SDL_DestroyTexture(score.message);
    TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
