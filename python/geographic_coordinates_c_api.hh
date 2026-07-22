//!
//! \file geographic_coordinates_c_api.hh
//! \brief Stable C API exposing GeodeticTransformations to non-C++ consumers.
//!
#pragma once

#include <geographic_coordinates/geodetic_transformations.hh>

#if defined(_WIN32) || defined(_WIN64)
  #if defined(GEOGRAPHIC_COORDINATES_EXPORTS)
    #define GEOGRAPHIC_COORDINATES_EXPORT __declspec(dllexport)
  #else
    #define GEOGRAPHIC_COORDINATES_EXPORT __declspec(dllimport)
  #endif
#else
  #define GEOGRAPHIC_COORDINATES_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

  //! Opaque handle to a geographic_coordinates::GeodeticTransformations instance.
  typedef struct GeographicCoordinatesHandle GeographicCoordinatesHandle;

  //! Status codes returned by every exported function.
  typedef enum GeographicCoordinatesStatus {
    GEOGRAPHIC_COORDINATES_OK    = 0, //!< call succeeded
    GEOGRAPHIC_COORDINATES_ERROR = 1  //!< call failed, an exception was caught
  } GeographicCoordinatesStatus;

  //!
  //! \brief Create a new GeodeticTransformations instance.
  //!
  //! Ownership is transferred to the caller: the returned handle must be
  //! released with geographic_coordinates_free().
  //!
  //! \param lat0   latitude of origin (deg)
  //! \param lon0   longitude of origin (deg)
  //! \param h0     height of origin above the WGS84 ellipsoid (m)
  //! \param handle [out] newly created handle, valid only on GEOGRAPHIC_COORDINATES_OK
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_create( double lat0, double lon0, double h0, GeographicCoordinatesHandle ** handle );

  //!
  //! \brief Destroy a handle previously returned by geographic_coordinates_create().
  //! \param handle handle to release; ownership is consumed, do not use it afterwards.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  void
  geographic_coordinates_free( GeographicCoordinatesHandle * handle );

  //!
  //! \brief Reset the origin of the local ENU projection.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_set_origin( GeographicCoordinatesHandle * handle, double phi, double lambda, double height );

  //!
  //! \brief Convert geodetic coordinates to ECEF Cartesian coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_geodetic_to_ecef(
    GeographicCoordinatesHandle * handle,
    double phi, double lambda, double h,
    double * x, double * y, double * z
  );

  //!
  //! \brief Convert ECEF Cartesian coordinates to geodetic coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_ecef_to_geodetic(
    GeographicCoordinatesHandle * handle,
    double x, double y, double z,
    double * phi, double * lambda, double * h
  );

  //!
  //! \brief Convert geodetic coordinates directly to local ENU coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_geodetic_to_enu(
    GeographicCoordinatesHandle * handle,
    double phi, double lambda, double h,
    double * x_east, double * y_north, double * z_up
  );

  //!
  //! \brief Convert local ENU coordinates to geodetic coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_enu_to_geodetic(
    GeographicCoordinatesHandle * handle,
    double x_east, double y_north, double z_up,
    double * phi, double * lambda, double * h
  );

  //!
  //! \brief Convert ECEF Cartesian coordinates to local ENU coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_ecef_to_enu(
    GeographicCoordinatesHandle * handle,
    double x_ecef, double y_ecef, double z_ecef,
    double * x_east, double * y_north, double * z_up
  );

  //!
  //! \brief Convert local ENU coordinates to ECEF Cartesian coordinates.
  //!
  GEOGRAPHIC_COORDINATES_EXPORT
  GeographicCoordinatesStatus
  geographic_coordinates_enu_to_ecef(
    GeographicCoordinatesHandle * handle,
    double x_east, double y_north, double z_up,
    double * x_ecef, double * y_ecef, double * z_ecef
  );

} // extern "C"
