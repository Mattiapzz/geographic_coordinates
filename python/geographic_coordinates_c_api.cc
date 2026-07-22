//!
//! \file geographic_coordinates_c_api.cc
//! \brief Implementation of the C API declared in geographic_coordinates_c_api.hh.
//!

#include "geographic_coordinates_c_api.hh"

using geographic_coordinates::GeodeticTransformations;

namespace
{
GeodeticTransformations *cast(GeographicCoordinatesHandle *handle)
{
  return reinterpret_cast<GeodeticTransformations *>(handle);
}
} // namespace

extern "C"
{

  GeographicCoordinatesStatus
  geographic_coordinates_create(double lat0, double lon0, double h0, GeographicCoordinatesHandle **handle)
  {
    try
    {
      *handle = reinterpret_cast<GeographicCoordinatesHandle *>(new GeodeticTransformations(lat0, lon0, h0)
      );
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      *handle = nullptr;
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  void geographic_coordinates_free(GeographicCoordinatesHandle *handle) { delete cast(handle); }

  GeographicCoordinatesStatus geographic_coordinates_set_origin(
    GeographicCoordinatesHandle *handle, double phi, double lambda, double height
  )
  {
    try
    {
      cast(handle)->set_origin(phi, lambda, height);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_geodetic_to_ecef(
    GeographicCoordinatesHandle *handle, double phi, double lambda, double h, double *x, double *y, double *z
  )
  {
    try
    {
      cast(handle)->convert_geodetic_coords_to_ECEF(phi, lambda, h, *x, *y, *z);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_ecef_to_geodetic(
    GeographicCoordinatesHandle *handle, double x, double y, double z, double *phi, double *lambda, double *h
  )
  {
    try
    {
      cast(handle)->convert_ECEF_to_geodetic_coords(x, y, z, *phi, *lambda, *h);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_geodetic_to_enu(
    GeographicCoordinatesHandle *handle, double phi, double lambda, double h, double *x_east, double *y_north, double *z_up
  )
  {
    try
    {
      cast(handle)->convert_geodetic_coords_to_ENU(phi, lambda, h, *x_east, *y_north, *z_up);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_enu_to_geodetic(
    GeographicCoordinatesHandle *handle, double x_east, double y_north, double z_up, double *phi, double *lambda, double *h
  )
  {
    try
    {
      cast(handle)->convert_ENU_to_geodetic_coords(x_east, y_north, z_up, *phi, *lambda, *h);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_ecef_to_enu(
    GeographicCoordinatesHandle *handle, double x_ecef, double y_ecef, double z_ecef, double *x_east, double *y_north, double *z_up
  )
  {
    try
    {
      cast(handle)->convert_ECEF_to_ENU(x_ecef, y_ecef, z_ecef, *x_east, *y_north, *z_up);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

  GeographicCoordinatesStatus geographic_coordinates_enu_to_ecef(
    GeographicCoordinatesHandle *handle, double x_east, double y_north, double z_up, double *x_ecef, double *y_ecef, double *z_ecef
  )
  {
    try
    {
      cast(handle)->convert_ENU_to_ECEF(x_east, y_north, z_up, *x_ecef, *y_ecef, *z_ecef);
      return GEOGRAPHIC_COORDINATES_OK;
    }
    catch (...)
    {
      return GEOGRAPHIC_COORDINATES_ERROR;
    }
  }

} // extern "C"
