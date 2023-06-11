#include "test_extensions.hh"
using namespace Wt;

extern std::vector<dc311_data_t> dc311_data;

extern std::vector<ep_data_t> ep_data;
extern std::vector<us_state_pop_t> us_state_pop;

extern std::vector<star_dataset_t> datasets;

///////////////////////////////////////////////////////////////////////////////////////
//Application_schools
///////////////////////////////////////////////////////////////////////////////////////

extern std::vector<double> lat_montgomery;
extern std::vector<double> lon_montgomery;
extern std::vector<school_t> schools_list;
extern std::vector<wmata_station_t> wmata_station;

///////////////////////////////////////////////////////////////////////////////////////
//Application_dc311
///////////////////////////////////////////////////////////////////////////////////////

Application_dc311::Application_dc311(const Wt::WEnvironment& env, const geojson_t &rgeojson) :
  WApplication(env)
{
  setTitle("dc311");
  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13);

  std::vector<std::string> ward_color =
  { rgb_to_hex(128, 128, 0), //olive
    rgb_to_hex(255, 255, 0), //yellow 
    rgb_to_hex(0, 128, 0), //green
    rgb_to_hex(0, 255, 0), //lime
    rgb_to_hex(0, 128, 128), //teal
    rgb_to_hex(0, 255, 255), //aqua
    rgb_to_hex(0, 0, 255), //blue
    rgb_to_hex(128, 0, 128) //purple
  };

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = rgeojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = rgeojson.m_feature.at(idx_fet);

    //make each feature have a unique color
    std::string color = ward_color.at(idx_fet);

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

        if (size_crd == 1)
        {
          leaflet->Circle(polygon.m_coord[0].m_lat, polygon.m_coord[0].m_lon, 100, rgb_to_hex(255, 0, 0));
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices passed to Polygon
        ///////////////////////////////////////////////////////////////////////////////////////

        std::vector<double> lat;
        std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].m_lat);
          lon.push_back(polygon.m_coord[idx_crd].m_lon);
        }

        leaflet->Polygon(lat, lon, color);
      }  //idx_pol
    } //idx_geo
  } //idx_fet

    ///////////////////////////////////////////////////////////////////////////////////////
    //render CSV points from DC_311 database
    ///////////////////////////////////////////////////////////////////////////////////////

  size_t size = dc311_data.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    dc311_data_t data = dc311_data.at(idx);
    leaflet->Circle(data.lat, data.lon);
  }

  root()->addWidget(std::move(leaflet));
}


///////////////////////////////////////////////////////////////////////////////////////
//Application_us_states
///////////////////////////////////////////////////////////////////////////////////////

Application_us_states::Application_us_states(const WEnvironment& env, const geojson_t &rgeojson) :
  WApplication(env)
{
  for (size_t idx = 0; idx < 3 * 256; idx += 3)
  {
    unsigned char r = pal_rgb[idx];
    unsigned char g = pal_rgb[idx + 1];
    unsigned char b = pal_rgb[idx + 2];
    m_rgb_256.push_back(rgb_t(r, g, b));
  }
  setTitle("geojson");
  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 37.0902, -95.7129, 5);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = rgeojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = rgeojson.m_feature.at(idx_fet);
    size_t nbr_palette_entries = m_rgb_256.size();
    double value = idx_fet;
    double value_min = 0;
    double value_range = size_features - value_min;
    size_t idx_pal = (size_t)(nbr_palette_entries * ((value - value_min) / value_range));
    idx_pal = (idx_pal < 0 ? 0 : (idx_pal >= nbr_palette_entries ? nbr_palette_entries - 1 : idx_pal));
    std::string color = rgb_to_hex(
      m_rgb_256.at(idx_pal).red,
      m_rgb_256.at(idx_pal).green,
      m_rgb_256.at(idx_pal).blue);

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices passed to Polygon
        ///////////////////////////////////////////////////////////////////////////////////////

        std::vector<double> lat;
        std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].m_lat);
          lon.push_back(polygon.m_coord[idx_crd].m_lon);
        }

        if (geometry.m_type.compare("Point") == 0)
        {
          leaflet->Circle(lat[0], lon[0], color);
        }
        else if (geometry.m_type.compare("Polygon") == 0 ||
          geometry.m_type.compare("MultiPolygon") == 0)
        {
          leaflet->Polygon(lat, lon, color);
        }

      }  //idx_pol
    } //idx_geo
  } //idx_fet

  root()->addWidget(std::move(leaflet));
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_ep
///////////////////////////////////////////////////////////////////////////////////////

