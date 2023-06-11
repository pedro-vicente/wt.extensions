#include <Wt/WLogger.h>
#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include "web/WebUtils.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <utility>
#include <iostream>
#include <cmath>

#include "WCesium.hh"

namespace Wt
{

  ///////////////////////////////////////////////////////////////////////////////////////
  //WCesium::WCesium
  ///////////////////////////////////////////////////////////////////////////////////////

  WCesium::WCesium(const std::string &js)
  {
    setImplementation(std::unique_ptr<WWidget>(new WContainerWidget()));
    this->addCssRule("html", "width: 100%; height: 100%; margin: 0; padding: 0; overflow: hidden;");
    this->addCssRule("body", "width: 100%; height: 100%; margin: 0; padding: 0; overflow: hidden;");
    this->addCssRule("#" + id(), "width: 100%; height: 100%; margin: 0; padding: 0; overflow: hidden;");
    WApplication *app = WApplication::instance();
    app->useStyleSheet("https://cesium.com/downloads/cesiumjs/releases/1.84/Build/Cesium/Widgets/widgets.css");
    const std::string library = "https://cesium.com/downloads/cesiumjs/releases/1.83/Build/Cesium/Cesium.js";
    app->require(library, "cesium");
    m_javascrit = js;
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  //WCesium::render
  ///////////////////////////////////////////////////////////////////////////////////////

  void WCesium::render(WFlags<RenderFlag> flags)
  {
    if (flags.test(RenderFlag::Full))
    {
      Wt::WApplication * app = Wt::WApplication::instance();
      Wt::WString initFunction = app->javaScriptClass() + ".init_celsium_" + id();
      Wt::WStringStream strm;

      strm
        << "{ " << initFunction.toUTF8() << " = function() {\n"
        << "  var self = " << jsRef() << ";\n"
        << "  if (!self) {\n"
        << "    setTimeout(" << initFunction.toUTF8() << ", 0);\n"
        << "  }\n";

      //pass 'DIV'
      strm
        << "Cesium.Ion.defaultAccessToken = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiI5YTU5NTg2ZS1hNTU1LTQ0MDMtYWY5Ni0xNjI3NzYwOTk5ZTQiLCJpZCI6NTUwNSwic2NvcGVzIjpbImFzciIsImdjIl0sImlhdCI6MTU0MzQ0ODUzN30.oNtrUYktcIAGxFQPWFTm9wBQ4r6wCFFL6L34kvwbrXg';"
        << "  var viewer = new Cesium.Viewer(" << jsRef() << ");\n";

      //rendering code (assume 'viewer' variable)
      strm << m_javascrit;

      strm
        << "  setTimeout(function(){ delete " << initFunction.toUTF8() << ";}, 0)};\n"
        << "}\n"
        << initFunction.toUTF8() << "();\n";

      app->doJavaScript(strm.str(), true);
    }
    Wt::WCompositeWidget::render(flags);
  }
}
