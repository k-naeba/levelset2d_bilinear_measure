#include <gtest/gtest.h>

#include "levelset2d_bilinear_measure/polygon_intersection.hpp"

namespace ns_ls2bm {
namespace {

using ns_cg::Edge2d;
using ns_cg::Vec2d;

TEST(RaySegmentIntersectTest, HitsSegmentMidpointFromTheSide) {
  const Vec2d a(0, 0), b(2, 0);
  const auto t = RaySegmentIntersect(Vec2d(1.0, 5.0), Vec2d(0, -1), a, b);
  ASSERT_TRUE(t.has_value());
  EXPECT_NEAR(*t, 5.0, 1e-9);
}

TEST(RaySegmentIntersectTest, MissesOutsideSegmentBounds) {
  const Vec2d a(0, 0), b(2, 0);
  const auto t = RaySegmentIntersect(Vec2d(5.0, 5.0), Vec2d(0, -1), a, b);
  EXPECT_FALSE(t.has_value());
}

TEST(RaySegmentIntersectTest, ParallelToSegmentMisses) {
  const Vec2d a(0, 0), b(2, 0);
  const auto t = RaySegmentIntersect(Vec2d(0.5, 1.0), Vec2d(1, 0), a, b);
  EXPECT_FALSE(t.has_value());
}

TEST(FindPolygonCrossingsTest, HorizontalLineCrossesOneSegmentOnce) {
  // A MarchCell-style case-1 output: a single diagonal segment from the
  // bottom edge to the left edge, at y in [0, 0.5]. A horizontal probe at
  // y=0.25 crosses it at x=0.25 (halfway along both the segment and,
  // here, the probe itself).
  const std::vector<Edge2d> segments = {Edge2d(Vec2d(0.5, 0), Vec2d(0, 0.5))};
  const std::vector<double> hits =
      FindPolygonCrossings(segments, Vec2d(0.0, 0.25), Vec2d(1.0, 0.0));
  ASSERT_EQ(hits.size(), 1u);
  EXPECT_NEAR(hits[0], 0.25, 1e-9);
}

TEST(FindPolygonCrossingsTest, LineMissingAllSegmentsHasNoCrossings) {
  const std::vector<Edge2d> segments = {Edge2d(Vec2d(0.5, 0), Vec2d(0, 0.5))};
  const std::vector<double> hits =
      FindPolygonCrossings(segments, Vec2d(0.0, 10.0), Vec2d(1.0, 0.0));
  EXPECT_TRUE(hits.empty());
}

}  // namespace
}  // namespace ns_ls2bm