Application_ep::Application_ep(const WEnvironment& env, const geojson_t &rgeojson) :
  WApplication(env)
{
  setTitle("geojson");
  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 37.0902, -95.7129, 5);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = rgeojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = rgeojson.m_feature.at(idx_fet);

    //find ep by state name
    size_t ep = find_ep(feature.m_name);
    //find population by state name
    size_t population = find_population(feature.m_name);

    if (ep == 0 || population == 0)
    {
      continue;
    }

    double percentage_state_epilepsy = (double(ep) / double(population)) / 100.0;

    percentage_state_epilepsy *= 100000;
    std::string color;
    if (percentage_state_epilepsy < 10)
    {
      color = rgb_to_hex(0, 255, 0);
    }
    else if (percentage_state_epilepsy < 10.4)
    {
      color = rgb_to_hex(0, 128, 0);
    }
    else if (percentage_state_epilepsy < 10.8)
    {
      color = rgb_to_hex(0, 255, 255);
    }
    else if (percentage_state_epilepsy < 11.2)
    {
      color = rgb_to_hex(255, 0, 0);
    }
    else
    {
      color = rgb_to_hex(128, 0, 0);
    }

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices passed to Polygon
        ///////////////////////////////////////////////////////////////////////////////////////

        std::vector<double> lat;
        std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].m_lat);
          lon.push_back(polygon.m_coord[idx_crd].m_lon);
        }

        if (geometry.m_type.compare("Point") == 0)
        {
          leaflet->Circle(lat[0], lon[0], color);
        }
        else if (geometry.m_type.compare("Polygon") == 0 ||
          geometry.m_type.compare("MultiPolygon") == 0)
        {
          leaflet->Polygon(lat, lon, color);
        }

      }  //idx_pol
    } //idx_geo
  } //idx_fet

  root()->addWidget(std::move(leaflet));
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_schools
///////////////////////////////////////////////////////////////////////////////////////

