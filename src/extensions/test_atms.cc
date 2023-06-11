#include "test_extensions.hh"
using namespace Wt;


///////////////////////////////////////////////////////////////////////////////////////
//str_lon_lat
///////////////////////////////////////////////////////////////////////////////////////

std::string str_lon_lat(double lon, double lat)
{
  std::string str;
  str = std::to_string(lon);
  str += ",";
  str += std::to_string(lat);
  return str;
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_atms
///////////////////////////////////////////////////////////////////////////////////////

Application_atms::Application_atms(const WEnvironment& env) : WApplication(env)
{
  for (size_t idx = 0; idx < 3 * 256; idx += 3)
  {
    unsigned char r = pal_rgb[idx];
    unsigned char g = pal_rgb[idx + 1];
    unsigned char b = pal_rgb[idx + 2];
    m_rgb_256.push_back(rgb_t(r, g, b));
  }
  star_dataset_t temperature = find_dataset("AntennaTemperature");
  star_dataset_t latitude = find_dataset("latitude");
  star_dataset_t longitude = find_dataset("longitude");
  size_t nbr_rows = temperature.m_shape[0];
  size_t nbr_cols = temperature.m_shape[1];
  size_t nbr_lev = temperature.m_shape[2];
  double temp_min;
  double temp_max;
  double lat_min;
  double lat_max;
  double lon_min;
  double lon_max;
  temperature.do_min_max(temp_min, temp_max);
  latitude.do_min_max(lat_min, lat_max);
  longitude.do_min_max(lon_min, lon_max);
  double lat = (lat_max + lat_min) / 2;
  double lon = (lon_max + lon_min) / 2;

  setTitle("atms");
  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, lat, lon, 5);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render border
  ///////////////////////////////////////////////////////////////////////////////////////

  std::vector<double> blat(4);
  std::vector<double> blon(4);
  blat[0] = lat_min;
  blon[0] = lon_min;
  blat[1] = lat_max;
  blon[1] = lon_min;
  blat[2] = lat_max;
  blon[2] = lon_max;
  blat[3] = lat_min;
  blon[3] = lon_max;
  leaflet->Polygon(blat, blon, "#76d7c4");

  ///////////////////////////////////////////////////////////////////////////////////////
  //render data
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t idx_channel = 0;

  for (size_t idx_row = 0; idx_row < nbr_rows - 1; idx_row++)
  {
    for (size_t idx_col = 0; idx_col < nbr_cols - 1; idx_col++)
    {
      ///////////////////////////////////////////////////////////////////////////
      //polygon coordinates for a granule
      ///////////////////////////////////////////////////////////////////////////

      std::vector<double> vlat(4);
      std::vector<double> vlon(4);
      vlat[0] = latitude.value_at(idx_row, idx_col);
      vlon[0] = longitude.value_at(idx_row, idx_col);
      vlat[1] = latitude.value_at(idx_row, idx_col + 1);
      vlon[1] = longitude.value_at(idx_row, idx_col + 1);
      vlat[2] = latitude.value_at(idx_row + 1, idx_col + 1);
      vlon[2] = longitude.value_at(idx_row + 1, idx_col + 1);
      vlat[3] = latitude.value_at(idx_row + 1, idx_col);
      vlon[3] = longitude.value_at(idx_row + 1, idx_col);

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //color
      //The Vizualization Toolkit, pg.156
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      size_t nbr_palette_entries = m_rgb_256.size();
      double value_range = temp_max - temp_min;
      double value_min = temp_min;
      double value = temperature.value_at(idx_row, idx_col, idx_channel);
      size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
      idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));
      std::string color = rgb_to_hex(
        m_rgb_256.at(idx_pal).red,
        m_rgb_256.at(idx_pal).green,
        m_rgb_256.at(idx_pal).blue);
      leaflet->Polygon(vlat, vlon, color);
    }
  }
  root()->addWidget(std::move(leaflet));
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_celsium_atms
///////////////////////////////////////////////////////////////////////////////////////

