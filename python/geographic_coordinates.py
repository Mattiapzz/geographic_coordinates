"""Thin ctypes wrapper around the geographic_coordinates C API."""

import ctypes
import os
import sys


def _library_name() -> str:
    if sys.platform.startswith("win"):
        return "geographic_coordinates_c_api.dll"
    if sys.platform == "darwin":
        return "libgeographic_coordinates_c_api.dylib"
    return "libgeographic_coordinates_c_api.so"


def _load_library() -> ctypes.CDLL:
    here = os.path.dirname(os.path.abspath(__file__))
    candidates = [
        os.path.join(here, _library_name()),
        os.path.join(here, "..", "..", "lib", _library_name()),
    ]
    for path in candidates:
        if os.path.exists(path):
            return ctypes.CDLL(path)
    # Fall back to the system loader (e.g. library installed on PATH/LD_LIBRARY_PATH).
    return ctypes.CDLL(_library_name())


_lib = _load_library()

_GEOGRAPHIC_COORDINATES_OK = 0


class GeographicCoordinatesError(RuntimeError):
    """Raised when a call into the geographic_coordinates C API fails."""


class _Handle(ctypes.Structure):
    pass


_HandlePtr = ctypes.POINTER(_Handle)

_lib.geographic_coordinates_create.argtypes = [
    ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.POINTER(_HandlePtr)
]
_lib.geographic_coordinates_create.restype = ctypes.c_int

_lib.geographic_coordinates_free.argtypes = [_HandlePtr]
_lib.geographic_coordinates_free.restype = None

_lib.geographic_coordinates_set_origin.argtypes = [
    _HandlePtr, ctypes.c_double, ctypes.c_double, ctypes.c_double
]
_lib.geographic_coordinates_set_origin.restype = ctypes.c_int

_TRIPLE_TO_TRIPLE_ARGTYPES = [
    _HandlePtr,
    ctypes.c_double, ctypes.c_double, ctypes.c_double,
    ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
]

for _name in (
    "geographic_coordinates_geodetic_to_ecef",
    "geographic_coordinates_ecef_to_geodetic",
    "geographic_coordinates_geodetic_to_enu",
    "geographic_coordinates_enu_to_geodetic",
    "geographic_coordinates_ecef_to_enu",
    "geographic_coordinates_enu_to_ecef",
):
    _fn = getattr(_lib, _name)
    _fn.argtypes = _TRIPLE_TO_TRIPLE_ARGTYPES
    _fn.restype = ctypes.c_int


def _check(status: int) -> None:
    if status != _GEOGRAPHIC_COORDINATES_OK:
        raise GeographicCoordinatesError("geographic_coordinates C API call failed")


class GeodeticTransformations:
    """Conversions between geodetic, ECEF and ENU coordinates.

    lat0, lon0 are in degrees; h0 is the height above the WGS84 ellipsoid in meters.
    """

    def __init__(self, lat0: float, lon0: float, h0: float) -> None:
        handle = _HandlePtr()
        _check(_lib.geographic_coordinates_create(lat0, lon0, h0, ctypes.byref(handle)))
        self._handle = handle

    def __del__(self) -> None:
        handle = getattr(self, "_handle", None)
        if handle:
            _lib.geographic_coordinates_free(handle)
            self._handle = None

    def set_origin(self, phi: float, lambda_: float, height: float) -> None:
        _check(_lib.geographic_coordinates_set_origin(self._handle, phi, lambda_, height))

    def geodetic_to_ecef(self, phi: float, lambda_: float, h: float) -> tuple[float, float, float]:
        x, y, z = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_geodetic_to_ecef(
            self._handle, phi, lambda_, h, ctypes.byref(x), ctypes.byref(y), ctypes.byref(z)
        ))
        return x.value, y.value, z.value

    def ecef_to_geodetic(self, x: float, y: float, z: float) -> tuple[float, float, float]:
        phi, lambda_, h = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_ecef_to_geodetic(
            self._handle, x, y, z, ctypes.byref(phi), ctypes.byref(lambda_), ctypes.byref(h)
        ))
        return phi.value, lambda_.value, h.value

    def geodetic_to_enu(self, phi: float, lambda_: float, h: float) -> tuple[float, float, float]:
        x_east, y_north, z_up = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_geodetic_to_enu(
            self._handle, phi, lambda_, h, ctypes.byref(x_east), ctypes.byref(y_north), ctypes.byref(z_up)
        ))
        return x_east.value, y_north.value, z_up.value

    def enu_to_geodetic(self, x_east: float, y_north: float, z_up: float) -> tuple[float, float, float]:
        phi, lambda_, h = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_enu_to_geodetic(
            self._handle, x_east, y_north, z_up, ctypes.byref(phi), ctypes.byref(lambda_), ctypes.byref(h)
        ))
        return phi.value, lambda_.value, h.value

    def ecef_to_enu(self, x: float, y: float, z: float) -> tuple[float, float, float]:
        x_east, y_north, z_up = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_ecef_to_enu(
            self._handle, x, y, z, ctypes.byref(x_east), ctypes.byref(y_north), ctypes.byref(z_up)
        ))
        return x_east.value, y_north.value, z_up.value

    def enu_to_ecef(self, x_east: float, y_north: float, z_up: float) -> tuple[float, float, float]:
        x, y, z = ctypes.c_double(), ctypes.c_double(), ctypes.c_double()
        _check(_lib.geographic_coordinates_enu_to_ecef(
            self._handle, x_east, y_north, z_up, ctypes.byref(x), ctypes.byref(y), ctypes.byref(z)
        ))
        return x.value, y.value, z.value
