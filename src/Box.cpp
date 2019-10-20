/*
 * Box.cpp
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#include "Box.h"

#include "AppUtils.hpp"
#include "AppTypes.h"

#include <memory>

std::string Box::boxFile[(int)BoxColor::COUNT] = {
        "res/item_box_yellow.png",
        "res/item_box_green.png",
        "res/item_box_orange.png",
        "res/item_box_purple.png",
        "res/item_box_gray.png",
};

std::function<void(std::shared_ptr<Box>, BoxLeanDir)> Box::leanedCbk = nullptr;
SDL_Texture * Box::boxTexture[(int)BoxColor::COUNT] = { nullptr };


Box::Box(BoxColor color) :
        x(0), y(0), angle(0), angle_goal(0), color(color), lastUpdate(0), leanRequest(BoxLeanDir::NONE), falling(false), sliding(
                false), blowing(false), w(0), v_x(0), v_y(0), a_x(0), a_y(0) {
}


bool Box::OnLoad(SDL_Renderer * renderer) {
    for (int i = 0; i < (int) BoxColor::COUNT; ++i) {
        Box::boxTexture[i] = apputils::loadImg(Box::boxFile[i], renderer);
        if (!Box::boxTexture[i]) {
            std::cout << "Failed to load box texture from file " << Box::boxFile[i] << std::endl;
            return false;
        }
    }
    return true;
}


void Box::OnCleanUp() {
    for (int i = 0; i < (int) BoxColor::COUNT; ++i) {
        if (Box::boxTexture[i]) {
            SDL_DestroyTexture(Box::boxTexture[i]);
        }
    }
}


void Box::subscribeLeanedCbk(const std::function<void(std::shared_ptr<Box>, BoxLeanDir)>& leanedCbk) {
    Box::leanedCbk = leanedCbk;
}


void Box::leanTo(BoxLeanDir leanDir) {
    leanRequest = leanDir;
    switch(leanRequest) {
        case BoxLeanDir::LEFT:
            angle_goal = -90;
            w = -wLean;
            break;
        case BoxLeanDir::RIGHT:
            angle_goal = 90;
            w = wLean;
            break;
        case BoxLeanDir::ORIGIN:
            angle_goal = 0;
            if (angle < angle_goal) {
                w = wLean;
            } else {
                w = -wLean;
            }
            break;
        default:
            break;
    }
}


void Box::fallFrom(Uint16 fallHeight) {
    if (blowing) {
        return;
    }
    falling = true;
    y += (-fallHeight);
    a_y = GRAVITY;
}


void Box::slideFrom(int from_x) {
    if (blowing) {
        return;
    }
    sliding = true;
    x += from_x;
    if (x > 0) {
        v_x = -vSlide;
    } else {
        v_x = vSlide;
    }
}


void Box::resetAngle() {
    angle = 0;
    angle_goal = 0;
    leanRequest = BoxLeanDir::NONE;
}


void Box::OnLoop() {
    auto time = SDL_GetTicks();
    if (time < lastUpdate + updatePosRate) {
        return;
    }
    lastUpdate += updatePosRate;

    v_x += (a_x * deltaT);
    x += (v_x * deltaT);
    v_y += (a_y * deltaT);
    y += (v_y * deltaT);
    angle += (w * deltaT);

    if (sliding && x * v_x > 0) {
        // we crossed the stop point, 0
        sliding = false;
        x = 0;
        v_x = 0;
        a_x = 0;
    }
    if (falling && y * v_y > 0) {
        // we crossed the stop point, 0
        falling = false;
        y = 0;
        v_y = 0;
        a_y = 0;
    }
    if (leanRequest != BoxLeanDir::NONE) {
        if ((angle_goal - angle) * w < 0) {
            // crossed the goal
            leanRequest = BoxLeanDir::NONE;
            w = 0;
            angle = angle_goal;
            if (Box::leanedCbk) {
                Box::leanedCbk(shared_from_this(), leanRequest);
            }
        }
    }
    if (blowing && x > APP_SIZE_W) {
        blowing = false;
        a_y = 0;
        v_x = 0;
        v_y = 0;
    }
}


void Box::OnRender(SDL_Renderer * renderer, int x, int y) {
    SDL_Point axis;
    SDL_Rect destRect = { static_cast<int>(x + this->x), static_cast<int>(y + this->y), BOX_SIZE, BOX_SIZE };
    if (angle < 0) {
        axis.x = 0;
        axis.y = BOX_SIZE;
    } else {
        axis.x = BOX_SIZE;
        axis.y = BOX_SIZE;
    }
    if (angle != 0) {
        SDL_RenderCopyEx(renderer, Box::boxTexture[(int) color], nullptr, &destRect, angle, &axis, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopy(renderer, Box::boxTexture[(int) color], nullptr, &destRect);
    }
}

void Box::blowFrom(const int start_x, const int start_y) {
    blowing = true;
    falling = sliding = false;
    x = start_x;
    y = start_y;
    v_x = ((rand()%2)*2 - 1) * (20 + rand()%20);
    v_y = -50-rand()%40;
    a_y = GRAVITY*2;
}

bool Box::isBlowing() const {
    return blowing;
}

bool Box::isFalling() const {
    return falling;
}

bool Box::isSliding() const {
    return sliding;
}
