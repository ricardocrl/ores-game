/*
 * Box.h
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#ifndef BOX_H_
#define BOX_H_

#include "SDL2/SDL.h"

#include "AppTypes.h"

#include <functional>
#include <memory>

class Box : public std::enable_shared_from_this<Box> {
public:
    Box(BoxColor color);
    virtual ~Box() = default;

    static bool OnLoad(SDL_Renderer * renderer);
    void OnLoop();
    void OnRender(SDL_Renderer * renderer, int x = 0, int y = 0);
    static void OnCleanUp();

    static void subscribeLeanedCbk(const std::function<void(std::shared_ptr<Box>, BoxLeanDir)>& leanedCbk);

    void leanTo(BoxLeanDir leanDir);

    void fallFrom(Uint16 fallHeight);
    void slideFrom(int from_x);
    void blowFrom(const int start_x, const int start_y);

    void resetAngle();

    void setValid(bool valid);
    bool isBlowing() const;
    bool isFalling() const;
    bool isSliding() const;

public:
    double x;
    double y;
    double angle;
    double angle_goal;
    const BoxColor color;

private:
    const Uint16 updatePosRate = 20; // Update every xx ms
    const double deltaT = (updatePosRate/1000.0);
    const double wLean = 120;
    const double vSlide = 300;

    Uint32 lastUpdate;
    BoxLeanDir leanRequest;
    bool falling, sliding, blowing;
    double w;
    double v_x, v_y;
    double a_x, a_y;

    static std::function<void(std::shared_ptr<Box>, BoxLeanDir)> leanedCbk;
    static SDL_Texture * boxTexture[(int)BoxColor::COUNT];
    static std::string boxFile[(int)BoxColor::COUNT];
};


#endif /* BOX_H_ */