Application_schools::Application_schools(const WEnvironment& env, const geojson_t &rgeojson) :
  WApplication(env)
{
  setTitle("geojson");
  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 39.0443047898, -77.1731281364, 11);

  marker_icon_t marker_green(
    "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-green.png",
    "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
    icon_size_t(25, 41),
    icon_size_t(12, 41),
    icon_size_t(1, -34),
    icon_size_t(41, 41));

  marker_icon_t marker_red(
    "https://cdn.rawgit.com/pointhi/leaflet-color-markers/master/img/marker-icon-2x-red.png",
    "https://cdnjs.cloudflare.com/ajax/libs/leaflet/0.7.7/images/marker-shadow.png",
    icon_size_t(25, 41),
    icon_size_t(12, 41),
    icon_size_t(1, -34),
    icon_size_t(41, 41));

  ///////////////////////////////////////////////////////////////////////////////////////
  //render boundary
  ///////////////////////////////////////////////////////////////////////////////////////

  assert(lat_montgomery.size() == lon_montgomery.size());
  std::string color = rgb_to_hex(128, 128, 0);
  leaflet->Polygon(lat_montgomery, lon_montgomery, color);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render geojson (ZIP)
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_features = rgeojson.m_feature.size();
  for (size_t idx_fet = 0; idx_fet < size_features; idx_fet++)
  {
    feature_t feature = rgeojson.m_feature.at(idx_fet);

    std::string color = rgb_to_hex(255, 255, 255);

    if (feature.m_name.compare("20850") == 0)
    {
      color = rgb_to_hex(0, 255, 0);
    }
    else if (feature.m_name.compare("20851") == 0)
    {
      color = rgb_to_hex(255, 255, 0);
    }
    else if (feature.m_name.compare("20852") == 0)
    {
      color = rgb_to_hex(0, 0, 255);
    }
    else if (feature.m_name.compare("20878") == 0)
    {
      color = rgb_to_hex(0, 255, 255);
    }
    else
    {
      continue;
    }

    size_t size_geometry = feature.m_geometry.size();
    for (size_t idx_geo = 0; idx_geo < size_geometry; idx_geo++)
    {
      geometry_t geometry = feature.m_geometry.at(idx_geo);
      size_t size_pol = geometry.m_polygons.size();

      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        polygon_t polygon = geometry.m_polygons[idx_pol];
        size_t size_crd = polygon.m_coord.size();

        if (size_crd == 0)
        {
          continue;
        }

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices passed to Polygon
        ///////////////////////////////////////////////////////////////////////////////////////

        std::vector<double> lat;
        std::vector<double> lon;

        for (size_t idx_crd = 0; idx_crd < size_crd; idx_crd++)
        {
          lat.push_back(polygon.m_coord[idx_crd].m_lat);
          lon.push_back(polygon.m_coord[idx_crd].m_lon);
        }

        leaflet->Polygon(lat, lon, color);
      }  //idx_pol
    } //idx_geo
  } //idx_fet

    ///////////////////////////////////////////////////////////////////////////////////////
    //render WMATA stations
    ///////////////////////////////////////////////////////////////////////////////////////

  std::string  color_red = rgb_to_hex(255, 0, 0);
  std::string  color_green = rgb_to_hex(0, 255, 0);
  std::string  color_blue = rgb_to_hex(0, 0, 255);
  std::string  color_orange = rgb_to_hex(255, 165, 0);
  std::string  color_yellow = rgb_to_hex(255, 255, 0);
  std::string  color_silver = rgb_to_hex(211, 211, 211);

  size_t size_stations = wmata_station.size();
  for (size_t idx = 0; idx < size_stations; idx++)
  {
    leaflet->Circle(wmata_station.at(idx).lat, wmata_station.at(idx).lon, color_red);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //render schools
  ///////////////////////////////////////////////////////////////////////////////////////

  size_t size_schools = schools_list.size();
  for (size_t idx = 0; idx < size_schools; idx++)
  {
    school_t data = schools_list.at(idx);

    std::string text(data.name);
    text += " ";
    text += std::to_string(data.rating);

    if (data.rating <= 4) //3, 4
    {
      leaflet->Marker(data.lat, data.lon, text, marker_red);
    }
    else if (data.rating <= 6) // 5, 6
    {
      leaflet->Marker(data.lat, data.lon, text);
    }
    else if (data.rating <= 10) // 7, 8, 9, 10
    {
      leaflet->Marker(data.lat, data.lon, text, marker_green);
    }
    else
    {
      assert(0);
    }
  }
  root()->addWidget(std::move(leaflet));
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_topojson
///////////////////////////////////////////////////////////////////////////////////////

Application_topojson::Application_topojson(const WEnvironment& env, topojson_t &rtopojson) :
  WApplication(env)
{
  setTitle("topojson sample");

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //must make the coordinates for the topology first
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t topology_index = 0;
  rtopojson.make_coordinates(topology_index);
  topology_object_t topology = rtopojson.m_topology.at(topology_index);

  size_t size_geom = topology.m_geom.size();

  std::vector<double> first = rtopojson.get_first(topology_index);
  int ifirst[2];
  ifirst[0] = (int)first[0];
  ifirst[1] = (int)first[1];
  std::vector<double> center = rtopojson.transform_point(ifirst);

  std::unique_ptr<WLeaflet> leaflet =
    cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, center[1], center[0], 6);

  ///////////////////////////////////////////////////////////////////////////////////////
  //render topojson
  ///////////////////////////////////////////////////////////////////////////////////////

  for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
  {
    Geometry_t geometry = topology.m_geom.at(idx_geom);
    if (geometry.type.compare("Polygon") == 0 || geometry.type.compare("MultiPolygon") == 0)
    {
      size_t size_pol = geometry.m_polygon.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        Polygon_topojson_t polygon = geometry.m_polygon.at(idx_pol);
        size_t size_arcs = polygon.arcs.size();

        ///////////////////////////////////////////////////////////////////////////////////////
        //render each polygon as a vector of vertices passed to Polygon
        ///////////////////////////////////////////////////////////////////////////////////////

        std::vector<double> lat;
        std::vector<double> lon;

        size_t size_points = geometry.m_polygon.at(idx_pol).m_y.size();
        for (size_t idx_crd = 0; idx_crd < size_points; idx_crd++)
        {
          lat.push_back(geometry.m_polygon.at(idx_pol).m_y.at(idx_crd));
          lon.push_back(geometry.m_polygon.at(idx_pol).m_x.at(idx_crd));
        }

        std::string color = rgb_to_hex(255, 255, 255);;
        if (geometry.type.compare("Polygon") == 0)
        {
          color = rgb_to_hex(255, 0, 0);
        }
        else if (geometry.type.compare("MultiPolygon") == 0)
        {
          color = rgb_to_hex(0, 255, 0);
        }
        leaflet->Polygon(lat, lon, color);

      }//size_pol
    }//"Polygon"
  }//size_geom

  root()->addWidget(std::move(leaflet));
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_json_wmata
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_json_wmata(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon)
{
  char *buf = 0;
  size_t length;
  FILE *f;

  std::cout << file_name << std::endl;
  f = fopen(file_name.c_str(), "rb");
  if (!f)
  {
    std::cout << "cannot open " << file_name << std::endl;
    assert(0);
    return -1;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc(length);
  if (buf)
  {
    if (fread(buf, 1, length, f) < 0)
    {
    }
  }
  fclose(f);

  char *endptr;
  JsonValue value;
  JsonAllocator allocator;
  int rc = jsonParse(buf, &endptr, &value, allocator);
  if (rc != JSON_OK)
  {
    std::cout << "invalid JSON format for " << buf << std::endl;
    return -1;
  }

  size_t arr_size = 0; //size of array
  for (JsonNode *node_root = value.toNode(); node_root != nullptr; node_root = node_root->next)
  {
    std::cout << node_root->key << std::endl;
    if (std::string(node_root->key).compare("Stations") == 0)
    {
      JsonValue value_stations = node_root->value;
      assert(value_stations.getTag() == JSON_ARRAY);
      for (JsonNode *node_arr = value_stations.toNode(); node_arr != nullptr; node_arr = node_arr->next)
      {
        arr_size++;
        JsonValue value_obj_station = node_arr->value;
        assert(value_obj_station.getTag() == JSON_OBJECT);

        double lat;
        double lon;
        std::string name;
        std::string line_code;

        for (JsonNode *node_obj = value_obj_station.toNode(); node_obj != nullptr; node_obj = node_obj->next)
        {
          if (std::string(node_obj->key).compare("Lat") == 0)
          {
            assert(node_obj->value.getTag() == JSON_NUMBER);
            lat = node_obj->value.toNumber();
          }
          else if (std::string(node_obj->key).compare("Lon") == 0)
          {
            assert(node_obj->value.getTag() == JSON_NUMBER);
            lon = node_obj->value.toNumber();
          }
          else if (std::string(node_obj->key).compare("LineCode1") == 0)
          {
            assert(node_obj->value.getTag() == JSON_STRING);
            line_code = node_obj->value.toString();
          }
          else if (std::string(node_obj->key).compare("Name") == 0)
          {
            assert(node_obj->value.getTag() == JSON_STRING);
            name = node_obj->value.toString();
            std::cout << name << " ";
          }
        } //node_obj

        wmata_station_t station(std::to_string(lat), std::to_string(lon), name, line_code);
        wmata_station.push_back(station);

      } //node_arr
    } //"Stations"
  } //node_root
  std::cout << "\n";

  free(buf);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_json_montgomery_county
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_json_montgomery_county(const std::string &file_name, std::vector<double> &lat, std::vector<double> &lon)
{
  char *buf = 0;
  size_t length;
  FILE *f;
  std::string polygon_data;

  std::cout << file_name << std::endl;
  f = fopen(file_name.c_str(), "rb");
  if (!f)
  {
    std::cout << "cannot open " << file_name << std::endl;
    assert(0);
    return -1;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  buf = (char*)malloc(length);
  if (buf)
  {
    if (fread(buf, 1, length, f) < 0)
    {
    }
  }
  fclose(f);

  char *endptr;
  JsonValue value;
  JsonAllocator allocator;
  int rc = jsonParse(buf, &endptr, &value, allocator);
  if (rc != JSON_OK)
  {
    std::cout << "invalid JSON format for " << buf << std::endl;
    return -1;
  }

  size_t arr_size = 0; //size of array
  for (JsonNode *node_root = value.toNode(); node_root != nullptr; node_root = node_root->next)
  {
    std::cout << node_root->key << std::endl;
    if (std::string(node_root->key).compare("data") == 0)
    {
      assert(node_root->value.getTag() == JSON_ARRAY);
      JsonValue value_data = node_root->value;
      for (JsonNode *node_data = value_data.toNode(); node_data != nullptr; node_data = node_data->next)
      {
        JsonValue value_arr = node_data->value;
        for (JsonNode *node_arr = value_arr.toNode(); node_arr != nullptr; node_arr = node_arr->next)
        {
          //"MULTIPOLYGON" string at index 8
          if (arr_size == 8)
          {
            assert(node_arr->value.getTag() == JSON_STRING);
            polygon_data = node_arr->value.toString();
          }
          arr_size++;
        }


      }//node_data
    }//"data"
  }//node_root

  size_t start = polygon_data.find("(((");
  size_t end = polygon_data.find(")))");
  start += 3;
  end += 3;
  size_t len = end - start + 1;
  std::string str = polygon_data.substr(start, len);
  size_t pos_comma = 0;
  while (true)
  {
    start = pos_comma;
    pos_comma = str.find(",", pos_comma);
    size_t len_lat_lon = pos_comma - start; //skip ","
    if (pos_comma == std::string::npos)
    {
      break;
    }
    std::string lat_lon = str.substr(start, len_lat_lon);
    size_t pos_space = lat_lon.find(" ");
    std::string str_lon = lat_lon.substr(0, pos_space);
    std::string str_lat = lat_lon.substr(pos_space + 1);
    double dlat = std::stod(str_lat);
    double dlon = std::stod(str_lon);
    lat.push_back(dlat);
    lon.push_back(dlon);
    pos_comma += 2; //skip ", " comma and space
  }

  free(buf);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_schools
//CATEGORY,SCHOOL NAME,ADDRESS,CITY,ZIP CODE,PHONE,URL,LONGITUDE,LATITUDE,LOCATION
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_schools(const std::string &file_name)
{
  read_csv_t csv;

  if (csv.open(file_name) < 0)
  {
    std::cout << "Cannot open file " << file_name.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::vector<std::string> row;

  //read header
  row = csv.read_row();
  size_t rows = 0;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string name = row.at(1);
    std::string lon = row.at(7);
    std::string lat = row.at(8);
    std::string rating = row.at(10);
    schools_list.push_back(school_t(name, lat, lon, std::stoi(rating)));
  }

  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";
  csv.m_ifs.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_dc311
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_dc311(const std::string &file_name)
{
  read_csv_t csv;

  if (csv.open(file_name) < 0)
  {
    std::cout << "Cannot open file " << file_name.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t rows = 0;
  std::vector<std::string> row;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    dc311_data_t data(row.at(0), row.at(1), row.at(2), row.at(3));
    dc311_data.push_back(data);
  }
  std::cout << "Processed " << rows << " rows" << std::endl;
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_ep
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_ep_pop(const std::string &file_name_ep, const std::string &file_name_pop)
{
  read_csv_t csv;

  if (csv.open(file_name_ep) < 0)
  {
    std::cout << "Cannot open file " << file_name_ep.c_str() << std::endl;
    return -1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //iterate until an empty row is returned (end of file)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t rows = 0;
  std::vector<std::string> row;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string eps = row.at(1);
    size_t pos = eps.find("(");
    eps = eps.substr(0, pos);
    pos = eps.find(",");
    std::string eps1 = eps.substr(0, pos);
    std::string eps2 = eps.substr(pos + 1);
    eps = eps1 + eps2;
    ep_data_t data(row.at(0), std::stoul(eps));
    ep_data.push_back(data);
    std::cout << ep_data.back().state << " " << ep_data.back().ep << "\t";
  }

  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";

  csv.m_ifs.close();

  if (csv.open(file_name_pop) < 0)
  {
    std::cout << "Cannot open file " << file_name_pop.c_str() << std::endl;
    return -1;
  }

  rows = 0;
  while (true)
  {
    row = csv.read_row();
    if (row.size() == 0)
    {
      break;
    }
    rows++;

    std::string state = row.at(0);
    std::string pop = row.at(1);
    us_state_pop_t state_pop(state, std::stoul(pop));
    us_state_pop.push_back(state_pop);
    std::cout << us_state_pop.back().state << " " << us_state_pop.back().population << "\t";

  }
  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";

  csv.m_ifs.close();
  return 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_dataset
/////////////////////////////////////////////////////////////////////////////////////////////////////

star_dataset_t find_dataset(std::string name)
{
  for (size_t idx = 0; idx < datasets.size(); idx++)
  {
    if (datasets.at(idx).m_name.compare(name) == 0)
    {
      return datasets.at(idx);
    }
  }
  assert(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_ep
//by state name
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t find_ep(std::string state_name)
{
  for (size_t idx = 0; idx < ep_data.size(); idx++)
  {
    std::string state = ep_data.at(idx).state;
    if (state.find(state_name) == 0)
    {
      return ep_data.at(idx).ep;
    }
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_population
//by state name
/////////////////////////////////////////////////////////////////////////////////////////////////////

size_t find_population(std::string state_name)
{
  for (size_t idx = 0; idx < us_state_pop.size(); idx++)
  {
    std::string state = us_state_pop.at(idx).state;
    if (state.find(state_name) == 0)
    {
      return us_state_pop.at(idx).population;
    }
  }
  return 0;
}


void write_blue_red()
{
  std::vector<std::string> blue =
  { rgb_to_hex(210,229,239),
    rgb_to_hex(178,213,230),
    rgb_to_hex(149,197,220),
    rgb_to_hex(108,173,207),
    rgb_to_hex(71,148,194),
    rgb_to_hex(55,125,182),
    rgb_to_hex(39,104,172),
    rgb_to_hex(24,76,132),
    rgb_to_hex(9,50,95),
    rgb_to_hex(4,33,63)
  };

  std::vector<std::string> red =
  { rgb_to_hex(254,219,201),
    rgb_to_hex(247,191,165),
    rgb_to_hex(243,164,134),
    rgb_to_hex(230,129,107),
    rgb_to_hex(212,98,80),
    rgb_to_hex(195,60,64),
    rgb_to_hex(176,25,47),
    rgb_to_hex(139,15,39),
    rgb_to_hex(102,3,31),
    rgb_to_hex(68,0,21)
  };
  std::ofstream ofs("blue.red.txt", std::ios::out | std::ios::binary);
  size_t size = blue.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    ofs << blue.at(idx) << "\n";
  }
  size = red.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    ofs << red.at(idx) << "\n";
  }
  ofs.close();
}
