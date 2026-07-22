//!
//! \file geodetic_transformations.hh
//! \brief Conversions between geodetic, ECEF and ENU geographic coordinates.
//!
#pragma once

#include <cmath>
#include <numbers>
#include <ostream>

namespace geographic_coordinates {

  using real = double;

  //!
  //! \brief Conversion between geographic coordinates.
  //!
  //! This class stores a geographic position as origin of a local reference
  //! frame built at runtime, and implements transformations between
  //! Earth-Centered-Earth-Fixed (ECEF) coordinates and East-North-Up (ENU)
  //! coordinates.
  //!
  //! The state consists of the latitude, longitude and altitude of the
  //! local frame origin.
  //!
  //! The datum used for GPS positioning is called WGS84 (World Geodetic
  //! System 1984). It consists of a three-dimensional Cartesian coordinate
  //! system and an associated ellipsoid so that WGS84 positions can be
  //! described as either XYZ Cartesian coordinates or latitude, longitude
  //! and ellipsoid height coordinates. The origin of the datum is the
  //! Geocentre (the centre of mass of the Earth) and it is designed for
  //! positioning anywhere on Earth. The shape and size of the WGS84 biaxial
  //! ellipsoid is defined by the semi-major axis length a = 6378137.0
  //! metres, and the reciprocal of flattening 1/f = 298.257223563. This
  //! ellipsoid is the same shape and size as the GRS80 ellipsoid.
  //!
  //! All internal coordinates follow the International System of Units and
  //! refer to the Earth center (WGS84 ellipsoid center) except where
  //! explicitly noted.
  //!
  //! Geodetic Coordinates
  //!
  //! A geodetic system uses the coordinates (lat, lon, h) to represent
  //! position relative to a reference ellipsoid.
  //!  - lat: the latitude, originates at the equator. The latitude of a
  //!    point is the angle a normal to the ellipsoid at that point makes
  //!    with the equatorial plane. Range [-90 deg, 90 deg]; positive north.
  //!  - lon: the longitude, originates at the prime meridian. Range
  //!    [-180 deg, 180 deg] or [0 deg, 360 deg]; positive counterclockwise
  //!    viewed from above the North Pole.
  //!  - h: the ellipsoidal height, measured along the ellipsoid normal.
  //!
  //! East-North-Up Coordinates
  //!
  //! An east-north-up (ENU) system uses Cartesian coordinates
  //! (x_east, y_north, z_up) to represent position relative to a local
  //! origin described by geodetic coordinates (lat0, lon0, h0). The origin
  //! does not necessarily lie on the ellipsoid surface. The positive
  //! x_east axis points east, the positive y_north axis points north, and
  //! the positive z_up axis points along the ellipsoid normal.
  //!
  class GeodeticTransformations {

    /* WGS84 ellipsoid */
    static constexpr real kWGS84SemiMajorAxis{6378137.0};        //!< a
    static constexpr real kWGS84SemiMinorAxis{6356752.31424518}; //!< b
    static constexpr real kWGS84Eccentricity{0.0818191908426215}; //!< e
    static constexpr real kWGS84ReciprocalFlat{298.257223563};   //!< 1/f

    /* GPS coordinates of the local frame origin */
    real m_orig_latitude{0};  //!< latitude of origin (rad), [-pi/2, pi/2]
    real m_orig_longitude{0}; //!< longitude of origin (rad), [-pi, pi]
    real m_orig_height{0};    //!< height above the ellipsoid surface (m)
    real m_orig_altitude{0};  //!< distance from equatorial plane along the normal (m)

    /* Unit vectors of the local ENU (East North Up) reference frame */
    real m_orig_uvec_east_x{0};
    real m_orig_uvec_east_y{0};
    real m_orig_uvec_east_z{0};

    real m_orig_uvec_north_x{0};
    real m_orig_uvec_north_y{0};
    real m_orig_uvec_north_z{0};

    real m_orig_norm_x{0};
    real m_orig_norm_y{0};
    real m_orig_norm_z{0};

  public:

    //!
    //! \brief Construct a transformation with the given local frame origin.
    //! \param lat0 latitude of origin (deg)
    //! \param lon0 longitude of origin (deg)
    //! \param h0   height of origin above the WGS84 ellipsoid (m)
    //!
    explicit GeodeticTransformations( real lat0, real lon0, real h0 ) { set_origin( lat0, lon0, h0 ); }

    //!
    //! \brief Set the geodetic coordinates of the origin of the ENU (East North Up) projection.
    //! \param phi    latitude of origin (deg)
    //! \param lambda longitude of origin (deg)
    //! \param height height of origin above the WGS84 ellipsoid (m)
    //!
    void set_origin( real phi, real lambda, real height );

    //! \return latitude of origin (rad)
    [[nodiscard]] real origin_latitude() const { return m_orig_latitude; }

    //! \return latitude of origin (deg)
    [[nodiscard]] real origin_latitude_deg() const { return m_orig_latitude*180/std::numbers::pi; }

    //! \return longitude of origin (rad)
    [[nodiscard]] real origin_longitude() const { return m_orig_longitude; }

    //! \return longitude of origin (deg)
    [[nodiscard]] real origin_longitude_deg() const { return m_orig_longitude*180/std::numbers::pi; }

    //! \return height of origin above the WGS84 ellipsoid (m)
    [[nodiscard]] real origin_height() const { return m_orig_height; }

    //! \return altitude of origin from the equatorial plane (m)
    [[nodiscard]] real origin_altitude() const { return m_orig_altitude; }

    [[nodiscard]] real origin_normal_x() const { return m_orig_norm_x; }
    [[nodiscard]] real origin_normal_y() const { return m_orig_norm_y; }
    [[nodiscard]] real origin_normal_z() const { return m_orig_norm_z; }

