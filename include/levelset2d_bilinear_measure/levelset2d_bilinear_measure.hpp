#pragma once

// The measurement primitives themselves now live in common_geometry
// (ns_cg::BilinearValue/FindBilinearCrossings/RaySegmentIntersect/
// FindPolygonCrossings/kCellCornerOffset) -- this header just re-includes
// them so existing callers of levelset2d_bilinear_measure.hpp keep working.
#include "common_geometry/bilinear.hpp"
#include "common_geometry/polygon_intersection.hpp"
