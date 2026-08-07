import numpy as np
import pytest

import levelset2d_bilinear_measure as lbm


def test_bilinear_value_at_corners_matches_input():
    v = [1, 2, 3, 4]
    for c, (ox, oy) in enumerate(lbm.CELL_CORNER_OFFSET):
        assert lbm.bilinear_value(v, ox, oy) == pytest.approx(v[c])


def test_axis_aligned_line_crosses_once_at_midpoint():
    v = [-1, 1, 1, -1]
    crossings = lbm.find_bilinear_crossings(v, np.array([0.0, 0.3]), np.array([1.0, 0.0]))
    assert len(crossings) == 1
    assert crossings[0] == pytest.approx(0.5, abs=1e-6)


def test_ray_segment_intersect_hit_and_miss():
    a, b = np.array([0.0, 0.0]), np.array([2.0, 0.0])
    hit = lbm.ray_segment_intersect(np.array([1.0, 5.0]), np.array([0.0, -1.0]), a, b)
    assert hit == pytest.approx(5.0)

    miss = lbm.ray_segment_intersect(np.array([5.0, 5.0]), np.array([0.0, -1.0]), a, b)
    assert miss is None


def test_edge2d_and_find_polygon_crossings():
    segments = [lbm.Edge2d(np.array([0.5, 0.0]), np.array([0.0, 0.5]))]
    hits = lbm.find_polygon_crossings(segments, np.array([0.0, 0.25]), np.array([1.0, 0.0]))
    assert len(hits) == 1
    assert hits[0] == pytest.approx(0.25)


@pytest.mark.parametrize("s,expected_mesh,expected_bilinear", [
    (0.3, 0.3 / (2 * 1.3), 1 - 1 / (1.3 ** 0.5)),
    (2.0, 2.0 / (2 * 3.0), 1 - 1 / (3.0 ** 0.5)),
])
def test_known_case1_matches_closed_form(s, expected_mesh, expected_bilinear):
    # case1 (single inside corner) has a closed-form solution along the
    # diagonal probe (corner 0 to corner 2): polygon crossing
    # t = s/(2(s+1)) (case5's own first-segment formula, since it's the
    # same segment), bilinear crossing t = 1 - 1/sqrt(s+1) (solving the
    # bilinear field's zero on the diagonal directly -- identical closed
    # form to levelset3d_trilinear_measure's case1, since restricted to
    # this diagonal the bilinear and trilinear fields reduce the same
    # way). Both always yield exactly one crossing -- unlike case5,
    # there's no s where they disagree on whether a crossing exists.
    v = [-s, 1.0, 1.0, 1.0]
    origin, direction = np.array([0.0, 0.0]), np.array([1.0, 1.0])
    segments = lbm.known_cases.case1_segments(v)
    mesh_ts = lbm.find_polygon_crossings(segments, origin, direction)
    bilinear_ts = lbm.find_bilinear_crossings(v, origin, direction)
    assert len(mesh_ts) == 1
    assert len(bilinear_ts) == 1
    assert mesh_ts[0] == pytest.approx(expected_mesh, abs=1e-6)
    assert bilinear_ts[0] == pytest.approx(expected_bilinear, abs=1e-6)


def test_known_case5_segments_below_threshold_matches_closed_form():
    # Below s=1, MarchCell's true-center-value disambiguation picks the
    # B-L/T-R pairing, whose diagonal-probe crossing is the closed form
    # t = s/(2(s+1)) and its mirror 1-t (the configuration is symmetric
    # under the 180 deg rotation swapping corner 0 <-> corner 2) --
    # coincides exactly with levelset3d_trilinear_measure's case5_mesh
    # reference value at the same s, since both reduce to the same 1D
    # algebra on their respective diagonals.
    s = 0.3
    v = [-s, 1.0, -s, 1.0]
    origin, direction = np.array([0.0, 0.0]), np.array([1.0, 1.0])
    segments = lbm.known_cases.case5_segments(v)
    ts = sorted(lbm.find_polygon_crossings(segments, origin, direction))
    assert len(ts) == 2
    assert ts[0] == pytest.approx(0.1154, abs=1e-3)
    assert ts[1] == pytest.approx(0.8846, abs=1e-3)


def test_known_case5_above_threshold_agrees_with_bilinear_on_the_diagonal():
    # s=2 is above the asymptotic-decider threshold (s=1): here
    # MarchCell's center-value disambiguation flips to the B-R/T-L
    # pairing, and both of those segments run parallel to the diagonal
    # probe, offset from it -- so, unlike levelset3d_trilinear_measure's
    # *fixed* triangulation (which always reports 2 crossings
    # regardless of s), the polygon crossing count tracks the bilinear
    # field's own topology change exactly: both report zero crossings.
    # This is the diagonal-probe analog of the existing
    # polygon_vs_bilinear_probe.html finding (that MarchCell never
    # disagrees on *whether* a crossing exists, only *where*) -- shown
    # here to also hold on the harder diagonal probe, not just the
    # horizontal one that page sweeps.
    v = [-2.0, 1.0, -2.0, 1.0]
    origin, direction = np.array([0.0, 0.0]), np.array([1.0, 1.0])
    segments = lbm.known_cases.case5_segments(v)
    mesh_ts = lbm.find_polygon_crossings(segments, origin, direction)
    bilinear_ts = lbm.find_bilinear_crossings(v, origin, direction)
    assert mesh_ts == []
    assert bilinear_ts == []


def test_plotting_helpers_produce_figures():
    segments = lbm.known_cases.case5_segments([-2.0, 1.0, -2.0, 1.0])
    fig = lbm.plotting.plot_segments(segments)
    assert len(fig.data) == 1

    fig2 = lbm.plotting.plot_probe_comparison(
        segments, np.array([0.0, 0.0]), np.array([1.0, 1.0]), [0.33, 0.67], [],
    )
    assert len(fig2.data) == 4
