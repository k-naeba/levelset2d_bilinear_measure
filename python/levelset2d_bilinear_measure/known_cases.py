"""Hand-reconstructed segment lists for a handful of classic
marching-squares cases -- the single-corner case (case 1) and the
face-saddle case (case 5) -- built directly from the corner-value
interpolation formula, not by calling any extraction library's
marching squares implementation (this project deliberately doesn't
depend on one).

case5_segments reproduces, exactly, levelset2d_polygon's own
MarchCell case-5 branch: unlike the 3D marching-cubes saddle (which
always emits the same fixed, disconnected triangulation regardless of
corner magnitude), MarchCell disambiguates this saddle using the true
cell-center value (the mean of the 4 corners), so the segment pairing
this function returns depends on `v`, not just on which corners are
inside. case1 is the unambiguous single-corner baseline used to
contrast against it: with only one inside corner there's no
disambiguation choice to make at all.

Corner ordering and edge interpolation both match CELL_CORNER_OFFSET's
convention exactly, so a segment list built here is directly
comparable to one built via levelset2d_polygon's real MarchCell.
"""

from __future__ import annotations

import numpy as np

from . import Edge2d

# Corner positions, matching CELL_CORNER_OFFSET exactly.
_CORNERS = [
    np.array([0.0, 0.0]), np.array([1.0, 0.0]),
    np.array([1.0, 1.0]), np.array([0.0, 1.0]),
]


def _interpolate(v, a, b):
    """The zero-crossing point along edge (corner a, corner b)."""
    va, vb = v[a], v[b]
    t = va / (va - vb)
    return _CORNERS[a] + t * (_CORNERS[b] - _CORNERS[a])


def case1_segments(v) -> list:
    """corner 0 inside; corners 1-3 outside -- one solid region, no
    saddle. The single segment capping corner 0 is exactly
    case5_segments's first segment when its center-value branch picks
    the same pairing (same two edges, same v0/v1/v3), so the two are
    directly comparable at matching s."""
    p_b = _interpolate(v, 0, 1)
    p_l = _interpolate(v, 0, 3)
    return [Edge2d(p_b, p_l)]


def case5_segments(v) -> list:
    """corners 0, 2 inside; 1, 3 outside. Matches
    levelset2d_polygon's MarchCell case 5 exactly: the cell-center
    value (mean of all 4 corners) picks B-R / T-L when negative, or
    B-L / T-R when positive."""
    p_b = _interpolate(v, 0, 1)
    p_r = _interpolate(v, 1, 2)
    p_t = _interpolate(v, 3, 2)
    p_l = _interpolate(v, 0, 3)
    center = sum(v) / 4.0
    if center < 0.0:
        return [Edge2d(p_b, p_r), Edge2d(p_t, p_l)]
    return [Edge2d(p_b, p_l), Edge2d(p_t, p_r)]
