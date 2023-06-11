#include "Wt/WLogger.h"
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPointF.h>
#include "web/WebUtils.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>
#include "WLeaflet.hh"

#ifdef _MSC_VER
bool local = true;
#else
bool local = false;
#endif

namespace Wt
{

  ///////////////////////////////////////////////////////////////////////////////////////
  //Coordinate
  ///////////////////////////////////////////////////////////////////////////////////////

  WLeaflet::Coordinate::Coordinate() :
    m_lat(0),
    m_lon(0)
  {
  }

  WLeaflet::Coordinate::Coordinate(double lat, double lon)
  {
    setLatitude(lat);
    setLongitude(lon);
  }

  WLeaflet::Coordinate::Coordinate(const std::pair<double, double>& lat_long)
  {
    setLatitude(lat_long.first);
    setLongitude(lat_long.second);
  }

  void WLeaflet::Coordinate::setLatitude(double latitude)
  {
    m_lat = latitude;
  }

  void WLeaflet::Coordinate::setLongitude(double longitude)
  {
    m_lon = longitude;
  }

  std::pair<double, double> WLeaflet::Coordinate::operator ()() const
  {
    return std::make_pair(m_lat, m_lon);
  }

  std::istream& operator >> (std::istream& i, WLeaflet::Coordinate& c)
  {
    double lat, lon;
    i >> lat >> std::ws >> lon;
    c.setLatitude(lat);
    c.setLongitude(lon);
    return i;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::WLeaflet
  ///////////////////////////////////////////////////////////////////////////////////////

  WLeaflet::WLeaflet(tile_provider_t tile, double lat, double lon, int zoom) :
    m_tile(tile),
    m_lat(lat),
    m_lon(lon),
    m_zoom(zoom),
    m_clicked(this, "click"),
    m_double_clicked(this, "dblclick")
  {
    auto container{ new Wt::WContainerWidget };
    setImplementation(std::unique_ptr<WWidget>(container));
    container->mouseWentDown().connect(this, &WLeaflet::MouseDown);
    this->addCssRule("html", "height: 100%");
    this->addCssRule("body", "height: 100%");
    this->addCssRule("#" + id(), "position:relative; top:0; bottom:0; height: 100%");
    //add style for 'legend' and 'info'
    this->addCssRule(".info", "padding: 6px 8px; background: white; border-radius: 5px;");
    this->addCssRule(".legend", "text-align: left; line-height: 18px; color: #555;");
    this->addCssRule(".legend i", "width: 18px; height: 18px; float: left; margin-right: 8px;");
    WApplication* app = WApplication::instance();
    std::string leaflet = "leaflet.js";
    std::string mapbox_gl = "mapbox-gl.js";
    std::string leaflet_mapbox_gl = "leaflet-mapbox-gl.js";

    //geotiff
    std::string geotiff_js;
    std::string plotty_js;
    std::string leaflet_geotiff_js = "leaflet-geotiff.js";
    std::string leaflet_geotiff_plotty_js = "leaflet-geotiff-plotty.js";

    geotiff_js = "https://unpkg.com/geotiff@1.0.0-beta.13/dist-browser/geotiff.js";
    plotty_js = "https://unpkg.com/plotty@0.4.1/dist/plotty.js";

    leaflet_geotiff_js = "https://unpkg.com/leaflet-geotiff-2@0.2.5/dist/leaflet-geotiff.js";
    leaflet_geotiff_plotty_js = "https://unpkg.com/leaflet-geotiff-2@0.2.5/dist/leaflet-geotiff-plotty.js";

    if (local)
    {
      app->useStyleSheet("leaflet.css");
      app->useStyleSheet("mapbox-gl.css");

      leaflet = "leaflet.js";
      mapbox_gl = "mapbox-gl.js";
      leaflet_mapbox_gl = "leaflet-mapbox-gl.js";
    }
    else
    {
      app->useStyleSheet("https://unpkg.com/leaflet@1.0.3/dist/leaflet.css");
      app->useStyleSheet("https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.css");

      leaflet = "https://unpkg.com/leaflet@1.0.3/dist/leaflet.js";
      mapbox_gl = "https://api.tiles.mapbox.com/mapbox-gl-js/v0.35.1/mapbox-gl.js";
      leaflet_mapbox_gl = "https://rawgit.com/mapbox/mapbox-gl-leaflet/master/leaflet-mapbox-gl.js";
    }

    app->require(leaflet, "leaflet");
    if (m_tile == tile_provider_t::MAPBOX)
    {
      app->require(mapbox_gl, "mapbox_gl");
      app->require(leaflet_mapbox_gl, "leaflet_mapbox_gl");
    }

    app->require(geotiff_js, "geotiff_js");
    app->require(plotty_js, "plotty_js");
    app->require(leaflet_geotiff_js, "leaflet_geotiff_js");
    app->require(leaflet_geotiff_plotty_js, "leaflet_geotiff_plotty_js");

  }

  void WLeaflet::require_geocoder()
  {
    WApplication* app = WApplication::instance();
    std::string leaflet_geocoder = "Control.Geocoder.js";
    std::string leaflet_locate = "L.Control.Locate.js";
    if (local)
    {
      app->useStyleSheet("Control.Geocoder.css");
      app->useStyleSheet("https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css");
      app->useStyleSheet("L.Control.Locate.css");
      app->useStyleSheet("L.Control.Locate.mapbox.css");
    }
    else
    {
      app->useStyleSheet("https://unpkg.com/leaflet-control-geocoder/dist/Control.Geocoder.css");
      app->useStyleSheet("https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css");
      app->useStyleSheet("https://cdn.jsdelivr.net/npm/leaflet.locatecontrol@0.65.1/dist/L.Control.Locate.min.css");
      leaflet_geocoder = "https://unpkg.com/leaflet-control-geocoder/dist/Control.Geocoder.js";
      leaflet_locate = "https://cdn.jsdelivr.net/npm/leaflet.locatecontrol@0.65.1/dist/L.Control.Locate.min.js";
    }
    app->require(leaflet_geocoder, "leaflet_geocoder");
    app->require(leaflet_locate, "leaflet_locate");
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::MouseDown
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::MouseDown(const Wt::WMouseEvent& e)
  {
    Wt::WPointF p = Wt::WPointF(e.widget().x, e.widget().y);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::add_geocoder
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::add_geocoder()
  {
    Wt::WStringStream strm;
    strm
      << " L.Control.geocoder().addTo(" << jsRef() << ".map);\n";
    strm
      << "L.control.locate().addTo(" << jsRef() << ".map);\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::render(WFlags<RenderFlag> flags)
  {
    if (flags.test(RenderFlag::Full))
    {
      Wt::WApplication* app = Wt::WApplication::instance();
      Wt::WString initFunction = app->javaScriptClass() + ".init_leaflet_" + id();
      Wt::WStringStream strm;
      std::string mapbox_key;
      bool read = Wt::WApplication::readConfigurationProperty("mapbox_api_key", mapbox_key);

      std::string str_z = std::to_string((int)m_zoom);
      std::string ll;
      ll = std::to_string((long double)m_lat);
      ll += ",";
      ll += std::to_string((long double)m_lon);

      strm
        << "{ " << initFunction.toUTF8() << " = function() {\n"
        << "  var self = " << jsRef() << ";\n"
        << "  if (!self) {\n"
        << "    setTimeout(" << initFunction.toUTF8() << ", 0);\n"
        << "  }\n";

      if (m_tile == tile_provider_t::RRZE)
      {
        strm
          << "  var layer_base = L.tileLayer(\n"
          << "  'http://{s}.osm.rrze.fau.de/osmhd/{z}/{x}/{y}.png',{\n"
          << "  attribution: '<a href=http://www.openstreetmap.org/copyright>(C) Openstreetmap Contributors</a>'\n"
          << "  });\n";
      }
      else if (m_tile == tile_provider_t::CARTODB)
      {
        strm
          << "  var layer_base = L.tileLayer(\n"
          << "  'http://cartodb-basemaps-{s}.global.ssl.fastly.net/light_all/{z}/{x}/{y}@2x.png',{\n"
          << "  opacity: 1,\n"
          << "  attribution: '&copy; <a href=http://www.openstreetmap.org/copyright>OpenStreetMap</a>, &copy; <a href=https://carto.com/attribution>CARTO</a>'\n"
          << "  });\n";
      }
      else if (m_tile == tile_provider_t::MAPBOX)
      {
        strm
          << "  var map = L.map(self).setView([" << ll << "], " << str_z << ");\n";
        strm
          << "  var gl = L.mapboxGL({\n"
          << "  accessToken:'" << mapbox_key << "',\n"
          << "  style: 'mapbox://styles/mapbox/streets-v9'\n"
          << "  }).addTo(map);\n";
      }

      if (m_tile == tile_provider_t::CARTODB || m_tile == tile_provider_t::RRZE)
      {
        strm
          << "  var map = new L.Map(self, {\n"
          << "  center: new L.LatLng(" << ll << "), \n"
          << "  zoom: " << str_z << ",\n"
          << "  scrollWheelZoom: false,\n"
          << "  layers: [layer_base]\n"
          << "  });\n";
      }

      strm << "self.map = map;";

      stream_listener(m_clicked, "click", strm);
      stream_listener(m_double_clicked, "dblclick", strm);

      for (size_t idx = 0; idx < m_additions.size(); idx++)
      {
        strm << m_additions[idx];
      }

      strm
        << "  setTimeout(function(){ delete " << initFunction.toUTF8() << ";}, 0)};\n"
        << "}\n"
        << initFunction.toUTF8() << "();\n";

      m_additions.clear();
      app->doJavaScript(strm.str(), true);
    }

    Wt::WCompositeWidget::render(flags);
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::do_javascript
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::do_javascript(const std::string& jscode)
  {
    if (isRendered())
    {
      doJavaScript(jscode);
    }
    else
    {
      m_additions.push_back(jscode);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::stream_listener
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::stream_listener(const JSignal<Coordinate>& signal, std::string signalName, Wt::WStringStream& strm)
  {
    strm <<
      "map.on('" << signalName << "', function(event) { "
      << signal.createCall({ "event.latlng.lat +' '+ event.latlng.lng" })
      << ";"
      "});";

  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const std::string& lat, const std::string& lon)
  {
    Wt::WStringStream strm;
    strm
      << " var opt = {radius: 100, stroke: false, color: '#ff0000'};\n";
    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(" << jsRef() << ".map);\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const double lat, const double lon, const std::string& color)
  {
    Wt::WStringStream strm;
    strm
      << " var opt = {radius: 100, stroke: false, color: '"
      << color
      << "'};\n";
    std::string str_ll;
    str_ll = std::to_string((long double)lat);
    str_ll += ",";
    str_ll += std::to_string((long double)lon);
    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(" << jsRef() << ".map);\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const std::string& lat, const std::string& lon, const std::string& color)
  {
    Wt::WStringStream strm;
    strm
      << " var opt = {radius: 1000, stroke: false, color: '"
      << color
      << "'};\n";
    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(" << jsRef() << ".map);\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Circle
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Circle(const double lat, const double lon, const double radius, const std::string& color)
  {
    Wt::WStringStream strm;
    std::string str_clr = std::to_string((long double)radius);
    strm
      << " var opt = {radius:"
      << radius
      << ", stroke: false, color: '"
      << color
      << "'};\n";
    std::string str_ll;
    str_ll = std::to_string((long double)lat);
    str_ll += ",";
    str_ll += std::to_string((long double)lon);
    strm
      << " var c = new L.circle([" << str_ll << "], opt).addTo(" << jsRef() << ".map);\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Polygon
  //draw a weight of 1 pixel and opacity .1 for border 
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Polygon(const std::vector<double>& lat, const std::vector<double>& lon, const std::string& color)
  {
    Wt::WStringStream strm;
    if (lat.size() == 0)
    {
      return;
    }
    strm
      << "var vert = [ [";
    for (size_t idx = 0; idx < lat.size() - 1; idx++)
    {
      strm
        << std::to_string((long double)lat.at(idx))
        << ","
        << std::to_string((long double)lon.at(idx))
        << "], [";
    }
    //last
    size_t idx = lat.size() - 1;
    strm
      << std::to_string((long double)lat.at(idx))
      << ","
      << std::to_string((long double)lon.at(idx))
      << "] ];";
    strm
      << "L.polygon(vert,{color:'"
      << color
      << "',opacity:.1,weight:1}).addTo(" << jsRef() << ".map);";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Polygon
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Polygon(const std::vector<double>& lat,
    const std::vector<double>& lon,
    const std::string& fill_color,
    const std::string& stroke_color,
    double fill_opacity,
    double stroke_weight,
    polygon_opt* opt)
  {
    Wt::WStringStream strm;
    if (lat.size() == 0)
    {
      return;
    }
    strm
      << "var vert = [ [";
    for (size_t idx = 0; idx < lat.size() - 1; idx++)
    {
      strm
        << std::to_string((long double)lat.at(idx))
        << ","
        << std::to_string((long double)lon.at(idx))
        << "], [";
    }
    //last
    size_t idx = lat.size() - 1;
    strm
      << std::to_string((long double)lat.at(idx))
      << ","
      << std::to_string((long double)lon.at(idx))
      << "] ];";
    strm
      << "var p=L.polygon(vert,{color:'" << stroke_color << "'" //stroke color
      << ",weight:" << stroke_weight //stroke width in pixels
      << ",fillColor:'" << fill_color << "'" //fill color. Defaults to the value of the color option
      << ",fillOpacity:" << fill_opacity //fill opacity (default is 0.2)
      << "}).addTo(" << jsRef() << ".map);";
    //tooltip is default empty
    if (opt && opt->tooltip.size())
    {
      strm
        //use bacticks for enclosing text string because of single quotes in names 
        << "p.bindTooltip(`" << opt->tooltip << "`);";
    }
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const std::string& lat, const std::string& lon, const std::string& text)
  {
    Wt::WStringStream strm;
    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " L.marker([" << str_ll << "]).addTo(" << jsRef() << ".map)"
      << ".bindPopup('"
      << text
      << "');\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const std::string& lat, const std::string& lon, const std::string& text, marker_icon_t icon)
  {
    Wt::WStringStream strm;
    strm
      << "var custom_icon = new L.Icon({"
      << "iconUrl: '"
      << icon.icon_url
      << "',"
      << "shadowUrl: '"
      << icon.shadow_url
      << "',"
      << "iconSize: [25, 41],"
      << "iconAnchor: [12, 41],"
      << "popupAnchor: [1, -34],"
      << "shadowSize: [41, 41]"
      << "});\n";
    std::string str_ll;
    str_ll = lat;
    str_ll += ",";
    str_ll += lon;
    strm
      << " L.marker([" << str_ll << "], {icon: custom_icon}).addTo(" << jsRef() << ".map)"
      << ".bindPopup('"
      << text
      << "');\n";
    do_javascript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WLeaflet::Marker
  ///////////////////////////////////////////////////////////////////////////////////////

  void WLeaflet::Marker(const double lat, const double lon, const std::string& text, marker_icon_t icon)
  {
    std::string ll;
    ll = std::to_string((long double)lat);
    ll += ",";
    ll += std::to_string((long double)lon);
    Wt::WStringStream strm;
    strm
      << "var custom_icon = new L.Icon({"
      << "iconUrl: '"
      << icon.icon_url
      << "',"
      << "shadowUrl: '"
      << icon.shadow_url
      << "',"
      << "iconSize: [25, 41],"
      << "iconAnchor: [12, 41],"
      << "popupAnchor: [1, -34],"
      << "shadowSize: [41, 41]"
      << "});\n";
    strm
      << " L.marker([" << ll << "], {icon: custom_icon}).addTo(" << jsRef() << ".map)"
      << ".bindPopup('"
      << text
      << "');\n";
    do_javascript(strm.str());
  }
}
