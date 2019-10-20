/*
 * Level.h
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#ifndef LEVELMANAGER_H_
#define LEVELMANAGER_H_

#include "AppTypes.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <string>
#include <array>

#define LEVEL_MAX (10)
#define LEVEL_ICON_W (185)
#define LEVEL_ICON_H (180)

#define LEVEL1_COLUMN_RATE (5000)
#define LEVELX_COLUMN_RATE_DEC (400)


class LevelManager {
public:
    LevelManager();
    virtual ~LevelManager() = default;

    static const std::array<std::string, LEVEL_MAX> files;
    static std::array<SDL_Texture*, LEVEL_MAX> textures;

    bool setLevel(const int level);
    bool next() { return setLevel(_level+1); }

    bool OnLoad(SDL_Renderer * renderer);
    void OnLoop();
    void OnRender(SDL_Renderer * renderer);
    void OnCleanUp();
    int getLevel() const;
    unsigned int getLevelDuration() const;

private:
    void setTarget(int x, int y, double zoom, Uint32 time, Uint32 delay);

    int _x, _targetX, _startX;
    int _y, _targetY, _startY;
    Uint32 _timeToTarget, _startTime;
    double _zoom, _targetZoom, _startZoom;

    int _level;
};


#endif /* LEVELMANAGER_H_ */
