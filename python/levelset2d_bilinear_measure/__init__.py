"""Python bindings for levelset2d_bilinear_measure: measurement primitives
for level-set-derived geometry (bilinear interpolation crossings,
polygon-segment ray intersections).

Deliberately scoped to exactly what the C++ library itself provides --
no dependency on any extraction algorithm (levelset2d_polygon).
Everything from the compiled extension is re-exported at the top level,
so ``import levelset2d_bilinear_measure as lbm; lbm.Edge2d(...)`` works
directly.
"""

from ._levelset2d_bilinear_measure import *  # noqa: F401,F403
from ._levelset2d_bilinear_measure import __doc__ as _doc  # noqa: F401

from . import plotting  # noqa: F401
from . import known_cases  # noqa: F401 -- imported after Edge2d is bound above

__all__ = [name for name in dir() if not name.startswith("_")]
