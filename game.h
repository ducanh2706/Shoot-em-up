#ifndef _GAME__H
#define _GAME__H

#include <SDL2/SDL.h>
#include <list>
#include <iostream>
#include "struct.h"
#include "graphics.h"
#include <cstdlib>

void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
void blitRect(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, int x, int y);

struct Game {
    Entity *player;
    std::list<Entity*> fighters;
    std::list<Entity*> bullets;
    std::list<Explosion*> explosions;
    std::list<Debris*> debris;

    int enemySpawnTimer;
    int stageResetTimer;
    Star stars[MAX_STARS];

    int backgroundX;

    SDL_Texture *bulletTexture, *enemyTexture, *playerTexture, *alienBulletTexture, *backgroundTexture, *starTexture;
    

    int upKey = SDL_SCANCODE_UP;
    int downKey = SDL_SCANCODE_DOWN;
    int leftKey = SDL_SCANCODE_LEFT;
    int rightKey = SDL_SCANCODE_RIGHT;
    int sKey = SDL_SCANCODE_S;

    void initPlayer(Graphics *graphics) {
        player = new Entity();
        player->x = 100;
        player->y = 100;
        player->dx = 0;
        player->dy = 0;
        player->health = 1;
        player->reload = 0;
        player->side = SIDE_PLAYER;
        player->texture = playerTexture;
        SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
        fighters.push_back(player);
    }
    
