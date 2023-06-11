#include "test_extensions.hh"
#include <iomanip>
using namespace Wt;

size_t total = 1;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//to_hex
//convert int to hex string, apply zero padding
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string to_hex(int n)
{
  std::stringstream ss;
  ss << std::hex << n;
  std::string str(ss.str());
  return str.size() == 1 ? "0" + str : str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_to_hex
/////////////////////////////////////////////////////////////////////////////////////////////////////

std::string rgb_to_hex(int r, int g, int b)
{
  std::string str("#");
  str += to_hex(r);
  str += to_hex(g);
  str += to_hex(b);
  return str;
}

///////////////////////////////////////////////////////////////////////////////////////
//gradient_t
///////////////////////////////////////////////////////////////////////////////////////

class gradient_t
{
public:
  gradient_t(const std::string& c, double g) :
    clr(c),
    grade(g)
  {
  };
  std::string clr;
  double grade;
};

std::vector<gradient_t> gbr =
{
  { "#04213f" , 0.05},{ "#09325f" , 0.1 },{ "#184c84" , 0.15 },{ "#2768ac" , 0.20 },{ "#377db6" , 0.25 },
  { "#4794c2" , 0.3 },{ "#6cadcf" , 0.35 },{ "#95c5dc" , 0.4 },{ "#b2d5e6" , 0.45 },{ "#d2e5ef" , 0.5 },
  { "#fedbc9" , 0.55 },{ "#f7bfa5" , 0.6 },{ "#f3a486" , 0.65 },{ "#e6816b" , 0.7 },{ "#d46250" , 0.75 },
  { "#c33c40" , 0.8 },{ "#b0192f" , 0.85 },{ "#8b0f27" , 0.9 },{ "#66031f" , 0.95 },{ "#440015" , 1 }
};

std::string get_color(double d)
{
  //20 colors, 5 points increase
  for (size_t idx = 0; idx < gbr.size(); idx++)
  {
    if (gbr.at(idx).grade > d)
    {
      return gbr.at(idx).clr;
    }
  }
  assert(0);
}

//./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=.

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 2
//DC311 rodent complaints and DC wards geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 2 -d ../../../examples/test_extensions/data/dc_311-2016.csv.s0311.csv -g ../../../examples/test_extensions/data/ward-2012.geojson

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 3
//US states geojson
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 3 -g ../../../examples/test_extensions/data/gz_2010_us_040_00_20m.json

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 4
//NOAA ATMS data
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 4 -d ../../../examples/test_extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json

std::vector<star_dataset_t> datasets;

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 5
//ep
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 5 
//-d ../../../examples/test_extensions/data/us_states_epilepsy_2015.csv 
//-g ../../../examples/test_extensions/data/gz_2010_us_040_00_20m.json 
//-u ../../../examples/test_extensions/data/us_states_population_2015.csv

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 6
//schools
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 6 -d ../../../examples/test_extensions/data/montgomery_county_schools.csv -g ../../../examples/test_extensions/data/montgomery_county_boundary.json -m ../../../examples/test_extensions/data/wmata_stations.json -z ../../../examples/test_extensions/data/md_maryland_zip_codes_geo.min.json

std::vector<school_t> schools_list;
std::vector<double> lat_montgomery;
std::vector<double> lon_montgomery;
std::vector<double> lat_wmata;
std::vector<double> lon_wmata;
std::string file_wmata_stations;
std::vector<wmata_station_t> wmata_station;
int read_schools(const std::string& file_name);
int read_json_montgomery_county(const std::string& file_name, std::vector<double>& lat, std::vector<double>& lon);
int read_json_wmata(const std::string& file_name, std::vector<double>& lat, std::vector<double>& lon);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 7
//topojson 
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 7 -g ../../../examples/test_extensions/data/example.quantized.topojson
//-t 7 -g ../../../examples/test_extensions/data/czech-republic-regions.json
//-t 7 -g ../../../examples/test_extensions/data/portugal-districts.topojson
//-t 7 -g ../../../examples/test_extensions/data/us.topojson

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 8
//Cesium render
//NOAA ATMS
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 8 -d ../../../examples/test_extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json 

/////////////////////////////////////////////////////////////////////////////////////////////////////
//example 10
//counties elections 
/////////////////////////////////////////////////////////////////////////////////////////////////////

//-t 10 -g ../../../examples/test_extensions/data/us.topojson -d ../../../examples/test_extensions/data/us_presidential_election_results_by_county.csv 

int read_counties_elections(const std::string& file_name);
void draw_geometry(Geometry_t* geometry, WLeaflet* leaflet,
  const std::string& fill_color, const std::string& stroke_color,
  double opacity, double stroke_weight);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//forward declarations
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_dc311(const std::string& file_name);
int read_ep_pop(const std::string& file_name_ep, const std::string& file_name_pop);
std::string to_hex(int n);

///////////////////////////////////////////////////////////////////////////////////////
//globals
///////////////////////////////////////////////////////////////////////////////////////

std::string test;
geojson_t geojson;
topojson_t topojson;
std::vector<dc311_data_t> dc311_data;
std::vector<ep_data_t> ep_data;
std::vector<us_state_pop_t> us_state_pop;

///////////////////////////////////////////////////////////////////////////////////////
//Application_test
//38.9072, -77.0369, 14 DC
//37.0902, -95.7129, 5 US
///////////////////////////////////////////////////////////////////////////////////////

class Application_test : public WApplication
{
public:
  Application_test(const WEnvironment& env) : WApplication(env)
  {
    setTitle("test");
    useStyleSheet("boxes.css");
    root()->setStyleClass("yellow-box");
    auto hbox = root()->setLayout(Wt::cpp14::make_unique<Wt::WHBoxLayout>());
    std::unique_ptr<WLeaflet> leaflet = cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 38.9072, -77.0369, 13);
    leaflet->setStyleClass("green-box");
    WLength len = leaflet->height();
    leaflet->resize(800, len);
    hbox->addWidget(std::move(leaflet));
    std::unique_ptr<WText> text = Wt::cpp14::make_unique<Wt::WText>("item 2");
    text->setStyleClass("blue-box");
    hbox->addWidget(std::move(text));
  }
};

