"""Plotly helpers for visualizing levelset2d_bilinear_measure results in
Jupyter.

Scoped to what this project measures: a list of segments, a probe line,
and the crossing points found along it (by either method). Nothing here
knows how the segments were produced (marching squares, hand-built, or
otherwise).
"""

from __future__ import annotations

import numpy as np
import plotly.graph_objects as go


def segments_trace(segments, **kwargs):
    """A go.Scatter trace drawing each segment as its own two-point line
    (segments are not assumed to be linked into a single polyline)."""
    xs, ys = [], []
    for seg in segments:
        xs += [seg.start[0], seg.end[0], None]
        ys += [seg.start[1], seg.end[1], None]
    kwargs.setdefault("mode", "lines")
    kwargs.setdefault("line", dict(color="#4488ff", width=3))
    kwargs.setdefault("name", "segments")
    return go.Scatter(x=xs, y=ys, **kwargs)


def plot_segments(segments, **kwargs) -> go.Figure:
    """A standalone figure showing one segment list, with an equal-aspect
    scene."""
    fig = go.Figure(data=[segments_trace(segments, **kwargs)])
    fig.update_yaxes(scaleanchor="x", scaleratio=1)
    return fig


def probe_line_trace(origin, direction, t_range=(0.0, 1.0), **kwargs):
    """A go.Scatter trace for the segment origin + t*direction, t in
    t_range."""
    origin = np.asarray(origin, dtype=float)
    direction = np.asarray(direction, dtype=float)
    p0 = origin + t_range[0] * direction
    p1 = origin + t_range[1] * direction
    kwargs.setdefault("mode", "lines")
    kwargs.setdefault("line", dict(color="black", width=2))
    kwargs.setdefault("name", "probe line")
    return go.Scatter(x=[p0[0], p1[0]], y=[p0[1], p1[1]], **kwargs)


def crossing_points_trace(origin, direction, ts, **kwargs):
    """A go.Scatter trace with a marker at origin + t*direction for each t
    in `ts` -- e.g. the output of find_polygon_crossings /
    find_bilinear_crossings."""
    origin = np.asarray(origin, dtype=float)
    direction = np.asarray(direction, dtype=float)
    points = np.array([origin + t * direction for t in ts]) if len(ts) else np.zeros((0, 2))
    kwargs.setdefault("mode", "markers")
    kwargs.setdefault("marker", dict(size=9, color="red"))
    kwargs.setdefault("name", "crossings")
    return go.Scatter(
        x=points[:, 0] if len(ts) else [],
        y=points[:, 1] if len(ts) else [],
        **kwargs,
    )


def plot_probe_comparison(segments, origin, direction, polygon_ts, bilinear_ts,
                           t_range=(0.0, 1.0)) -> go.Figure:
    """A figure overlaying a segment list, its probe line, and both
    methods' crossing points (polygon crossings in red, bilinear
    crossings in green)."""
    fig = go.Figure(data=[
        segments_trace(segments),
        probe_line_trace(origin, direction, t_range),
        crossing_points_trace(origin, direction, polygon_ts,
                               name="polygon crossings",
                               marker=dict(size=10, color="red", symbol="circle")),
        crossing_points_trace(origin, direction, bilinear_ts,
                               name="bilinear crossings",
                               marker=dict(size=10, color="green", symbol="diamond")),
    ])
    fig.update_yaxes(scaleanchor="x", scaleratio=1)
    return fig
