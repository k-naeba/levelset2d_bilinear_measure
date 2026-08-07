#pragma once

#include <array>
#include <vector>

#include "common_geometry/types.hpp"

namespace ns_ls2bm {

// Cell corner offsets, matching levelset2d_polygon's MarchCell corner
// order exactly (v00, v10, v11, v01): corner c's offset from a cell's
// local origin, in {0,1}^2.
//
//    3---------2
//    |         |
//    |         |
//    0---------1
inline constexpr std::array<std::array<int, 2>, 4> kCellCornerOffset = {{
    {0, 0}, {1, 0}, {1, 1}, {0, 1},
}};

// Bilinear interpolation of 4 corner values `v` (ordered per
// kCellCornerOffset) at local coordinates (x,y), each typically in [0,1]
// (values outside that range extrapolate the same multilinear function --
// there's nothing special about the unit range here).
inline double BilinearValue(const std::array<double, 4>& v, double x,
                              double y) {
  double result = 0.0;
  for (int c = 0; c < 4; ++c) {
    const auto& o = kCellCornerOffset[c];
    const double wx = o[0] ? x : 1.0 - x;
    const double wy = o[1] ? y : 1.0 - y;
    result += v[c] * wx * wy;
  }
  return result;
}

// Finds every parametric t in [0,1] where the bilinear interpolant of `v`
// (local cell coordinates, see BilinearValue) crosses zero along the line
// origin + t*dir. Works for any line direction: restricted to a line, the
// bilinear interpolant is a quadratic in t in general (linear only for an
// axis-aligned line with the other coordinate fixed), so this doesn't
// assume a closed form -- it's a dense-sampling + bisection root find
// instead, matching levelset3d_trilinear_measure's FindTrilinearCrossings.
inline std::vector<double> FindBilinearCrossings(const std::array<double, 4>& v,
                                                    const ns_cg::Vec2d& origin,
                                                    const ns_cg::Vec2d& dir,
                                                    int samples = 4000) {
  const auto f = [&](double t) {
    const ns_cg::Vec2d p = origin + t * dir;
    return BilinearValue(v, p.x(), p.y());
  };

  std::vector<double> roots;
  double prev_t = 0.0;
  double prev_f = f(0.0);
  if (prev_f == 0.0) roots.push_back(0.0);
  for (int i = 1; i <= samples; ++i) {
    const double t = static_cast<double>(i) / samples;
    const double fv = f(t);
    if ((prev_f < 0.0) != (fv < 0.0)) {
      double lo = prev_t, hi = t, flo = prev_f;
      for (int iter = 0; iter < 60; ++iter) {
        const double mid = 0.5 * (lo + hi);
        const double fm = f(mid);
        if ((fm < 0.0) == (flo < 0.0)) {
          lo = mid;
          flo = fm;
        } else {
          hi = mid;
        }
      }
      roots.push_back(0.5 * (lo + hi));
    }
    prev_t = t;
    prev_f = fv;
  }
  return roots;
}

}  // namespace ns_ls2bm
