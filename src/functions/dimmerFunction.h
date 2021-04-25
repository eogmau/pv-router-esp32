#ifndef DIMMER_FUNCTIONS
#define DIMMER_FUNCTIONS

#include <WiFi.h>
#include "../config/enums.h"
#include "../config/config.h"
#include "../functions/spiffsFunctions.h"
#include "../functions/Mqtt_http_Functions.h"

extern DisplayValues gDisplayValues;

HTTPClient http;

/*
*   fonction d'envoie de commande au dimmer
*/

void dimmer_change(char dimmerurl[15], String dimmerIDX, int dimmervalue) {
    /// envoyer la commande avec la valeur gDisplayValues.dimmer vers le dimmer config.dimmer
    #if WIFI_ACTIVE == true
    String baseurl; 
      baseurl = "/?POWER=" + String(dimmervalue) ; 
      http.begin(dimmerurl,80,baseurl);   
      http.GET();
      http.end(); 

    #if MQTT_CLIENT == true 
    /// A vérifier que c'est necessaire ( envoie double ? )
      Mqtt_send(dimmerIDX, String(dimmervalue));  
    #endif
    
    delay (2000); // delay de transmission réseau dimmer et application de la charge
    #endif
}


//***********************************
//************* Fonction aservissement autonome
//***********************************

void dimmer(){
gDisplayValues.change = 0; 

  // 0 -> linky ; 1-> injection  ; 2-> stabilisé

  /// Linky 
  // si grosse puissance instantanée sur le réseau, coupure du dimmer. ( ici 350w environ ) 
  if ( gDisplayValues.watt >= 350 && gDisplayValues.dimmer != 0 )  {
    gDisplayValues.dimmer = 0 ;  
    gDisplayValues.change = 1 ;
    } 
  
  /// si gros mode linky  on reduit la puissance par extrapolation ( valeur de puissance supérieur à config.delta + 30 )
  else if ( gDisplayValues.dimmer != 0 && gDisplayValues.watt >= (config.delta+30) ) {
    gDisplayValues.dimmer += -2*((gDisplayValues.watt-config.delta)/(50*config.resistance/1000)) ; 
    gDisplayValues.change = 1; 
    } 
  
    /// si petit mode linky on reduit la puissance 
  else if (gDisplayValues.watt >= (config.delta) && gDisplayValues.dimmer != 0 ) {
    gDisplayValues.dimmer += -1 ; 
    gDisplayValues.change = 1; 
    }  
  
    // injection 
    /// si grosse injection on augmente la puissance par extrapolation
  else if ( gDisplayValues.watt <= (config.deltaneg-30) ) {   
    gDisplayValues.dimmer += 2*abs(gDisplayValues.watt/(50*config.resistance/1000)) ; 
    gDisplayValues.change = 1 ; 
    } 
  
    /// si injection legère on augmente la puissance doucement
  else if (gDisplayValues.watt <= (config.deltaneg)  ) { 
    gDisplayValues.dimmer += 1 ; 
    gDisplayValues.change = 1 ; 
    }
  
    /// test puissance de sécurité 
  if ( gDisplayValues.dimmer >= config.num_fuse ) {
    gDisplayValues.dimmer = config.num_fuse; 
    gDisplayValues.change = 1 ; 
    }

    /// valeur négative impossible
  if ( gDisplayValues.dimmer <= 0 && gDisplayValues.dimmer != 0 ) {
    gDisplayValues.dimmer = 0; 
    gDisplayValues.change = 1 ; 
    }
    
    gDisplayValues.security ++ ;

   //// envoie d'un Zero au dimmer de temps en temps pour des raisons de sécurité
    if ( gDisplayValues.security >= 5 ) { 
      if ( gDisplayValues.dimmer <= 0 ) {
        gDisplayValues.dimmer = 0; 
        gDisplayValues.change = 1 ; 
        gDisplayValues.security = 0;  
      }
    } 

  if  (gDisplayValues.change == 1 )  {
    dimmer_change( config.dimmer, config.IDXdimmer, gDisplayValues.dimmer ) ; 
  }
}




#endif