    void initStarfield() {
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].x = rand() % SCREEN_WIDTH;
            stars[i].y = rand() % SCREEN_HEIGHT;
            stars[i].speed = 1 + rand() % STAR_SPEED;
        }
    }

    void resetStage(Graphics *graphics) {
        fighters.clear();
        bullets.clear();
        explosions.clear();
        debris.clear();

        initPlayer(graphics);
        initStarfield();

        enemySpawnTimer = 0;
        backgroundX = 0;
        
        stageResetTimer = FPS * 3;
    }

    void init(Graphics *graphics) {
        srand(time(NULL));

        playerTexture = graphics->loadTexture("img/spaceCraft_rotated.png");
        bulletTexture = graphics->loadTexture("img/bulletEgg.png");
        enemyTexture = graphics->loadTexture("img/enemyCraft.png");
        alienBulletTexture = graphics->loadTexture("img/bulletEgg.png");
        backgroundTexture = graphics->loadTexture("img/background.jpg");


        resetStage(graphics);
    }

    void fireBullet(){
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = player->x;
        bullet->y = player->y;
        bullet->dx = BULLET_SPEED;
        bullet->health = 1;
        bullet->texture = bulletTexture;
        bullet->side = SIDE_PLAYER;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
        
        bullet->y += (player->h / 2) - (bullet->h / 2);

        player->reload = PLAYER_RELOAD;
    }

    void doPlayer(int *keyboard){
        if (player == NULL) return;
        player->dx = player->dy = 0;

        if (player->reload > 0) {
            player->reload--;
        }

        if (keyboard[upKey]){
            player->dy = -PLAYER_SPEED;
        }
        if (keyboard[downKey]){
            player->dy = PLAYER_SPEED;
        }

        if (keyboard[leftKey]){
            player->dx = -PLAYER_SPEED;
        }

        if (keyboard[rightKey]){
            player->dx = PLAYER_SPEED;
        }

        if (keyboard[sKey] && player->reload == 0){
            fireBullet();
        }

    }

    void fireAlienBullet(Entity *enemy){
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = enemy->x;
        bullet->y = enemy->y;
        bullet->health = 1;
        bullet->texture = alienBulletTexture;
        bullet->side = SIDE_ALIEN;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        bullet->x += (enemy->w / 2) - (bullet->w / 2);
        bullet->y += (enemy->h / 2) - (bullet->h / 2);

        calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), enemy->x, enemy->y, &bullet->dx, &bullet->dy);

        bullet->dx *= ALIEN_BULLET_SPEED;
        bullet->dy *= ALIEN_BULLET_SPEED;

        enemy->reload = (rand() % FPS * 2);


    }

    void doEnemies(){
        for (auto enemy : fighters){
            if (enemy != player && player != NULL && --enemy->reload <= 0){
                fireAlienBullet(enemy);
            }
        }
    }

    bool bulletHitFighter(Entity *bullet){
        for (auto fighter : fighters){
            if (bullet->side != fighter->side && (bullet->collision(fighter) || fighter->collision(bullet))){
                bullet->health = 0;
                fighter->health = 0;
                return true;
            }
        }
        return false;
    }

    void doBullets() {
        auto ins = bullets.begin();
        while (ins != bullets.end()){
            auto temp = ins++;
            Entity *b = *temp;
            b->x += b->dx;
            b->y += b->dy;
            if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT){
                delete b;
                bullets.erase(temp);
            }
        }
    }
    
    void doFighters() {
        auto ins = fighters.begin();

        while (ins != fighters.end()) {
            auto temp = ins++;
            Entity *b = *temp;
            b->x += b->dx;
            b->y += b->dy;
            if (b != player && b->x < -b->w){
                b->health = 0;
            }
            if (b->health == 0){
                if (b == player){
                    player = NULL;
                }
                addDebris(b);
                delete b;
                fighters.erase(temp);     
            }
        }
    }

    void spawnEnemies() {
        Entity *enemy = new Entity();
        if (--enemySpawnTimer <= 0) {
            enemy->x = SCREEN_WIDTH;
            enemy->y = rand() % SCREEN_HEIGHT;
            enemy->dy = 0;
            enemy->health = 1;
            enemy->texture = enemyTexture;
            enemy->dx = -(2 + rand() % 4);
            enemy->side = SIDE_ALIEN;
            enemySpawnTimer = 30 + rand() % 60;
            SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);
            fighters.push_back(enemy);
        }
    }   

    void clipPlayer(){
        if (player == NULL) return;
        if (player->x < 0){
            player->x = 0;
        }

        if (player->y < 0){
            player->y = 0;
        }

        if (player->x > SCREEN_WIDTH / 2){
            player->x = SCREEN_WIDTH / 2;
        }

        if (player->y > SCREEN_HEIGHT - player->h){
            player->y = SCREEN_HEIGHT - player->h;
        }
    }
    
    void doBackground() {
        if (--backgroundX < -SCREEN_WIDTH){
            backgroundX = 0;
        }
    }

    void doStarFields() {
        for (int i = 0; i < MAX_STARS; i++){
            stars[i].x -= stars[i].speed;

            if (stars[i].x < 0){
                stars[i].x = SCREEN_WIDTH + stars[i].x;
            }
        }
    }

    void doExplosions() {
        auto ins = explosions.begin();
        while (ins != explosions.end()){
            auto temp = ins++;
            Explosion *b = *temp;

            b->x += b->dx;
            b->y += b->dy;

            if (--b->a <= 0){
                delete b;
                explosions.erase(temp);
            }
        }
    }

    void doDebris() {
        auto ins = debris.begin();
        while (ins != debris.end()){
            auto temp = ins++;
            Debris *b = *temp;
            
            b->x += b->dx;
            b->y += b->dy;

            b->dy += 0.5;

            if (--b->life <= 0){
                delete b;
                debris.erase(temp);
            }
        }
    }

    void addExplosions(int x, int y, int num) {
        for (int i = 0; i < num; i++){
            Explosion *e = new Explosion();
            explosions.push_back(e);

            e->x = x + (rand() % 32) - (rand() % 32);
            e->y = y + (rand() % 32) - (rand() % 32);
            e->dx = (rand() % 10) - (rand() % 10);
            e->dy = (rand() % 10) - (rand() % 10);

            e->dx /= 10;
            e->dy /= 10;

            switch (rand() % 4) {
                case 0:
                    e->r = 255;
                    break;
                case 1:
                    e->r = 255;
                    e->g = 128;
                    break;
                case 2:
                    e->r = 255;
                    e->g = 255;
                    break;
                default:
                    e->r = 255;
                    e->g = 255;
                    e->b = 255;
                    break;
            }

            e->a = rand() % FPS * 3;

        }
    }
    
    void addDebris(Entity *enemy) {
        int x, y, w, h;

        w = enemy->w / 2;
        h = enemy->h / 2;
        
        for (y = 0; y <= h; y += h){
            for (x = 0; x <= w; x += w){
                Debris *d = new Debris();  
                debris.push_back(d);

                d->x = enemy->x + enemy->w / 2;
                d->y = enemy->y + enemy->h / 2;
                d->dx = (rand() % 5) - (rand() % 5);
                d->dy = -(5 + rand() % 12);
                d->life = FPS * 2;
                d->texture = enemy->texture;

                d->rect.x = x;
                d->rect.y = y;
                d->rect.w = w;
                d->rect.h = h;
            }
        }
    }

    void logic(Graphics *graphics, int *keyboard) {
        doBackground();
        doStarFields();
        doPlayer(keyboard);
        doFighters();
        doEnemies();
        doBullets();
        spawnEnemies(); 
        doExplosions();
        doDebris();

        clipPlayer();

        if (player == NULL && --stageResetTimer <= 0){
            resetStage(graphics);
        }
    }


    void drawFighters(Graphics *graphics) {
        for (auto fighter : fighters){
            graphics->renderTexture(fighter->texture, fighter->x, fighter->y);
        }
    }

    void drawBullets(Graphics *graphics) {
        for (auto bullet : bullets){
            graphics->renderTexture(bullet->texture, bullet->x, bullet->y);
        }
    }

    void drawBackground(Graphics *graphics) {
        SDL_Rect dest;
        int x;

        for (x = backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH){
            dest.x = x;
            dest.y = 0;
            dest.w = SCREEN_WIDTH;
            dest.h = SCREEN_HEIGHT;
            SDL_RenderCopy(graphics->renderer, backgroundTexture, NULL, &dest);
        }
    }
    void drawStarfield(Graphics *graphics) {
        for (int i = 0; i < MAX_STARS; i++) {
            int c = 32 * stars[i].speed;
            SDL_SetRenderDrawColor(graphics->renderer, c, c, c, 255);
            SDL_RenderDrawLine(graphics->renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
        }
    }

    void drawDebris(Graphics *graphics) {
        for (auto d : debris) {
            blitRect(graphics->renderer, d->texture, &d->rect, d->x, d->y);
        }
    }

    void drawExplosions(Graphics *graphics) {

    }
     
    void draw(Graphics *graphics) {
        // drawBackground(graphics);
        drawStarfield(graphics);
        drawFighters(graphics);
        drawDebris(graphics);
        drawExplosions(graphics);
        drawBullets(graphics);

    }


};

#endif