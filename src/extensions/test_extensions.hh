#ifndef WLEAFLET_TEST_H_
#define WLEAFLET_TEST_H_

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <Wt/WPushButton.h>
#include <Wt/WStringListModel.h>
#include <Wt/WTemplate.h>
#include <Wt/WDateTime.h>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <Wt/WContainerWidget.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
#include "gason.h"
#include "WLeaflet.hh"
#include "WPlotly.hh"
#include "WCesium.hh"
#include "csv.hh"
#include "geojson.hh"
#include "topojson.hh"
#include "star_json.hh"
#include "star_dataset.hh"
#include "pal_rgb.h"

star_dataset_t find_dataset(std::string name);
std::string rgb_to_hex(int r, int g, int b);
size_t find_ep(std::string state_name);
size_t find_population(std::string state_name);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//rgb_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class rgb_t
{
public:
  rgb_t(unsigned char r, unsigned char g, unsigned char b) :
    red(r),
    green(g),
    blue(b)
  {
  }
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

///////////////////////////////////////////////////////////////////////////////////////
//dc311_data_t
///////////////////////////////////////////////////////////////////////////////////////

class dc311_data_t
{
public:
  dc311_data_t(const std::string &date_,
    const std::string &lat_,
    const std::string &lon_,
    const std::string &zip_) :
    date(date_),
    lat(lat_),
    lon(lon_),
    zip(zip_)
  {
  };
  std::string date;
  std::string lat;
  std::string lon;
  std::string zip;
};

///////////////////////////////////////////////////////////////////////////////////////
//ep_data_t
///////////////////////////////////////////////////////////////////////////////////////

class ep_data_t
{
public:
  ep_data_t(std::string state_, size_t ep_) :
    state(state_),
    ep(ep_)
  {
  };
  std::string state;
  size_t ep;
};

///////////////////////////////////////////////////////////////////////////////////////
//us_state_pop_t
///////////////////////////////////////////////////////////////////////////////////////

class us_state_pop_t
{
public:
  us_state_pop_t(std::string state_, size_t population_) :
    state(state_),
    population(population_)
  {
  };
  std::string state;
  size_t population;
};


///////////////////////////////////////////////////////////////////////////////////////
//school_t
///////////////////////////////////////////////////////////////////////////////////////

class school_t
{
public:
  school_t(std::string name_, std::string lat_, std::string lon_, int rating_) :
    name(name_),
    lat(lat_),
    lon(lon_),
    rating(rating_)
  {
  };
  std::string name;
  std::string lat;
  std::string lon;
  int rating;
};

///////////////////////////////////////////////////////////////////////////////////////
//wmata_station_t
///////////////////////////////////////////////////////////////////////////////////////

class wmata_station_t
{
public:
  wmata_station_t(const std::string &lat_,
    const std::string &lon_,
    const std::string &name_,
    const std::string &line_code_) :
    lat(lat_),
    lon(lon_),
    name(name_),
    line_code(line_code_)
  {
  };
  std::string lat;
  std::string lon;
  std::string name;
  std::string line_code;
};


class Application_celsium_atms : public Wt::WApplication
{
public:
  Application_celsium_atms(const Wt::WEnvironment& env);
private:
  std::vector<rgb_t> m_rgb_256;
};

class Application_atms : public Wt::WApplication
{
public:
  Application_atms(const Wt::WEnvironment& env);
private:
  std::vector<rgb_t> m_rgb_256;
};

class Application_dc311 : public Wt::WApplication
{
public:
  Application_dc311(const Wt::WEnvironment& env, const geojson_t &rgeojson);
};

class Application_geotiff : public Wt::WApplication
{
public:
  Application_geotiff(const Wt::WEnvironment& env);
};

class Application_image : public Wt::WApplication
{
public:
  Application_image(const Wt::WEnvironment& env);
};


class Application_us_states : public Wt::WApplication
{
public:
  Application_us_states(const Wt::WEnvironment& env, const geojson_t &rgeojson);
private:
  std::vector<rgb_t> m_rgb_256;
};


class Application_ep : public Wt::WApplication
{
public:
  Application_ep(const Wt::WEnvironment& env, const geojson_t &rgeojson);
};

class Application_schools : public Wt::WApplication
{
public:
  Application_schools(const Wt::WEnvironment& env, const geojson_t &rgeojson);
};


///////////////////////////////////////////////////////////////////////////////////////
//Application_topojson
///////////////////////////////////////////////////////////////////////////////////////

class Application_topojson : public Wt::WApplication
{
public:
  Application_topojson(const Wt::WEnvironment& env, topojson_t &rtopojson);
};

///////////////////////////////////////////////////////////////////////////////////////
//PushWidget
///////////////////////////////////////////////////////////////////////////////////////

class PushWidget : public Wt::WContainerWidget
{
public:
  PushWidget() :
    Wt::WContainerWidget(),
    m_iter(0)
  {
    m_text = this->addWidget(Wt::cpp14::make_unique<Wt::WText>(Wt::asString(m_iter)));
    Wt::WApplication *app = Wt::WApplication::instance();
    app->enableUpdates(true);
    if (m_listen_thread.joinable())
    {
      m_listen_thread.join();
    }
    m_listen_thread = std::thread(std::bind(&PushWidget::listen, this, app));
  }

  virtual ~PushWidget()
  {
    if (m_listen_thread.get_id() != std::this_thread::get_id() && m_listen_thread.joinable())
    {
      m_listen_thread.join();
    }
  }

private:
  Wt::WText *m_text;
  size_t m_iter;
  std::thread m_listen_thread;

  void listen(Wt::WApplication *app)
  {
    while (true)
    {
      std::this_thread::sleep_for(std::chrono::seconds(3));
      Wt::WApplication::UpdateLock uiLock(app);
      if (uiLock)
      {
        m_iter++;
        m_text->setText(Wt::asString(m_iter));
        app->triggerUpdate();
      }
      else
      {
        return;
      }
    }
  }
};

#endif
