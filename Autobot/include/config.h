#ifndef _CONFIG_H
#define _CONFIG_H

/* config.h */
#define Create_Comport "COM3"

/* Remote controller TCP network port. */
#define CTRL_TCP_PORT 80

/* Remote Viewer TCP network port. */
#define VIEW_TCP_PORT 81

/* Uncomment DISABLE_CONTROLLER to disable remote controller
* and use local OpenCV window for controlling.
*/
//#define DISABLE_CONTROLLER

/* Uncomment USE_KINECT to use kinect.
* Comment to generate random solid color image in cv::Mat colorImg.
*/
//#define USE_KINECT

/* Uncomment USE_ROBOT to enable robot connectivity. */
//#define USE_ROBOT

#define INIT_DELAY_MS  200
#define START_DELAY_MS 200
#define STOP_DELAY_MS  200

#define BUMPER_THRESHOLD 12

#endif /* _CONFIG_H */