Application_celsium_atms::Application_celsium_atms(const WEnvironment& env)
  : WApplication(env)
{
  setTitle("Cesium NASA HDF5");
  std::string js;
  for (size_t idx = 0; idx < 3 * 256; idx += 3)
  {
    unsigned char r = pal_rgb[idx];
    unsigned char g = pal_rgb[idx + 1];
    unsigned char b = pal_rgb[idx + 2];
    m_rgb_256.push_back(rgb_t(r, g, b));
  }

  star_dataset_t temperature = find_dataset("AntennaTemperature");
  star_dataset_t latitude = find_dataset("latitude");
  star_dataset_t longitude = find_dataset("longitude");
  size_t nbr_rows = temperature.m_shape[0];
  size_t nbr_cols = temperature.m_shape[1];
  size_t nbr_lev = temperature.m_shape[2];
  double temp_min;
  double temp_max;
  double lat_min;
  double lat_max;
  double lon_min;
  double lon_max;
  temperature.do_min_max(temp_min, temp_max);
  latitude.do_min_max(lat_min, lat_max);
  longitude.do_min_max(lon_min, lon_max);
  double lat = (lat_max + lat_min) / 2;
  double lon = (lon_max + lon_min) / 2;

  js += "var lon = ";
  js += std::to_string(lon);
  js += ";";
  js += "var lat = ";
  js += std::to_string(lat);
  js += ";";
  js += "var center = Cesium.Cartesian3.fromDegrees(lon, lat);";
  js += "viewer.camera.lookAt(center, new Cesium.Cartesian3(0.0, 0.0, 4000000.0)); ";

  js += "var scene = viewer.scene;";
  js += "var instances = [];";

  ///////////////////////////////////////////////////////////////////////////////////////
  //render data
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t idx_channel = 0;

  for (size_t idx_row = 0; idx_row < nbr_rows - 1; idx_row++)
  {
    for (size_t idx_col = 0; idx_col < nbr_cols - 1; idx_col++)
    {
      ///////////////////////////////////////////////////////////////////////////
      //polygon coordinates for a granule
      ///////////////////////////////////////////////////////////////////////////

      std::vector<double> vlat(4);
      std::vector<double> vlon(4);
      vlat[0] = latitude.value_at(idx_row, idx_col);
      vlon[0] = longitude.value_at(idx_row, idx_col);
      vlat[1] = latitude.value_at(idx_row, idx_col + 1);
      vlon[1] = longitude.value_at(idx_row, idx_col + 1);
      vlat[2] = latitude.value_at(idx_row + 1, idx_col + 1);
      vlon[2] = longitude.value_at(idx_row + 1, idx_col + 1);
      vlat[3] = latitude.value_at(idx_row + 1, idx_col);
      vlon[3] = longitude.value_at(idx_row + 1, idx_col);

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //color
      //The Vizualization Toolkit, pg.156
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      size_t nbr_palette_entries = m_rgb_256.size();
      double value_range = temp_max - temp_min;
      double value_min = temp_min;
      double value = temperature.value_at(idx_row, idx_col, idx_channel);
      size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
      idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));

      double r = m_rgb_256.at(idx_pal).red / 255.0;
      double g = m_rgb_256.at(idx_pal).green / 255.0;
      double b = m_rgb_256.at(idx_pal).blue / 255.0;

      std::string clr = std::to_string(r); clr += ",";
      clr += std::to_string(g); clr += ",";
      clr += std::to_string(b); clr += ",";
      clr += "0.1"; //alpha

      js += "instances.push(new Cesium.GeometryInstance({";
      js += "  geometry : new Cesium.PolygonGeometry({";
      js += "    polygonHierarchy : new Cesium.PolygonHierarchy(";
      js += "     Cesium.Cartesian3.fromDegreesArray([";

      js += str_lon_lat(vlon[0], vlat[0]); js += ",";
      js += str_lon_lat(vlon[1], vlat[1]); js += ",";
      js += str_lon_lat(vlon[2], vlat[2]); js += ",";
      js += str_lon_lat(vlon[3], vlat[3]);

      js += "     ])";//array
      js += "     ),";//PolygonHierarchy
      js += "    vertexFormat: Cesium.PerInstanceColorAppearance.VERTEX_FORMAT";
      js += "   }),"; //PolygonGeometry
      js += "  attributes : {";
      js += "    color : new Cesium.ColorGeometryInstanceAttribute(";
      js += clr;
      js += "    )";
      js += "  }";//attributes
      js += "}));";//push

      js += "scene.primitives.add(new Cesium.Primitive({";
      js += "  geometryInstances : instances,";
      js += "  appearance : new Cesium.PerInstanceColorAppearance()";
      js += "}));";//add
    }
  }

  std::unique_ptr<WCesium> celsium = cpp14::make_unique<WCesium>(js);
  root()->addWidget(std::move(celsium));
}

