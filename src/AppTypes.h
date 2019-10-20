/*
 * AppTypes.h
 *
 *  Created on: 23/04/2018
 *      Author: roliveira
 */

#ifndef APPTYPES_H_
#define APPTYPES_H_

enum class BoxLeanDir {
    LEFT = 0,
    RIGHT,
    ORIGIN,
    NONE
};

enum class BoxColor {
    YELLOW = 0,
    GREEN,
    ORANGE,
    PURPLE,
    GRAY,
    COUNT
};

#define APP_SIZE_W (640)
#define APP_SIZE_H (480)

#define APP_PLAY_OFFSET_X (0)
#define APP_PLAY_OFFSET_Y (-70)

#define BOX_SIZE (30)
#define BOX_QUEUE_HEIGHT (9)
#define BOX_COUNT_MAX (17)
#define BOX_COUNT_INIT (9)
#define BOX_SAME_COLOR_PROB (20) // 0 to 100

#define GRAVITY (300)

#define EARTHQKE_MIN_TIME (15000)

#define USR_EVT_ADDQUEUE (0)
#define USR_EVT_EARTHQKE (1)
#define USR_EVT_NEXT_LEVEL (2)

#define LEVEL_INTERVAL (25000)

#endif /* APPTYPES_H_ */