    [[nodiscard]] real origin_uvec_east_x() const { return m_orig_uvec_east_x; }
    [[nodiscard]] real origin_uvec_east_y() const { return m_orig_uvec_east_y; }
    [[nodiscard]] real origin_uvec_east_z() const { return m_orig_uvec_east_z; }

    [[nodiscard]] real origin_uvec_north_x() const { return m_orig_uvec_north_x; }
    [[nodiscard]] real origin_uvec_north_y() const { return m_orig_uvec_north_y; }
    [[nodiscard]] real origin_uvec_north_z() const { return m_orig_uvec_north_z; }

    //!
    //! \brief Write a human-readable summary of the ellipsoid and origin data.
    //! \param stream output stream
    //!
    void info( std::ostream & stream ) const;

    //!
    //! \brief Compute the normal to the ellipsoid surface at a given location.
    //! \param lat    latitude (rad)
    //! \param lon    longitude (rad)
    //! \param norm_x [out] x component of the normal vector
    //! \param norm_y [out] y component of the normal vector
    //! \param norm_z [out] z component of the normal vector
    //!
    void compute_norm_to_ellipsoid( real lat, real lon, real & norm_x, real & norm_y, real & norm_z ) const;

    //!
    //! \brief Compute the unit vector pointing east at a given location.
    //! \param lon         longitude (rad)
    //! \param uvec_east_x [out] x component of the east unit vector
    //! \param uvec_east_y [out] y component of the east unit vector
    //! \param uvec_east_z [out] z component of the east unit vector
    //!
    void compute_unit_vector_east( real lon, real & uvec_east_x, real & uvec_east_y, real & uvec_east_z ) const;

    //!
    //! \brief Compute the unit vector pointing north at a given location.
    //! \param lat          latitude (rad)
    //! \param lon          longitude (rad)
    //! \param uvec_north_x [out] x component of the north unit vector
    //! \param uvec_north_y [out] y component of the north unit vector
    //! \param uvec_north_z [out] z component of the north unit vector
    //!
    void compute_unit_vector_north( real lat, real lon, real & uvec_north_x, real & uvec_north_y, real & uvec_north_z ) const;

    //!
    //! \brief Transform geodetic coordinates to ECEF Cartesian coordinates.
    //! \param phi    geodetic latitude (deg)
    //! \param lambda geodetic longitude (deg)
    //! \param h      geodetic height (m)
    //! \param x      [out] ECEF x (m)
    //! \param y      [out] ECEF y (m)
    //! \param z      [out] ECEF z (m)
    //!
    void convert_geodetic_coords_to_ECEF( real phi, real lambda, real h, real & x, real & y, real & z ) const;

    //!
    //! \brief Transform ECEF Cartesian coordinates to local ENU coordinates.
    //! \param x_ecef  ECEF x (m)
    //! \param y_ecef  ECEF y (m)
    //! \param z_ecef  ECEF z (m)
    //! \param x_east  [out] local east coordinate (m)
    //! \param y_north [out] local north coordinate (m)
    //! \param z_up    [out] local up coordinate (m)
    //!
    void convert_ECEF_to_ENU( real x_ecef, real y_ecef, real z_ecef, real & x_east, real & y_north, real & z_up ) const;

    //!
    //! \brief Transform geodetic coordinates directly to local ENU coordinates.
    //! \param phi     geodetic latitude (deg)
    //! \param lambda  geodetic longitude (deg)
    //! \param h       geodetic height (m)
    //! \param x_east  [out] local east coordinate (m)
    //! \param y_north [out] local north coordinate (m)
    //! \param z_up    [out] local up coordinate (m)
    //!
    void convert_geodetic_coords_to_ENU( real phi, real lambda, real h, real & x_east, real & y_north, real & z_up ) const;

    //!
    //! \brief Transform local ENU coordinates to ECEF Cartesian coordinates.
    //! \param x_east  local east coordinate (m)
    //! \param y_north local north coordinate (m)
    //! \param z_up    local up coordinate (m)
    //! \param x_ecef  [out] ECEF x (m)
    //! \param y_ecef  [out] ECEF y (m)
    //! \param z_ecef  [out] ECEF z (m)
    //!
    void convert_ENU_to_ECEF( real x_east, real y_north, real z_up, real & x_ecef, real & y_ecef, real & z_ecef ) const;

    //!
    //! \brief Transform ECEF Cartesian coordinates to geodetic coordinates.
    //!
    //! Implementation from: Vermeille, H., 2002. Direct transformation from
    //! geocentric coordinates to geodetic coordinates. Journal of Geodesy,
    //! 76(8), pp.451-454. https://doi.org/10.1007/s00190-002-0273-6
    //!
    //! \param x_ecef ECEF x (m)
    //! \param y_ecef ECEF y (m)
    //! \param z_ecef ECEF z (m)
    //! \param phi    [out] geodetic latitude (deg)
    //! \param lambda [out] geodetic longitude (deg)
    //! \param h      [out] geodetic height (m)
    //!
    void convert_ECEF_to_geodetic_coords( real x_ecef, real y_ecef, real z_ecef, real & phi, real & lambda, real & h ) const;

    //!
    //! \brief Transform local ENU coordinates to geodetic coordinates.
    //! \param x_east  local east coordinate (m)
    //! \param y_north local north coordinate (m)
    //! \param z_up    local up coordinate (m)
    //! \param phi     [out] geodetic latitude (deg)
    //! \param lambda  [out] geodetic longitude (deg)
    //! \param h       [out] geodetic height (m)
    //!
    void convert_ENU_to_geodetic_coords( real x_east, real y_north, real z_up, real & phi, real & lambda, real & h ) const;
  };

} // namespace geographic_coordinates
