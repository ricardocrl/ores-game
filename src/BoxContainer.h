/*
 * BoxContainer.h
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#ifndef BOXCONTAINER_H_
#define BOXCONTAINER_H_

#include "AppTypes.h"

#include "SDL2/SDL.h"

#include <list>
#include <vector>
#include <memory>

class Box;

class BoxContainer {
private:
    typedef std::list<std::shared_ptr<Box>> Queue;

public:
    BoxContainer();

    static std::string backgroundFile;
    static SDL_Texture * backgroundTexture;

    /**
     * Loads all necessary textures, including the ones controlled by
     * Box class, which are loaded once for any number of existing
     * boxes
     */
    bool OnLoad(SDL_Renderer * renderer);

    /**
     * Frees BoxContainer dynamic data and also from all Box's that it
     * manages
     */
    void OnCleanUp();

    /**
     * Renders background and all boxes.
     * Note that BoxContainer decides where boxes are rendered, hence
     * the (x,y) arguments when calling boxes' OnRender()
     * Box's have take this input and add their own relative position
     * Box's fall and rotate, giving them a pose, but relative to an
     * origin which belongs to scope of BoxContainer
     */
    void OnRender(SDL_Renderer * renderer);

    /**
     * Processes:
     * - Box's OnLoop
     * - Callbacks from Boxes that lean over
     * - Earthquake OnLoop
     */
    void OnLoop();
    /**
     * Stores 'earthquake' details to perform the earthquake during
     * earthquakeLoop()
     * magn  : magnitude in pixels of oscillation around the original position
     * speed : shaking speed that tries to be in pixels/sec but in practice it is
     *         somehow not true. Currently these values are to be seen without metrics.
     * duration : After 'duration' the earthquake stops and the world reset its position
     */
    void shake(int magn, double speed, int duration);

    /**
     * Returns Box's position in terms of <Queue ID, Box ID>
     * Queue ID starts from left to right, starting in 0.
     * Box ID starts from bottom up, starting in 0.
     */
    std::pair<int, int> getBoxQueuePos(int mouse_x, int mouse_y);

    /**
     * Returns a vector with valid neighbours from up, down, left, right
     * Each neighbor is a pair of <queue ID, box ID>
     */
    std::vector<std::pair<int,int>> getNeighbours(int queueIdx, int boxIdx);

    /**
     * This is the starting point, from which we will check all boxes around and finally
     * perform the destruction
     */
    void blowBox(int queueIdx, int boxIdx);

    /**
     * Request boxes to lean. This does not mean they will move to a different queue
     * That is tracked by the callback updates when reaching the max leaning angle (90 deg)
     * These call-backs are processed by leanedBoxProcess().
     */
    void leanBoxes(BoxLeanDir leanDir);

    /**
     * Adds a queue to the box container
     * Returns false if max queues was exceeded -> game over
     */
    bool addQueue(const bool init = false);

    /**
     * Add 'n' queues to the box container
     * Returns false if max queues was exceeded -> game over
     */
    bool addQueue(const int n, bool init = false);

    /**
     * Reset queues for next level
     * It uses BOX_COUNT_INIT for the initial amount of queues
     */
    void resetQueues();

private:
    /**
     * Recursive function that calls itself for all neighbors (left, right, up, down)
     * if they are still marked 'valid'. Each Box that is contiguously of the same color
     * as 'color' will be marked as not valid.
     * This is done before the actual removal of all boxes that shall be blown to avoid
     * tricky management of list iterators, while browsing the neighbors.
     * The 'valid' attribute of Box also allows to avoid infinite loops when searching.
     */
    void markForBlowAround(BoxColor color, int queue, int box);

    /**
     * Actual removal of boxes, called within blowBox implementation. See also 'markForBlowAround'
     */
    void removeBlowingBoxes();

    /**
     * Saves cbks to be processed in one go, after OnLoop
     */
    void leanedBoxSave_cbk(std::shared_ptr<Box> box, BoxLeanDir direction) {
        leanedBoxes.push_back({box, direction});
    }

    /**
     * Process a callback for a box that leaned too much and has to be moved to a
     * new queue.
     */
    void leanedBoxProcess(std::shared_ptr<Box> box, BoxLeanDir direction);

    /**
     * Returns a pointer to the Box, given its Queue-Box index.
     */
    std::shared_ptr<Box> getBox(Uint8 queue, Uint8 box);

    /**
     * Returns a pair with the screen coordinates X and Y for a given box at position
     * queueIdx and boxIdx. If 'shake' the earthquake offset is added. This is usually desired
     * but can be disabled for example, when the box is blowing.
     */
    std::pair<int, int> getXYFromListIndexes(const int queueIdx, const int boxIdx, const bool shake = true);

    /**
     * Performs earthquake calculations
     */
    void earthquakeLoop();

    const Uint16 updatePosRate = 20; // Update every xx ms
    const double deltaT = (updatePosRate/1000.0);
    Uint32 lastUpdate;

    std::list<Queue> boxQueues;
    std::list<std::pair<std::shared_ptr<Box>, BoxLeanDir>> leanedBoxes;

    double shakeSpeed;
    int shakeGoal;
    Uint32 shakeDuration;
    Uint32 shakeStart;
    double offset_x;

    static std::vector<std::shared_ptr<Box>> blowingBoxes; // Used to keep alive Boxes
};

#endif /* BOXCONTAINER_H_ */
