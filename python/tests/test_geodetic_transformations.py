import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from geographic_coordinates import GeodeticTransformations


def test_geodetic_ecef_roundtrip():
    gt = GeodeticTransformations(44.53449107748528, 10.85770480278604, 121.0)
    x, y, z = gt.geodetic_to_ecef(44.53449107748528, 10.85770480278604, 121.0)
    phi, lam, h = gt.ecef_to_geodetic(x, y, z)
    assert math.isclose(phi, 44.53449107748528, rel_tol=1e-9)
    assert math.isclose(lam, 10.85770480278604, rel_tol=1e-9)
    assert math.isclose(h, 121.0, abs_tol=1e-6)


def test_enu_above_origin_has_zero_east_north_and_z_up_equals_height():
    # The ENU frame is anchored on the ellipsoid surface at (lat0, lon0, h=0),
    # so a point at (lat0, lon0, h0) sits directly above it at height h0.
    gt = GeodeticTransformations(44.53449107748528, 10.85770480278604, 121.0)
    x_east, y_north, z_up = gt.geodetic_to_enu(44.53449107748528, 10.85770480278604, 121.0)
    assert math.isclose(x_east, 0.0, abs_tol=1e-6)
    assert math.isclose(y_north, 0.0, abs_tol=1e-6)
    assert math.isclose(z_up, 121.0, abs_tol=1e-6)


def test_enu_ecef_roundtrip():
    gt = GeodeticTransformations(44.53449107748528, 10.85770480278604, 121.0)
    x_ecef, y_ecef, z_ecef = gt.geodetic_to_ecef(44.6, 10.9, 150.0)
    x_east, y_north, z_up = gt.ecef_to_enu(x_ecef, y_ecef, z_ecef)
    x2, y2, z2 = gt.enu_to_ecef(x_east, y_north, z_up)
    assert math.isclose(x2, x_ecef, rel_tol=1e-9)
    assert math.isclose(y2, y_ecef, rel_tol=1e-9)
    assert math.isclose(z2, z_ecef, rel_tol=1e-9)
