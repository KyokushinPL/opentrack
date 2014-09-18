/* Copyright (c) 2012-2014 Stanislaw Halik <sthalik@misaki.pl>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */

/*
 * this file appeared originally in facetracknoir, was rewritten completely
 * following opentrack fork.
 *
 * originally written by Wim Vriend.
 */

#include "tracker.h"
#include "facetracknoir.h"
#include <opencv2/core/core.hpp>
#include <cmath>
#include <algorithm>

#if defined(_WIN32)
#   include <windows.h>
#endif

Work::Work(const PoseState& pose) :
    pose(pose),
    should_quit(false),
    do_center(false),
    enabled(true)
{
}

Work::~Work()
{
    should_quit = true;
    wait();
}

static void get_curve(double pos, double& out, Mapping& axis) {
    bool altp = (pos < 0) && axis.opts.altp;
    axis.curve.setTrackingActive( !altp );
    axis.curveAlt.setTrackingActive( altp );    
    auto& fc = altp ? axis.curveAlt : axis.curve;
    out = (axis.opts.invert ? -1 : 1) * fc.getValue(pos);
    
    out += axis.opts.zero;
}

static void t_compensate(double* input, double* output, bool rz)
{
    const auto H = input[Yaw] * M_PI / -180;
    const auto P = input[Pitch] * M_PI / -180;
    const auto B = input[Roll] * M_PI / 180;

    const auto cosH = cos(H);
    const auto sinH = sin(H);
    const auto cosP = cos(P);
    const auto sinP = sin(P);
    const auto cosB = cos(B);
    const auto sinB = sin(B);

    double foo[] = {
        cosH * cosB - sinH * sinP * sinB,
        - sinB * cosP,
        sinH * cosB + cosH * sinP * sinB,
        cosH * sinB + sinH * sinP * cosB,
        cosB * cosP,
        sinB * sinH - cosH * sinP * cosB,
        - sinH * cosP,
        - sinP,
        cosH * cosP,
    };

    const cv::Mat rmat(3, 3, CV_64F, foo);
    const cv::Mat tvec(3, 1, CV_64F, input);
    cv::Mat ret = rmat * tvec;

    const int max = !rz ? 3 : 2;

    for (int i = 0; i < max; i++)
        output[i] = ret.at<double>(i);
}

void Work::run() {
    Camera center_camera;

    double newpose[6] = {0};
    int sleep_ms = 15;

    if (Libraries->pTracker)
        sleep_ms = std::min(sleep_ms, 1000 / Libraries->pTracker->preferredHz());

    if (Libraries->pSecondTracker)
        sleep_ms = std::min(sleep_ms, 1000 / Libraries->pSecondTracker->preferredHz());

    qDebug() << "tracker Hz:" << 1000 / sleep_ms;

#if defined(_WIN32)
    (void) timeBeginPeriod(1);
#endif

    for (;;)
    {
        t.start();

        if (should_quit)
            break;

        if (Libraries->pSecondTracker)
            Libraries->pSecondTracker->GetHeadPoseData(newpose);

        if (Libraries->pTracker)
            Libraries->pTracker->GetHeadPoseData(newpose);

        {
            QMutexLocker foo(&mtx);

            for (int i = 0; i < 6; i++)
            {
                auto& axis = pose.axes[i];
                raw_6dof.axes[i] = newpose[i];
                int k = axis.opts.src;
                if (k < 0 || k >= 6)
                    continue;
                axis.headPos = newpose[k];
            }

            if (do_center)  {
                for (int i = 0; i < 6; i++)
                    center_camera.axes[i] = pose.axes[i].headPos;

                do_center = false;

                if (Libraries->pFilter)
                    Libraries->pFilter->reset();
            }

            Camera input_pose;

            if (enabled)
            {
                Camera target_camera;
                
                for (int i = 0; i < 6; i++)
                    target_camera.axes[i] = pose.axes[i].headPos;

                input_pose = target_camera - center_camera;
            }
            
            Camera filtered_pose;

            if (Libraries->pFilter) {
                Libraries->pFilter->FilterHeadPoseData(input_pose.axes, filtered_pose.axes);
            } else {
                filtered_pose = input_pose;
            }

            for (int i = 0; i < 6; i++) {
                get_curve(filtered_pose.axes[i], output_camera.axes[i], pose.axes[i]);
            }

            if (mainApp->s.tcomp_p)
                t_compensate(output_camera.axes, output_camera.axes, mainApp->s.tcomp_tz);

            if (Libraries->pProtocol) {
                Libraries->pProtocol->sendHeadposeToGame( output_camera.axes );
            }
        }

        const long q = std::max(0L, sleep_ms * 1000L - std::max(0L, t.elapsed()));

        usleep(q);
    }
    
#if defined(_WIN32)
    (void) timeEndPeriod(1);
#endif

    for (int i = 0; i < 6; i++)
    {
        pose.axes[i].curve.setTrackingActive(false);
        pose.axes[i].curveAlt.setTrackingActive(false);
    }
}

void Work::getHeadPose( double *data ) {
    QMutexLocker foo(&mtx);
    for (int i = 0; i < 6; i++)
        data[i] = raw_6dof.axes[i];
}

void Work::getOutputHeadPose( double *data ) {
    QMutexLocker foo(&mtx);
    for (int i = 0; i < 6; i++)
        data[i] = output_camera.axes[i];
}
