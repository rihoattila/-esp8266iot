#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <EEPROM.h>

#include "global.h"
#include "NTP.h"

// Include the HTML, STYLE and Script "Pages"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"

extern "C" {
#include "user_interface.h"
}

Ticker ticker;

os_timer_t myTimer;

const int PIN_SWITCH_CONFIG = 13;

String chipID;


void setupWebConfig(String EUID){
  bool CFG_saved = false;
  int WIFI_connected = false;
  
 
  //**** Network Config load 
  EEPROM.begin(512); // define an EEPROM space of 512Bytes to store data

  //check config button
  pinMode(PIN_SWITCH_CONFIG, INPUT_PULLUP);
  if(!digitalRead(PIN_SWITCH_CONFIG))
  {
	  ClearConfig();
  }
	  
  CFG_saved = ReadConfig();

  //  Connect to WiFi acess point or start as Acess point
  if (CFG_saved)  //if no configuration yet saved, load defaults
  {    
      // Connect the ESP8266 to local WIFI network in Station mode
      //printConfig();
	  WiFi.persistent(false);
	  WiFi.setAutoConnect(true);
      WiFi.mode(WIFI_STA);  
      WiFi.begin(config.ssid.c_str(), config.password.c_str());
      WIFI_connected = WiFi.waitForConnectResult();
      // if(WIFI_connected!= WL_CONNECTED )
          // Serial.println("Connection Failed! activating to AP mode...");
      
  }

  if ( (WIFI_connected!= WL_CONNECTED) or !CFG_saved){
    // DEFAULT CONFIG
    config.ssid = "IOT-" + EUID;       // SSID of access point
    config.password = "123456" ;   // password of access point
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    config.ntpServerName = "0.ch.pool.ntp.org"; // to be adjusted to PT ntp.ist.utl.pt
    config.Update_Time_Via_NTP_Every =  5;
    config.timeZone = 1;
    config.isDayLightSaving = true;
    config.DeviceName = "Not Named";
    WiFi.mode(WIFI_AP);  
    WiFi.softAP(config.ssid.c_str());
     
   }
   

    // Start HTTP Server for configuration
    server.on ( "/", []() {
      server.send_P ( 200, "text/html", PAGE_AdminMainPage);  // const char top of page
    }  );
  
    server.on ( "/favicon.ico",   []() {
      server.send( 200, "text/html", "" );
    }  );
  
    // Network config
    server.on ( "/config.html", send_network_configuration_html );
    // Info Page
    server.on ( "/info.html", []() {
      server.send_P ( 200, "text/html", PAGE_Information );
    }  );
    server.on ( "/ntp.html", send_NTP_configuration_html  );
  
    //server.on ( "/appl.html", send_application_configuration_html  );
    server.on ( "/general.html", send_general_html  );
    //  server.on ( "/example.html", []() { server.send_P ( 200, "text/html", PAGE_EXAMPLE );  } );
    server.on ( "/style.css", []() {
      server.send_P ( 200, "text/plain", PAGE_Style_css );
    } );
    server.on ( "/microajax.js", []() {
      server.send_P ( 200, "text/plain", PAGE_microajax_js );
    } );
    server.on ( "/admin/values", send_network_configuration_values_html );
    server.on ( "/admin/connectionstate", send_connection_state_values_html );
    server.on ( "/admin/infovalues", send_information_values_html );
    server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
    //server.on ( "/admin/applvalues", send_application_configuration_values_html );
    server.on ( "/admin/generalvalues", send_general_configuration_values_html);
    server.on ( "/admin/devicename",     send_devicename_value_html);
  
  
    server.onNotFound ( []() {
      server.send ( 400, "text/html", "Page not Found" );
    }  );
    server.begin();
	
    // start internal time update ISR
    tkSecond.attach(1, ISRsecondTick);
    

}

void handleWebConfig(){
	server.handleClient();
	
	//  feed de DOG :) 
	customWatchdog = millis();


}