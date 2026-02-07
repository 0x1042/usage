#include "gtest/gtest.h"
#include "lib/log.h"
#include "s2/s1angle.h"
#include "s2/s2cell_id.h"
#include "s2/s2latlng.h"

TEST(s2, latlng) {
    const auto& p1 = S2LatLng::FromDegrees(39.9042, 116.4074);
    const auto& p2 = S2LatLng::FromDegrees(31.2304, 121.4737);

    S1Angle distance_rad = p1.GetDistance(p2);
    double distance_km = distance_rad.radians() * 6371.0;
    INFO("distance is {}", distance_km);
}

TEST(s2, encode) {
    const auto& p1 = S2LatLng::FromDegrees(39.9042, 116.4074);
    auto cell_id = S2CellId(p1);

    for (int i = 5; i <= 30; i += 5) {
        auto tmp = cell_id.parent(i);
        INFO("level is {} {} {} {}", i, tmp.level(), tmp.id(), tmp.ToString());
    }
}