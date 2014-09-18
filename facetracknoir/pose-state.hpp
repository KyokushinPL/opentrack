#pragma once

#include <vector>

#include "facetracknoir/main-settings.hpp"
#include "facetracknoir/mapping.hpp"
#include "ftnoir_tracker_base/ftnoir_tracker_types.h"

class PoseState {
public:
    Mapping axes[6];
    PoseState(std::vector<axis_opts*> opts) :
        axes {
            Mapping("tx","tx_alt", 100, 100, 100, 100, opts[TX]),
            Mapping("ty","ty_alt", 100, 100, 100, 100, opts[TY]),
            Mapping("tz","tz_alt", 100, 100, 100, 100, opts[TZ]),
            Mapping("rx", "rx_alt", 180, 180, 180, 180, opts[Yaw]),
            Mapping("ry", "ry_alt", 90, 90, 90, 90, opts[Pitch]),
            Mapping("rz", "rz_alt", 180, 180, 180, 180, opts[Roll])
        }
    {}
};