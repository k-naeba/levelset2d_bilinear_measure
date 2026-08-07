#include <gtest/gtest.h>

#include "levelset2d_bilinear_measure/bilinear.hpp"

namespace ns_ls2bm {
namespace {

using ns_cg::Vec2d;

TEST(BilinearTest, ValueAtEachCornerMatchesThatCornersInput) {
  const std::array<double, 4> v = {1, 2, 3, 4};
  for (int c = 0; c < 4; ++c) {
    const auto& o = kCellCornerOffset[c];
    EXPECT_DOUBLE_EQ(BilinearValue(v, o[0], o[1]), v[c]) << "corner " << c;
  }
}

TEST(BilinearTest, ValueAtCenterIsAverageOfAllCorners) {
  const std::array<double, 4> v = {1, 2, 3, 4};
  double expected = 0.0;
  for (double x : v) expected += x;
  expected /= 4.0;
  EXPECT_DOUBLE_EQ(BilinearValue(v, 0.5, 0.5), expected);
}

TEST(BilinearTest, AxisAlignedLineCrossesOnceAtTheMidpoint) {
  // x=0 edge negative, x=1 edge positive: bilinear along any fixed-y line
  // is then an exact linear ramp from -1 to +1.
  const std::array<double, 4> v = {-1, 1, 1, -1};
  const std::vector<double> crossings =
      FindBilinearCrossings(v, Vec2d(0.0, 0.3), Vec2d(1.0, 0.0));
  ASSERT_EQ(crossings.size(), 1u);
  EXPECT_NEAR(crossings[0], 0.5, 1e-6);
}

TEST(BilinearTest, DiagonalSaddleBelowThresholdGivesTwoCrossings) {
  // Case-5-style saddle (corners 0,2 inside), inside magnitude well below
  // the outside magnitude: along the diagonal probe (corner 0 to corner
  // 2), the bilinear value is v(t) = -s + (2s+2)*t*(1-t), maximized at
  // t=0.5 with value 0.5*(1-s) -- positive for s<1, so the field pinches
  // off between the two inside corners (2 crossings), same threshold
  // structure as the 3D trilinear case.
  const std::array<double, 4> v = {-0.5, 1.0, -0.5, 1.0};
  const std::vector<double> crossings =
      FindBilinearCrossings(v, Vec2d(0, 0), Vec2d(1, 1));
  EXPECT_EQ(crossings.size(), 2u);
}

TEST(BilinearTest, DiagonalSaddleAboveThresholdGivesNoCrossings) {
  // Same saddle, inside magnitude well above threshold: 0.5*(1-s) < 0 for
  // s=2, so the two inside corners are connected the whole way.
  const std::array<double, 4> v = {-2.0, 1.0, -2.0, 1.0};
  const std::vector<double> crossings =
      FindBilinearCrossings(v, Vec2d(0, 0), Vec2d(1, 1));
  EXPECT_TRUE(crossings.empty());
}

}  // namespace
}  // namespace ns_ls2bm
