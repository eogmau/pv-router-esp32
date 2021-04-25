//***********************************
//************* SPIFFS 
//***********************************
#ifndef SPIFFS_FUNCTIONS
#define SPIFFS_FUNCTIONS


// File System
#ifdef ESP32
#include <FS.h>
#include "SPIFFS.h"
#include "config/enums.h"
#endif

#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include <ArduinoJson.h> // ArduinoJson : https://github.com/bblanchon/ArduinoJson

/*
struct Config {
  char hostname[15];
  int port;
  char apiKey[64];
  bool UseDomoticz;
  bool UseJeedom;
  String IDX;
  char otapassword[64];
  int delta;
  int deltaneg;
  int cosphi;
  int readtime;
  int cycle;
  bool sending;
  bool autonome;
  char dimmer[15];
  bool dimmerlocal;
  float facteur;
  int num_fuse;
  bool mqtt;
  char mqttserver[15];
  String IDXdimmer;
  int tmax;
  int resistance;
};
*/

const char *filename_conf = "/config.json";
extern Config config; 

//***********************************
//************* Gestion de la configuration - Lecture du fichier de configuration
//***********************************
// Loads the configuration from a file
void loadConfiguration(const char *filename, Config &config) {
  // Open file for reading
  File configFile = SPIFFS.open(filename_conf, "r");

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration in function loadConfiguration"));


  }

  
  // Copy values from the JsonDocument to the Config
  config.port = doc["port"] | 8080;
  strlcpy(config.hostname,                  // <- destination
          doc["hostname"] | "192.168.1.20", // <- source
          sizeof(config.hostname));         // <- destination's capacity
  
  strlcpy(config.apiKey,                  // <- destination
          doc["apiKey"] | "Myapikeystring", // <- source
          sizeof(config.apiKey));         // <- destination's capacity
      
  config.UseDomoticz = doc["UseDomoticz"] | false; 
  config.UseJeedom = doc["UseJeedom"] | false; 
  config.IDX = doc["IDX"] | 100; 
  config.IDXdimmer = doc["IDXdimmer"] | 110; 
  
  strlcpy(config.otapassword,                  // <- destination
          doc["otapassword"] | "Pvrouteur2", // <- source
          sizeof(config.otapassword));         // <- destination's capacity
  
  config.facteur = doc["facteur"] | 0.86; 
  config.delta = doc["delta"] | 50; 
  config.num_fuse = doc["fuse"] | 70;
  config.deltaneg = doc["deltaneg"] | -100; 
  config.cosphi = doc["cosphi"] | 23; 
  config.readtime = doc["readtime"] | 555;
  config.cycle = doc["cycle"] | 25;
  config.tmax = doc["tmax"] | 65;
  config.resistance = doc["resistance"] | 1000;
  config.sending = doc["sending"] | true;
  config.autonome = doc["autonome"] | true;
  config.mqtt = doc["mqtt"] | true;
  config.dimmerlocal = doc["dimmerlocal"] | false;
  strlcpy(config.dimmer,                  // <- destination
          doc["dimmer"] | "192.168.1.20", // <- source
          sizeof(config.dimmer));         // <- destination's capacity

   strlcpy(config.mqttserver,                  // <- destination
          doc["mqttserver"] | "192.168.1.20", // <- source
          sizeof(config.mqttserver));         // <- destination's capacity
  configFile.close();
      
}

//***********************************
//************* Gestion de la configuration - sauvegarde du fichier de configuration
//***********************************

void saveConfiguration(const char *filename, const Config &config) {
  
  // Open file for writing
   File configFile = SPIFFS.open(filename_conf, "w");
  if (!configFile) {
    Serial.println(F("Failed to open config file for writing in function Save configuration"));
    
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Set the values in the document
  doc["hostname"] = config.hostname;
  doc["port"] = config.port;
  doc["apiKey"] = config.apiKey;
  doc["UseDomoticz"] = config.UseDomoticz;
  doc["UseJeedom"] = config.UseJeedom;
  doc["IDX"] = config.IDX;
  doc["IDXdimmer"] = config.IDXdimmer;
  doc["otapassword"] = config.otapassword;
  doc["delta"] = config.delta;
  doc["deltaneg"] = config.deltaneg;
  doc["cosphi"] = config.cosphi;
  doc["readtime"] = config.readtime;
  doc["cycle"] = config.cycle;
  doc["sending"] = config.sending;
  doc["autonome"] = config.autonome;
  doc["dimmer"] = config.dimmer;
  doc["dimmerlocal"] = config.dimmerlocal;
  doc["facteur"] = config.facteur;
  doc["fuse"] = config.num_fuse;
  doc["mqtt"] = config.mqtt;
  doc["mqttserver"] = config.mqttserver;  
  doc["tmax"] = config.tmax;
  doc["resistance"] = config.resistance;

  // Serialize JSON to file
  if (serializeJson(doc, configFile) == 0) {
    Serial.println(F("Failed to write to file in function Save configuration "));
    
  }

  // Close the file
  configFile.close();
}


#endif