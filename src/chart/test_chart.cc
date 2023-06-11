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
#include "WPlotly.hh"
#include <Wt/WContainerWidget.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTimer.h>
#include <Wt/Http/Client.h>
#include <Wt/Http/Response.h>
#include <Wt/Http/ResponseContinuation.h>
#include <Wt/Http/Request.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Serializer.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Array.h>
#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <fstream>
#include <cstring>
#include <cmath>
#include "chart_model.hh"
using namespace Wt;

#if defined(_MSC_VER)
#define strtok_r strtok_s
#endif

int to_json(const std::string &file_name, model_t &model);

//./test_chart.wt --http-address=0.0.0.0 --http-port=8080  --docroot=. -d ../../../examples/test_chart/INX_2019_minor.txt

std::string file_data_txt;
const WColor color_grey = WColor();
const WColor color_green = WColor(0, 255, 0, 128);

///////////////////////////////////////////////////////////////////////////////////////
//Application_plotly
///////////////////////////////////////////////////////////////////////////////////////

class Application_plotly : public WApplication
{
private:
  WPlotly *m_plotly;
  WPushButton *btn_update;
  WPushButton *btn_wave_0;
  WPushButton *btn_wave_1;
  WPushButton *btn_wave_2;
  WPushButton *btn_wave_3;
  WPushButton *btn_wave_4;
  std::string m_edit_wave;
  model_t m_model;
  Http::Client *client;

public:
  Application_plotly(const WEnvironment& env) :
    WApplication(env),
    m_edit_wave("0")
  {
    setTitle("Chart");
    useStyleSheet("boxes.css");
    if (m_model.read(file_data_txt) < 0)
    {

    }
    //enable server push
    WApplication::instance()->enableUpdates(true);

    ///////////////////////////////////////////////////////////////////////////////////////
    //timer
    ///////////////////////////////////////////////////////////////////////////////////////

    WTimer *timer = root()->addChild(Wt::cpp14::make_unique<Wt::WTimer>());
    timer->setInterval(std::chrono::seconds(60 * 60));
    timer->timeout().connect(this, &Application_plotly::client_get);

    ///////////////////////////////////////////////////////////////////////////////////////
    //client
    ///////////////////////////////////////////////////////////////////////////////////////

    client = addChild(Wt::cpp14::make_unique<Http::Client>());
    client->done().connect(this, &Application_plotly::on_done);

    ///////////////////////////////////////////////////////////////////////////////////////
    //layout
    ///////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<WContainerWidget> container = Wt::cpp14::make_unique<Wt::WContainerWidget>();
    container->setStyleClass("yellow-box");
    WVBoxLayout *hbox = container->setLayout(cpp14::make_unique<WVBoxLayout>());

    std::string name = m_model.m_ticker;
    WText *text = hbox->addWidget(cpp14::make_unique<WText>(Wt::asString(name)));
    text->setStyleClass("green-box");

    ///////////////////////////////////////////////////////////////////////////////////////
    //buttons
    ///////////////////////////////////////////////////////////////////////////////////////

    btn_update = hbox->addWidget(cpp14::make_unique<WPushButton>("update"));
    btn_update->setWidth(70);
    btn_update->clicked().connect(btn_update, &WPushButton::disable);
    btn_update->clicked().connect(this, &Application_plotly::client_get);

    btn_wave_0 = hbox->addWidget(cpp14::make_unique<WPushButton>("0"));
    btn_wave_0->setWidth(70);
    btn_wave_0->clicked().connect(this, &Application_plotly::edit_wave_0);

    btn_wave_1 = hbox->addWidget(cpp14::make_unique<WPushButton>("1"));
    btn_wave_1->setWidth(70);
    btn_wave_1->clicked().connect(this, &Application_plotly::edit_wave_1);

    btn_wave_2 = hbox->addWidget(cpp14::make_unique<WPushButton>("2"));
    btn_wave_2->setWidth(70);
    btn_wave_2->clicked().connect(this, &Application_plotly::edit_wave_2);

    btn_wave_3 = hbox->addWidget(cpp14::make_unique<WPushButton>("3"));
    btn_wave_3->setWidth(70);
    btn_wave_3->clicked().connect(this, &Application_plotly::edit_wave_3);

    btn_wave_4 = hbox->addWidget(cpp14::make_unique<WPushButton>("4"));
    btn_wave_4->setWidth(70);
    btn_wave_4->clicked().connect(this, &Application_plotly::edit_wave_4);

    btn_wave_0->decorationStyle().setBackgroundColor(color_green);
    btn_wave_1->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_2->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_3->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_4->decorationStyle().setBackgroundColor(color_grey);

    ///////////////////////////////////////////////////////////////////////////////////////
    //plot
    ///////////////////////////////////////////////////////////////////////////////////////

    m_plotly = hbox->addWidget(cpp14::make_unique<WPlotly>(set_data()));
    text->setStyleClass("blue-box");
    m_plotly->clicked().connect([=](WPlotly::Coordinate c)
    {
      std::cerr << "Clicked at coordinate (" << c.x() << "," << c.y() << ")";
      this->update_model(c.x() / 1000);
    });

    root()->addWidget(std::move(container));
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //update_model()
  ///////////////////////////////////////////////////////////////////////////////////////

  void update_model(time_t time)
  {
    m_model.set_index(time);
    m_model.set_wave(m_edit_wave);
    m_model.ratios();
    Wt::WStringStream strm;
    strm
      << set_data();
    strm
      << "var self = " << m_plotly->jsRef() << ";\n"
      << "Plotly.react(self, data, layout);";

    time_price_t tp = m_model.m_tp.at(m_model.m_idx_curr);
    WDateTime date;
    date.setTime_t(std::time_t(tp.time));
    WString str_time = date.toString("yyyy-MM-dd-hh:mm:ss");
  
    m_plotly->doJavaScript(strm.str());
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //set_data()
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  std::string set_data()
  {
    Wt::WStringStream strm;
    strm
      << "var x_time = [];"
      << "var y_price = [];"
      << "var x_wave = [];"
      << "var y_wave = [];"
      << "var y_wave_label = [];"
      << "var x_expected = [];"
      << "var y_expected = [];"
      << "";
    size_t nbr_rows = m_model.m_tp.size();
    for (size_t idx = 0; idx < nbr_rows; idx++)
    {
      time_price_t tp = m_model.m_tp.at(idx);
      WDateTime date;
      date.setTime_t(std::time_t(tp.time));
      WString str_time = date.toString("yyyy-MM-dd-hh:mm:ss");
      strm
        << "x_time.push('" << str_time.toUTF8() << "');"
        << "y_price.push(" << std::to_string(tp.value) << ");";
      if (tp.wave != "-")
      {
        strm
          << "x_wave.push('" << str_time.toUTF8() << "');"
          << "y_wave.push(" << std::to_string(tp.value) << ");"
          << "y_wave_label.push(" << tp.wave << ");";
      }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //expected
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    for (size_t idx = 0; idx < m_model.m_expected.size(); idx++)
    {
      expected_t expected = m_model.m_expected.at(idx);
      //use data to get time for expected 
      size_t index = expected.index;
      if (index >= m_model.m_tp.size())
      {
        index = m_model.m_tp.size() - 1;
      }
      time_price_t tp = m_model.m_tp.at(index);
      WDateTime date;
      date.setTime_t(std::time_t(tp.time));
      WString str_time = date.toString("yyyy-MM-dd-hh:mm:ss");
      strm
        << "x_expected.push('" << str_time.toUTF8() << "');"
        << "y_expected.push(" << std::to_string(expected.price) << ");";
    }

#ifdef _DEBUG_
    strm
      << "var trace_price = {"
      << "x: x_time,"
      << "y: y_price,"
      << "mode: 'lines+markers',"
      << "type: 'scatter',"
      << "marker: {size: 7, color:'rgb(0,0,255)'},"
      << "line: {width: 1, color:'rgb(0,0,128)'}"
      << "};";
#else
    strm
      << "var trace_price = {"
      << "x: x_time,"
      << "y: y_price,"
      << "mode: 'lines',"
      << "type: 'scatter'"
      << "};";
#endif
    strm
      << "var trace_wave = {"
      << "x: x_wave,"
      << "y: y_wave,"
      << "text: y_wave_label,"
      << "mode: 'markers+text',"
      << "type: 'scatter',"
      << "marker: {size: 15, color:'rgb(0,255,0)'}"
      << "};";
    strm
      << "var trace_expected = {"
      << "x: x_expected,"
      << "y: y_expected,"
      << "mode: 'markers',"
      << "type: 'scatter',"
      << "marker: {size: 8, color:'rgb(255,0,255)'}"
      << "};";
    strm
      << "console.log(x_expected); console.log(y_expected);";

    ///////////////////////////////////////////////////////////////////////////////////////
    // "data", "layout" objects
    // Note: data object MUST named 'data', layout object MUST named 'layout'
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    strm
      << "var data = [trace_price, trace_wave, trace_expected];";
#ifdef _DEBUG_
    strm
      <<
      "var layout = {"
      "plot_bgcolor : 'rgb(192, 192, 192)',"
      "paper_bgcolor: 'rgb(192, 192, 192)',"
      "autosize: false, width: 2230, height: 800,"
      "yaxis: {"
      "tickformat: '.0'"
      "}"
      "};";
#else
    strm
      <<
      "var layout = {"
      "plot_bgcolor : 'rgb(192, 192, 192)',"
      "paper_bgcolor: 'rgb(192, 192, 192)',"
      "autosize: false, width: 1230, height: 500,"
      "yaxis: {"
      "tickformat: '.0'"
      "}"
      "};";
#endif
    return strm.str();
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //edit_wave_()
  ///////////////////////////////////////////////////////////////////////////////////////

  void edit_wave_0()
  {
    btn_wave_0->decorationStyle().setBackgroundColor(color_green);
    btn_wave_1->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_2->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_3->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_4->decorationStyle().setBackgroundColor(color_grey);
    m_edit_wave = "0";
  }

  void edit_wave_1()
  {
    btn_wave_0->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_1->decorationStyle().setBackgroundColor(color_green);
    btn_wave_2->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_3->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_4->decorationStyle().setBackgroundColor(color_grey);
    m_edit_wave = "1";
  }

  void edit_wave_2()
  {
    btn_wave_0->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_1->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_2->decorationStyle().setBackgroundColor(color_green);
    btn_wave_3->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_4->decorationStyle().setBackgroundColor(color_grey);
    m_edit_wave = "2";
  }

  void edit_wave_3()
  {
    btn_wave_0->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_1->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_2->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_3->decorationStyle().setBackgroundColor(color_green);
    btn_wave_4->decorationStyle().setBackgroundColor(color_grey);
    m_edit_wave = "3";
  }

  void edit_wave_4()
  {
    btn_wave_0->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_1->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_2->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_3->decorationStyle().setBackgroundColor(color_grey);
    btn_wave_4->decorationStyle().setBackgroundColor(color_green);
    m_edit_wave = "4";
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //on_done
  ///////////////////////////////////////////////////////////////////////////////////////

  void on_done(Wt::AsioWrapper::error_code err, const Http::Message& m)
  {
    enum csv_header
    {
      TIMESTAMP, OPEN, HIGH, LOW, CLOSE, VOLUME
    };
    Http::Message message = m;
    std::cerr << message.body().c_str();
    std::ofstream ofs("message.txt");
    ofs << message.body().c_str();
    ofs.close();

    //copy string
    char *cstr = new char[message.body().size() + 1];
    strcpy(cstr, message.body().c_str());
    std::vector<time_price_t> tp;
    char *bookmark_line;
    char *line = strtok_r(cstr, "\n", &bookmark_line);
    line = strtok_r(NULL, "\n", &bookmark_line); // skip header.
    double *closing_quotes = NULL;
    size_t cq_size = 0;
    size_t ts_size = 0;
    while (line)
    {
      char *bookmark_cell;
      char *cell = strtok_r(line, ",", &bookmark_cell);
      size_t column = TIMESTAMP;
      WDateTime date;
      while (cell)
      {
        if (column == TIMESTAMP)
        {
          ts_size++;
          WString str(cell);
          date = Wt::WDateTime::fromString(cell, "yyyy-MM-dd h:mm:ss");
        }
        else if (column == CLOSE)
        {
          cq_size++;
          closing_quotes = (double*)realloc(closing_quotes, cq_size * sizeof(double));
          closing_quotes[cq_size - 1] = atof(cell);
          time_t ts = date.toTime_t();
          time_price_t tv(ts, (float)closing_quotes[cq_size - 1], "-");
          tp.push_back(tv);
        }
        column++;
        cell = strtok_r(NULL, ",", &bookmark_cell);
      }
      line = strtok_r(NULL, "\n", &bookmark_line);
    }
    free(closing_quotes);
    std::reverse(tp.begin(), tp.end());
    delete[] cstr;

    ///////////////////////////////////////////////////////////////////////////////////////
    //update data
    ///////////////////////////////////////////////////////////////////////////////////////

    size_t size = tp.size();
    if (m_model.m_tp.size())
    {
      for (size_t idx = 0; idx < size; idx++)
      {
        time_price_t tv = tp.at(idx);
        time_t time_last = m_model.m_tp.back().time;
        if (tv.time > time_last)
        {
          m_model.m_tp.push_back(tv);
        }
      }
    }
    else
    {
      for (size_t idx = 0; idx < size; idx++)
      {
        m_model.m_tp.push_back(tp.at(idx));
      }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    //update chart
    ///////////////////////////////////////////////////////////////////////////////////////

    m_model.ratios();
    Wt::WStringStream strm;
    strm
      << set_data();
    strm
      << "var self = " << m_plotly->jsRef() << ";\n"
      << "Plotly.react(self, data, layout);";
    m_plotly->doJavaScript(strm.str());
    btn_update->enable();
    WApplication::instance()->triggerUpdate();
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //client_get
  ///////////////////////////////////////////////////////////////////////////////////////

  void client_get()
  {
    char url[255];
    char *ticker = "^INX";
    char *api_key = "IBT6KR8WTX30IUHS";
    char *outputsize = "full";
    int interval = 60;
    snprintf(url, sizeof url,
      "https://www.alphavantage.co/query"
      "?function=TIME_SERIES_INTRADAY"
      "&symbol=%s"
      "&interval=%dmin"
      "&datatype=csv"
      "&apikey=%s",
      ticker, interval, api_key);
    if (client->get(url))
    {
    }
  }

};

///////////////////////////////////////////////////////////////////////////////////////
//create_application
///////////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<WApplication> create_application(const WEnvironment& env)
{
  return cpp14::make_unique<Application_plotly>(env);
}

///////////////////////////////////////////////////////////////////////////////////////
//main
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
  for (int i = 1; i < argc; i++)
  {
    if (argv[i][0] == '-')
    {
      switch (argv[i][1])
      {
      case 'd':
        file_data_txt = argv[i + 1];
        i++;
        break;
      }
    }
  }
  std::cout << file_data_txt << std::endl;
  return WRun(argc, argv, &create_application);
}

///////////////////////////////////////////////////////////////////////////////////////
//to_json
//convert .txt to .json
///////////////////////////////////////////////////////////////////////////////////////

int to_json(const std::string &file_name, model_t &model)
{
  if (model.read(file_data_txt) < 0)
  {
    return - 1;
  }
  const std::string MARKET_JSON("market.json");
  std::string input;
  std::stringstream buffer;
  std::ifstream ifs(MARKET_JSON);
  buffer << ifs.rdbuf();
  input = buffer.str();
  ifs.close();

  Json::Object result;
  Json::parse(input, result);
  std::set<std::string> names = result.names(); //ticker object names (object key)

  Json::Object root;
  size_t size = model.m_tp.size();

  //object for 1 model
  Json::Object obj;
  obj.insert(std::pair<std::string, Json::Value>("function", Json::Value("TIME_SERIES_INTRADAY")));
  obj.insert(std::pair<std::string, Json::Value>("interval", Json::Value(model.m_interval)));

  Json::Array time;
  Json::Array price;
  Json::Array waves_arr;
  for (size_t idx = 0; idx < size; idx++)
  {
    time_price_t tp = model.m_tp.at(idx);
    time.push_back((long long)tp.time);
    price.push_back(roundf(tp.value));
    Json::Object wave;
    Json::Value v = (long long)idx;
    if (tp.wave != "-")
    {
      wave[tp.wave] = v;
      waves_arr.push_back(wave);
    }
  }
  Json::Object waves;
  if (model.m_wave_type.find("minute"))
  {
    waves.insert(std::pair<std::string, Json::Value>("minute", waves_arr));
  }
  else if (model.m_wave_type.find("minor"))
  {
    waves.insert(std::pair<std::string, Json::Value>("minor", waves_arr));
  }
  else if (model.m_wave_type.find("intermediate"))
  {
    waves.insert(std::pair<std::string, Json::Value>("intermediate", waves_arr));
  }
  else
  {
    assert(0);
  }
  obj.insert(std::pair<std::string, Json::Value>("waves", waves));
  obj.insert(std::pair<std::string, Json::Value>("time", time));
  obj.insert(std::pair<std::string, Json::Value>("price", price));

  root.insert(std::pair<std::string, Json::Value>(model.m_ticker, obj));

  std::string generated = Json::serialize(root);
  std::ofstream out(MARKET_JSON);
  out << generated;
  out.close();
  return 0;
}