//////////////////////////////////////////////////////////////////////////////////////
//Application_update
///////////////////////////////////////////////////////////////////////////////////////

class Application_update : public WApplication
{
public:
  Application_update(const WEnvironment& env) : WApplication(env)
  {
    std::unique_ptr<PushWidget> update = cpp14::make_unique<PushWidget>();
    root()->addWidget(std::move(update));
  }
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_counties
///////////////////////////////////////////////////////////////////////////////////////

class Application_counties : public Wt::WApplication
{
public:
  Application_counties(const Wt::WEnvironment& env, topojson_t& rtopojson);
};

///////////////////////////////////////////////////////////////////////////////////////
//Application_counties
//counties_votes_t
///////////////////////////////////////////////////////////////////////////////////////

class counties_votes_t
{
public:
  counties_votes_t(std::string name_, std::string state_, int fips_, const std::vector<int>& votes_,
    double per) :
    name(name_),
    state(state_),
    fips(fips_),
    votes(votes_),
    perc_vote(per)
  {
  };
  std::vector<int> votes;
  std::string name;
  std::string state;
  double perc_vote; //percent for vote 2 (red)
  int fips;
  void build_geometry(size_t topology_index);
  //geometry for this county found by FIPS on topology
  std::vector<double> lat;
  std::vector<double> lon;
};
std::vector<counties_votes_t> counties_votes;

///////////////////////////////////////////////////////////////////////////////////////
//counties_votes_t::build_geometry
///////////////////////////////////////////////////////////////////////////////////////

void counties_votes_t::build_geometry(size_t topology_index)
{
  size_t size_geom = topojson.m_topology.at(topology_index).m_geom.size();
  for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
  {
    Geometry_t geometry = topojson.m_topology.at(topology_index).m_geom.at(idx_geom);
    int id = std::stoi(geometry.id);
    if (id == fips)
    {
      size_t size_pol = geometry.m_polygon.size();
      for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
      {
        Polygon_topojson_t polygon = geometry.m_polygon.at(idx_pol);
        size_t size_arcs = polygon.arcs.size();
        size_t size_points = geometry.m_polygon.at(idx_pol).m_y.size();
        for (size_t idx_crd = 0; idx_crd < size_points; idx_crd++)
        {
          lat.push_back(geometry.m_polygon.at(idx_pol).m_y.at(idx_crd));
          lon.push_back(geometry.m_polygon.at(idx_pol).m_x.at(idx_crd));
        }
      }//size_pol
      return;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//read_counties_elections
//,county_name,state_abbr,votes_total,votes_dem,votes_gop,combined_fips,per_dem,per_gop
/////////////////////////////////////////////////////////////////////////////////////////////////////

int read_counties_elections(const std::string& file_name)
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
    std::string state = row.at(2);
    std::vector<int> votes;
    votes.push_back(std::stoi(row.at(4)));
    votes.push_back(std::stoi(row.at(5)));
    int fips = std::stoi(row.at(6));
    double perc_vote_2 = -1;
    try
    {
      perc_vote_2 = std::stod(row.at(8));
    }
    catch (std::exception& e)
    {
      std::cerr << "exception: " << e.what() << std::endl;
    }
    counties_votes_t cv(name, state, fips, votes, perc_vote_2);
    counties_votes.push_back(cv);
  }

  std::cout << "\n";
  std::cout << "Read " << rows << " rows" << "\n";
  csv.m_ifs.close();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //must make the coordinates for the topology first
  //3 objects: counties (0), states (1), land (2) 
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  topojson.make_coordinates(0);
  topojson.make_coordinates(1);

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //build geometry
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  size_t size_votes = counties_votes.size();
  std::cout << "Building geometry...";
  for (size_t idx_vot = 0; idx_vot < size_votes / total; idx_vot++)
  {
    counties_votes.at(idx_vot).build_geometry(0);
  }
  std::cout << "done\n";
  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_counties
///////////////////////////////////////////////////////////////////////////////////////

Application_counties::Application_counties(const WEnvironment& env, topojson_t& rtopojson) :
  WApplication(env)
{
  double opacity = 0;
  double stroke_weight = 0;
  std::string stroke_color = rgb_to_hex(0, 0, 0);
  setTitle("US elections 2016");
  WLeaflet* leaflet = root()->addWidget(cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, 37.0902, -95.7129, 5));
  leaflet->Circle(37.0902, -95.7129, 20000, "#ff0000");


  ///////////////////////////////////////////////////////////////////////////////////////
  //render states
  ///////////////////////////////////////////////////////////////////////////////////////

  int draw_states = 1;
  if (draw_states)
  {
    size_t topology_index = 1;
    //draw states only border 
    opacity = 0;
    stroke_weight = 1;
    std::string stroke_color = rgb_to_hex(0, 0, 0);
    topology_object_t topology = topojson.m_topology.at(topology_index);
    size_t size_geom = topojson.m_topology.at(topology_index).m_geom.size();
    for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
    {
      Geometry_t geometry = topology.m_geom.at(idx_geom);
      draw_geometry(&geometry, leaflet, "#000000", stroke_color, opacity, stroke_weight);
    }
  }

  Wt::WStringStream strm;
  strm
    << "var legend = L.control({ position: 'bottomright' });"
    << "legend.onAdd = function(map)"
    << "{"
    << "var div = L.DomUtil.create('div', 'info legend'),"
    << "labels = [];";
  for (size_t idx_clr = 0; idx_clr < gbr.size(); idx_clr++)
  {
    std::string label;
    size_t idx = idx_clr;
    if (idx_clr < gbr.size() / 2)
    {
      idx = gbr.size() - idx_clr - 1;
    }
    double per = gbr.at(idx).grade * 100;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << per;
    label += ss.str();
    label += "%";
    strm
      << "labels.push('<i style=\"background:" << gbr.at(idx_clr).clr << "\"></i>" << label << "');";
  }
  strm
    << "div.innerHTML = labels.join('<br>');"
    << "return div;"
    << "};"
    << "legend.addTo(" << leaflet->jsRef() << ".map);";
  leaflet->do_javascript(strm.str());


  ///////////////////////////////////////////////////////////////////////////////////////
  //render counties
  ///////////////////////////////////////////////////////////////////////////////////////

  opacity = 0.8;
  stroke_weight = 0.3;
  stroke_color = rgb_to_hex(51, 51, 51);
  size_t size_votes = counties_votes.size();
  for (size_t idx_vot = 0; idx_vot < size_votes; idx_vot++)
  {
    counties_votes_t cv = counties_votes.at(idx_vot);
    if (cv.lat.size() == 0)
    {
      return;
    }
    Wt::WStringStream text;
    text << cv.name << ", " << cv.state << "<br>"
      << "Hillary Clinton: " << std::to_string(cv.votes[0]) << "<br>"
      << "Donald J. Trump: " << std::to_string(cv.votes[1]) << "<br>"
      << "Total Votes: " << std::to_string(cv.votes[0] + cv.votes[1]);
    polygon_opt opt;
    opt.tooltip = text.str();
    leaflet->Polygon(cv.lat,
      cv.lon,
      get_color(cv.perc_vote),
      stroke_color,
      opacity,
      stroke_weight,
      &opt);
  }
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_geotiff
///////////////////////////////////////////////////////////////////////////////////////

Application_geotiff::Application_geotiff(const Wt::WEnvironment& env) :
  WApplication(env)
{
  setTitle("geotiff");
  //Sidney 33.8688° S, 151.2093° E
  WLeaflet* leaflet = root()->addWidget(cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, -33.8688, 151.2093, 5));

  Wt::WStringStream strm;
  strm
    << "const url = 'tiff/wind_speed.tif';"
    << "const plottyRenderer = L.LeafletGeotiff.plotty({"
    << "displayMin: 0,"
    << "displayMax: 10,"
    << "clampLow: false,"
    << "clampHigh: false,"
    << "});"
    << "const layer = L.leafletGeotiff(url, {"
    << "renderer: plottyRenderer,"
    << "}).addTo(" << leaflet->jsRef() << ".map);";
  leaflet->do_javascript(strm.str());
}

///////////////////////////////////////////////////////////////////////////////////////
//Application_image
///////////////////////////////////////////////////////////////////////////////////////

Application_image::Application_image(const Wt::WEnvironment& env) :
  WApplication(env)
{
  setTitle("image overlay");
  //Sidney 33.8688° S, 151.2093° E
  WLeaflet* leaflet = root()->addWidget(cpp14::make_unique<WLeaflet>(tile_provider_t::CARTODB, -33.8688, 151.2093, 5));

  Wt::WStringStream strm;
  strm
    << "const url = 'images/rose.png';"
    << "bounds = [[-31, 149], [-35, 153]];"
    << "L.imageOverlay(url, bounds).addTo("
    << leaflet->jsRef() << ".map);";

  leaflet->do_javascript(strm.str());
}



///////////////////////////////////////////////////////////////////////////////////////
//create_application
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  if (test.compare("1") == 0)
  {
    return cpp14::make_unique<Application_test>(env);
  }
  else if (test.compare("2") == 0)
  {
    return cpp14::make_unique<Application_dc311>(env, geojson);
  }
  else if (test.compare("3") == 0)
  {
    return cpp14::make_unique<Application_us_states>(env, geojson);
  }
  else if (test.compare("4") == 0)
  {
    return cpp14::make_unique<Application_atms>(env);
  }
  else if (test.compare("5") == 0)
  {
    return cpp14::make_unique<Application_ep>(env, geojson);
  }
  else if (test.compare("6") == 0)
  {
    return cpp14::make_unique<Application_schools>(env, geojson);
  }
  else if (test.compare("7") == 0)
  {
    return cpp14::make_unique<Application_topojson>(env, topojson);
  }
  else if (test.compare("8") == 0)
  {
    return cpp14::make_unique<Application_celsium_atms>(env);
  }
  else if (test.compare("9") == 0)
  {
    return cpp14::make_unique<Application_update>(env);
  }
  else if (test.compare("10") == 0)
  {
    return cpp14::make_unique<Application_counties>(env, topojson);
  }
  else if (test.compare("11") == 0)
  {
    return cpp14::make_unique<Application_geotiff>(env);
  }
  else if (test.compare("12") == 0)
  {
    return cpp14::make_unique<Application_image>(env);
  }
  assert(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//usage
/////////////////////////////////////////////////////////////////////////////////////////////////////

void usage()
{
  std::cout << "usage: ./test_extensions.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. ";
  std::cout << "-t TEST <-d DATABASE> <-u DATABASE> <-g GEOJSON> <-z GEOJSON> ";
  std::cout << std::endl;
  std::cout << "-t TEST: test number (1 to 6)" << std::endl;
  std::cout << "-d DATABASE: data file" << std::endl;
  std::cout << "-g GEOJSON: geojson file" << std::endl;
  std::cout << "-u DATABASE: data file" << std::endl;
  std::cout << "-z GEOJSON: data file" << std::endl;
  exit(0);
}



///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  std::string data_file;
  std::string data_file_us_states_pop;
  std::string geojson_file;
  std::string zip_geojson_file;


  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 't':
        test = argv[i + 1];
        i++;
        break;
      case 'd':
        data_file = argv[i + 1];
        std::cout << data_file << "\n";
        i++;
        break;
      case 'u':
        data_file_us_states_pop = argv[i + 1];
        std::cout << data_file_us_states_pop << "\n";
        i++;
        break;
      case 'g':
        geojson_file = argv[i + 1];
        std::cout << geojson_file << "\n";
        i++;
        break;
      case 'm':
        file_wmata_stations = argv[i + 1];
        std::cout << file_wmata_stations << "\n";
        i++;
        break;
      case 'z':
        zip_geojson_file = argv[i + 1];
        std::cout << zip_geojson_file << "\n";
        i++;
        break;
      }
    }
  }

  if (test.empty())
  {
    usage();
  }

  if (test.compare("1") == 0)
  {

  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //dc_311 test, read CSV data file with -d, geojson with -g 
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("2") == 0)
  {
    if (data_file.empty())
    {
      usage();
    }
    if (read_dc311(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //geojson test, read geojson file with -f 
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("3") == 0)
  {
    if (geojson_file.empty())
    {
      usage();
      exit(1);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //NOAA ATMS star json file
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("4") == 0)
  {
    if (data_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_datasets(data_file.c_str(), datasets) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //ep
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("5") == 0)
  {
    if (data_file.empty() || data_file_us_states_pop.empty() || geojson_file.empty())
    {
      usage();
      exit(1);
    }

    std::cout << data_file << std::endl;
    std::cout << data_file_us_states_pop << std::endl;
    std::cout << geojson_file << std::endl;

    if (data_file.empty() || geojson_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_ep_pop(data_file, data_file_us_states_pop) < 0)
    {
      assert(0);
    }
    if (geojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  ///////////////////////////////////////////////////////////////////////////////////////
  //schools
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("6") == 0)
  {
    std::cout << data_file << std::endl;
    std::cout << geojson_file << std::endl;
    std::cout << zip_geojson_file << std::endl;
    if (read_schools(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
    if (read_json_montgomery_county(geojson_file.c_str(), lat_montgomery, lon_montgomery) < 0)
    {
      assert(0);
      exit(1);
    }
    if (read_json_wmata(file_wmata_stations, lat_wmata, lon_wmata) < 0)
    {
      assert(0);
      exit(1);
    }
    if (geojson.convert(zip_geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //topojson sample
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("7") == 0)
  {
    std::cout << geojson_file << std::endl;
    if (topojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //cesium
  ///////////////////////////////////////////////////////////////////////////////////////

  else if (test.compare("8") == 0)
  {
    std::cout << data_file << std::endl;
    if (read_datasets(data_file.c_str(), datasets) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  else if (test.compare("9") == 0)
  {
    std::cout << data_file << std::endl;
    if (data_file.empty())
    {
      usage();
      exit(1);
    }
    if (read_datasets(data_file.c_str(), datasets) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  else if (test.compare("10") == 0)
  {
    std::cout << geojson_file << std::endl;
    if (topojson.convert(geojson_file.c_str()) < 0)
    {
      assert(0);
      exit(1);
    }
    if (read_counties_elections(data_file) < 0)
    {
      assert(0);
      exit(1);
    }
  }
  else if (test.compare("11") == 0)
  {
  }
  else if (test.compare("12") == 0)
  {
  }
  else
  {
    assert(0);
    exit(1);
  }
  return WRun(argc, argv, &create_application);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//Application_counties
//get a geometry on "id" input; used to get coordinates for "id"
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get_geometry_id(size_t idx_topo, std::string id, Geometry_t& g)
{
  size_t size_geom = topojson.m_topology.at(idx_topo).m_geom.size();
  for (size_t idx_geom = 0; idx_geom < size_geom; idx_geom++)
  {
    Geometry_t geometry = topojson.m_topology.at(idx_topo).m_geom.at(idx_geom);
    if (id.compare(geometry.id) == 0)
    {
      g = geometry;
      return 1;
    }
  }
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////
//Application_counties
//draw_geometry
///////////////////////////////////////////////////////////////////////////////////////

void draw_geometry(Geometry_t* geometry, WLeaflet* leaflet, const std::string& fill_color,
  const std::string& stroke_color, double opacity, double stroke_weight)
{
  if (geometry->type.compare("Polygon") == 0 || geometry->type.compare("MultiPolygon") == 0)
  {
    size_t size_pol = geometry->m_polygon.size();
    for (size_t idx_pol = 0; idx_pol < size_pol; idx_pol++)
    {
      Polygon_topojson_t polygon = geometry->m_polygon.at(idx_pol);
      size_t size_arcs = polygon.arcs.size();

      ///////////////////////////////////////////////////////////////////////////////////////
      //render each polygon as a vector of vertices passed to Polygon
      ///////////////////////////////////////////////////////////////////////////////////////

      std::vector<double> lat;
      std::vector<double> lon;
      size_t size_points = geometry->m_polygon.at(idx_pol).m_y.size();
      for (size_t idx_crd = 0; idx_crd < size_points; idx_crd++)
      {
        lat.push_back(geometry->m_polygon.at(idx_pol).m_y.at(idx_crd));
        lon.push_back(geometry->m_polygon.at(idx_pol).m_x.at(idx_crd));
      }
      leaflet->Polygon(lat, lon, fill_color, stroke_color, opacity, stroke_weight, nullptr);

    }//size_pol
  }//"Polygon"
}




