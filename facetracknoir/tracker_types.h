#ifndef __TRACKER_TYPES_H__
#define __TRACKER_TYPES_H__

#include "ftnoir_tracker_base/ftnoir_tracker_types.h"

struct Camera {
public:
    double axes[6];

    Camera() : axes {0,0,0, 0,0,0 } {}
};

Camera operator-(const Camera& A, const Camera& B); // get new pose with respect to reference pose B
Camera operator+(const Camera& A, const Camera& B); // get new pose with respect to reference pose B^-1

#endif //__TRACKER_TYPES_H__
