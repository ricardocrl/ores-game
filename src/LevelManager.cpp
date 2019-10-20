/*
 * Level.cpp
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#include "LevelManager.h"

#include "AppUtils.hpp"

const std::array<std::string, LEVEL_MAX> LevelManager::files = {
        "res/Icon-level-01.png",
        "res/Icon-level-02.png",
        "res/Icon-level-03.png",
        "res/Icon-level-04.png",
        "res/Icon-level-05.png",
        "res/Icon-level-06.png",
        "res/Icon-level-07.png",
        "res/Icon-level-08.png",
        "res/Icon-level-09.png",
        "res/Icon-level-10.png"
};

std::array<SDL_Texture*, LEVEL_MAX> LevelManager::textures = { nullptr };

LevelManager::LevelManager() :
        _x(0), _targetX(0), _startX(0), _y(0), _targetY(0), _startY(0), _timeToTarget(0),
		_startTime(0), _zoom(0), _targetZoom(0), _startZoom(0), _level(1) {
};

bool LevelManager::setLevel(const int level) {
    if (_level != level) {
    	_level = level;
        _x = (APP_SIZE_W - LEVEL_ICON_W) / 2;
        _y = (APP_SIZE_H - LEVEL_ICON_H) / 2;
        _zoom = 1;
        setTarget(20, 20, 0.3, 1000, 300);
        return true;
    }
    return false;
}

bool LevelManager::OnLoad(SDL_Renderer * renderer) {
    for (size_t i = 0; i < LevelManager::textures.size(); ++i) {
        LevelManager::textures[i] = apputils::loadImg(LevelManager::files[i], renderer);
        if (!LevelManager::textures[i]) {
            std::cout << "Failed to load level texture from file " << LevelManager::files[i] << std::endl;
            return false;
        }
    }
    return true;
}

void LevelManager::OnLoop() {
    if (_timeToTarget != 0) {
        if (SDL_GetTicks() > _startTime) {
            double percent = double(SDL_GetTicks() - _startTime) / _timeToTarget;
            if (percent >= 1) {
                _timeToTarget = 0;
                _x = _targetX;
                _y = _targetY;
                _zoom = _targetZoom;
            } else {
                _x = _startX + ((_targetX - _startX) * percent);
                _y = _startY + ((_targetY - _startY) * percent);
                _zoom = _startZoom + ((_targetZoom - _startZoom) * percent);
            }
        } else {
            // Delay
        }
    }
}

void LevelManager::OnRender(SDL_Renderer * renderer) {
    SDL_Rect destRect = {_x, _y, static_cast<int>(LEVEL_ICON_W*_zoom), static_cast<int>(LEVEL_ICON_H*_zoom) };
    SDL_RenderCopy(renderer, LevelManager::textures[_level-1], nullptr, &destRect);
}

int LevelManager::getLevel() const {
    return _level;
}

unsigned int LevelManager::getLevelDuration() const {
	return (LEVEL1_COLUMN_RATE - (LEVELX_COLUMN_RATE_DEC * _level));
}

void LevelManager::setTarget(int x, int y, double zoom, Uint32 time, Uint32 delay) {
    _targetX = x;
    _targetY = y;
    _targetZoom = zoom;
    _startX = _x;
    _startY = _y;
    _startZoom = _zoom;
    _startTime = SDL_GetTicks() + delay;
    _timeToTarget = time;
}

void LevelManager::OnCleanUp() {
    // nothing to be done
}

