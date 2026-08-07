#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

#include "common_geometry/edge.hpp"
#include "common_geometry/types.hpp"

namespace ns_ls2bm {

// 2D cross product (scalar): a.x*b.y - a.y*b.x.
inline double Cross2d(const ns_cg::Vec2d& a, const ns_cg::Vec2d& b) {
  return a.x() * b.y() - a.y() * b.x();
}

// Ray-segment intersection: returns the ray parameter t (origin + t*dir)
// where it crosses segment [a,b], or nullopt if it doesn't (including the
// parallel case and the case where the crossing falls outside [a,b]).
// The 2D analog of RayTriangleIntersect (Moller-Trumbore) --
// levelset2d_polygon's MarchCell segments play the role Mesh3d's
// triangles play in 3D.
inline std::optional<double> RaySegmentIntersect(const ns_cg::Vec2d& origin,
                                                    const ns_cg::Vec2d& dir,
                                                    const ns_cg::Vec2d& a,
                                                    const ns_cg::Vec2d& b) {
  constexpr double kEps = 1e-12;
  const ns_cg::Vec2d s = b - a;
  const double denom = Cross2d(dir, s);
  if (std::abs(denom) < kEps) return std::nullopt;

  const ns_cg::Vec2d qp = a - origin;
  const double t = Cross2d(qp, s) / denom;
  const double u = Cross2d(qp, dir) / denom;
  if (u < 0.0 || u > 1.0) return std::nullopt;

  return t;
}

// Finds every t in [0,1] where the segment origin + t*dir crosses one of
// `segments`, sorted ascending. A line grazing an endpoint shared by two
// segments may be reported twice (once per segment); callers wanting a
// deduplicated crossing count should account for that (e.g. by merging
// hits closer together than a small epsilon). The 2D analog of
// FindMeshCrossings -- `segments` plays the role a Mesh3d's triangle list
// plays in 3D (e.g. levelset2d_polygon's MarchCell output, before it's
// linked into closed loops).
inline std::vector<double> FindPolygonCrossings(
    const std::vector<ns_cg::Edge2d>& segments, const ns_cg::Vec2d& origin,
    const ns_cg::Vec2d& dir) {
  std::vector<double> hits;
  for (const auto& seg : segments) {
    const auto t =
        RaySegmentIntersect(origin, dir, seg.GetStart(), seg.GetEnd());
    if (t.has_value() && *t >= -1e-9 && *t <= 1.0 + 1e-9) hits.push_back(*t);
  }
  std::sort(hits.begin(), hits.end());
  return hits;
}

}  // namespace ns_ls2bm
