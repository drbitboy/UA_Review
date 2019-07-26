/** \file andorCtrl.hpp
  * \brief The MagAO-X Andor EMCCD camera controller.
  *
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup andorCtrl_files
  */

#ifndef andorCtrl_hpp
#define andorCtrl_hpp






#include "../../libMagAOX/libMagAOX.hpp" //Note this is included on command line to trigger pch
#include "../../magaox_git_version.h"


#include "atmcdLXd.h"

namespace MagAOX
{
namespace app
{

#define CAMCTRL_E_NOCONFIGS (-10)
   
///\todo craete cameraConfig in libMagAOX
struct cameraConfig 
{
   std::string m_configFile;
   std::string m_serialCommand;
   unsigned m_binning {0};
   unsigned m_sizeX {0};
   unsigned m_sizeY {0};
   float m_maxFPS {0};
};

typedef std::unordered_map<std::string, cameraConfig> cameraConfigMap;

inline
int loadCameraConfig( cameraConfigMap & ccmap,
                      mx::app::appConfigurator & config 
                    )
{
   std::vector<std::string> sections;

   config.unusedSections(sections);

   if( sections.size() == 0 )
   {
      return CAMCTRL_E_NOCONFIGS;
   }
   
   for(size_t i=0; i< sections.size(); ++i)
   {
      bool fileset = config.isSetUnused(mx::app::iniFile::makeKey(sections[i], "configFile" ));
      /*bool binset = config.isSetUnused(mx::app::iniFile::makeKey(sections[i], "binning" ));
      bool sizeXset = config.isSetUnused(mx::app::iniFile::makeKey(sections[i], "sizeX" ));
      bool sizeYset = config.isSetUnused(mx::app::iniFile::makeKey(sections[i], "sizeY" ));
      bool maxfpsset = config.isSetUnused(mx::app::iniFile::makeKey(sections[i], "maxFPS" ));
      */
      
      //The configuration file tells us most things for EDT, so it's our current requirement. 
      if( !fileset ) continue;
      
      std::string configFile;
      config.configUnused(configFile, mx::app::iniFile::makeKey(sections[i], "configFile" ));
      
      std::string serialCommand;
      config.configUnused(serialCommand, mx::app::iniFile::makeKey(sections[i], "serialCommand" ));
      
      unsigned binning = 0;
      config.configUnused(binning, mx::app::iniFile::makeKey(sections[i], "binning" ));
      
      unsigned sizeX = 0;
      config.configUnused(sizeX, mx::app::iniFile::makeKey(sections[i], "sizeX" ));
      
      unsigned sizeY = 0;
      config.configUnused(sizeY, mx::app::iniFile::makeKey(sections[i], "sizeY" ));
      
      float maxFPS = 0;
      config.configUnused(maxFPS, mx::app::iniFile::makeKey(sections[i], "maxFPS" ));
      
      ccmap[sections[i]] = cameraConfig({configFile, serialCommand, binning, sizeX, sizeY, maxFPS});
   }
   
   return 0;
}

/** \defgroup andorCtrl Andor EMCCD Camera
  * \brief Control of the Andor EMCCD Camera.
  *
  *  <a href="../apps_html/page_module_andorCtrl.html">Application Documentation</a>
  *
  * \ingroup apps
  *
  */

/** \defgroup andorCtrl_files Andor EMCCD Camera Files
  * \ingroup andorCtrl
  */

/** MagAO-X application to control the Andor EMCCD
  *
  * \ingroup andorCtrl
  * 
  */
class andorCtrl : public MagAOXApp<>, public dev::edtCamera<andorCtrl>, public dev::frameGrabber<andorCtrl>
{

   friend class dev::frameGrabber<andorCtrl>;
   
   
protected:

   /** \name configurable parameters 
     *@{
     */ 
   
   //Camera:
   unsigned long m_powerOnWait {10}; ///< Time in sec to wait for camera boot after power on.

   cameraConfigMap m_cameraModes; ///< Map holding the possible camera mode configurations
   
   float m_startupTemp {20.0}; ///< The temperature to set after a power-on.
      
   unsigned m_maxEMGain {600};
   
   ///@}
   
   u_char * m_image_p {nullptr};
   
   int m_powerOnCounter {0}; ///< Counts numer of loops after power on, implements delay for camera bootup.

   std::string m_modeName;
   std::string m_nextMode;
   
      
   unsigned m_emGain {1};
   
public:

   ///Default c'tor
   andorCtrl();

   ///Destructor
   ~andorCtrl() noexcept;

   /// Setup the configuration system (called by MagAOXApp::setup())
   virtual void setupConfig();

   /// load the configuration system results (called by MagAOXApp::setup())
   virtual void loadConfig();

   /// Startup functions
   /** Sets up the INDI vars.
     *
     */
   virtual int appStartup();

   /// Implementation of the FSM for the Siglent SDG
   virtual int appLogic();

   /// Implementation of the on-power-off FSM logic
   virtual int onPowerOff();

   /// Implementation of the while-powered-off FSM
   virtual int whilePowerOff();

   /// Do any needed shutdown tasks.  Currently nothing in this app.
   virtual int appShutdown();



   
   int cameraSelect(int camNo);
   
   int getTemp();
   
   int setTemp(float temp);
   
   int getFPS();
   
   int setFPS(float fps);
   
   int getEMGain();
   
   int setEMGain( unsigned emg );
   
   int configureAcquisition();
   int startAcquisition();
   int acquireAndCheckValid();
   int loadImageIntoStream(void * dest);
   int reconfig();
   
   
   //INDI:
protected:
   //declare our properties
   pcf::IndiProperty m_indiP_ccdtemp;
   pcf::IndiProperty m_indiP_mode;
   pcf::IndiProperty m_indiP_fps;
   pcf::IndiProperty m_indiP_emGain;

public:
   INDI_NEWCALLBACK_DECL(andorCtrl, m_indiP_ccdtemp);
   INDI_NEWCALLBACK_DECL(andorCtrl, m_indiP_mode);
   INDI_NEWCALLBACK_DECL(andorCtrl, m_indiP_fps);
   INDI_NEWCALLBACK_DECL(andorCtrl, m_indiP_emGain);

};

inline
andorCtrl::andorCtrl() : MagAOXApp(MAGAOX_CURRENT_SHA1, MAGAOX_REPO_MODIFIED)
{
   //m_powerMgtEnabled = true;
   
   return;
}

inline
andorCtrl::~andorCtrl() noexcept
{
   return;
}

inline
void andorCtrl::setupConfig()
{
   dev::edtCamera<andorCtrl>::setupConfig(config);
   dev::frameGrabber<andorCtrl>::setupConfig(config);
 
   config.add("camera.powerOnWait", "", "camera.powerOnWait", argType::Required, "camera", "powerOnWait", false, "int", "Time after power-on to begin attempting connections [sec].  Default is 10 sec.");
   
   config.add("camera.startupTemp", "", "camera.startupTemp", argType::Required, "camera", "startupTemp", false, "float", "The temperature setpoint to set after a power-on [C].  Default is 20 C.");
   
   config.add("camera.maxEMGain", "", "camera.maxEMGain", argType::Required, "camera", "maxEMGain", false, "unsigned", "The maximum EM gain which can be set by  user. Default is 600.  Min is 1, max is 600.");
   
   
}



inline
void andorCtrl::loadConfig()
{
   dev::edtCamera<andorCtrl>::loadConfig(config);
   
   config(m_powerOnWait, "camera.powerOnWait");
   config(m_startupTemp, "camera.startupTemp");
   config(m_maxEMGain, "camera.maxEMGain");
   if(m_maxEMGain < 1)
   {
      m_maxEMGain = 1;
      log<text_log>("maxEMGain set to 1");
   }
   
   if(m_maxEMGain > 600)
   {
      m_maxEMGain = 600;
      log<text_log>("maxEMGain set to 600");
   }
   
   dev::frameGrabber<andorCtrl>::loadConfig(config);
   
   
   
   
}



inline
int andorCtrl::appStartup()
{
   // set up the  INDI properties
   REG_INDI_NEWPROP(m_indiP_ccdtemp, "ccdtemp", pcf::IndiProperty::Number);
   m_indiP_ccdtemp.add (pcf::IndiElement("current"));
   m_indiP_ccdtemp["current"].set(0);
   m_indiP_ccdtemp.add (pcf::IndiElement("target"));
   

   REG_INDI_NEWPROP(m_indiP_mode, "mode", pcf::IndiProperty::Text);
   m_indiP_mode.add (pcf::IndiElement("current"));
   m_indiP_mode.add (pcf::IndiElement("target"));

   REG_INDI_NEWPROP(m_indiP_fps, "fps", pcf::IndiProperty::Number);
   m_indiP_fps.add (pcf::IndiElement("current"));
   m_indiP_fps["current"].set(0);
   m_indiP_fps.add (pcf::IndiElement("target"));
   m_indiP_fps.add (pcf::IndiElement("measured"));

   REG_INDI_NEWPROP(m_indiP_emGain, "emgain", pcf::IndiProperty::Number);
   m_indiP_emGain.add (pcf::IndiElement("current"));
   m_indiP_emGain["current"].set(m_emGain);
   m_indiP_emGain.add (pcf::IndiElement("target"));
   
   if(pdvConfig(m_startupMode) < 0) 
   {
      log<software_error>({__FILE__, __LINE__});
      return -1;
   }
   
   if(dev::frameGrabber<andorCtrl>::appStartup() < 0)
   {
      return log<software_critical,-1>({__FILE__,__LINE__});
   }
   
   
   return 0;

}



inline
int andorCtrl::appLogic()
{
   //first run frameGrabber's appLogic to see if the f.g. thread has exited.
   if(dev::frameGrabber<andorCtrl>::appLogic() < 0)
   {
      return log<software_error, -1>({__FILE__, __LINE__});
   }
   
   if( state() == stateCodes::POWERON )
   {
      if(m_powerOnCounter*m_loopPause > ((double) m_powerOnWait)*1e9)
      {
         state(stateCodes::NOTCONNECTED);
         m_reconfig = true; //Trigger a f.g. thread reconfig.
         m_powerOnCounter = 0;
      }
      else
      {
         ++m_powerOnCounter;
         return 0;
      }
   }

   if( state() == stateCodes::NOTCONNECTED || state() == stateCodes::ERROR)
   {
      std::string response;

      //Might have gotten here because of a power off.
      if(m_powerState == 0) return 0;
      
      int ret = 0;//pdvSerialWriteRead( response, m_pdv, "fps", m_readTimeout);
      if( ret == 0)
      {
         state(stateCodes::CONNECTED);
      }
      else
      {
         sleep(1);
         return 0;
      }
   }

   if( state() == stateCodes::CONNECTED )
   {
      //Get a lock
      std::unique_lock<std::mutex> lock(m_indiMutex);
      
//       if( getFPS() == 0 )
//       {
//          if(m_fpsSet == 0) state(stateCodes::READY);
//          else state(stateCodes::OPERATING);
//          
//          if(setTemp(m_startupTemp) < 0)
//          {
//             return log<software_error,0>({__FILE__,__LINE__});
//          }
//       }
//       else
//       {
//          state(stateCodes::ERROR);
//          return log<software_error,0>({__FILE__,__LINE__});
//       }
   }

   if( state() == stateCodes::READY || state() == stateCodes::OPERATING )
   {
      //Get a lock if we can
      std::unique_lock<std::mutex> lock(m_indiMutex, std::try_to_lock);

      //but don't wait for it, just go back around.
      if(!lock.owns_lock()) return 0;
      
      if(getTemp() < 0)
      {
         if(m_powerState == 0) return 0;
         
         state(stateCodes::ERROR);
         return 0;
      }

      if(getFPS() < 0)
      {
         if(m_powerState == 0) return 0;
         
         state(stateCodes::ERROR);
         return 0;
      }
      
      if(getEMGain () < 0)
      {
         if(m_powerState == 0) return 0;
         
         state(stateCodes::ERROR);
         return 0;
      }
      
      if(frameGrabber<andorCtrl>::updateINDI() < 0)
      {
         log<software_error>({__FILE__, __LINE__});
         state(stateCodes::ERROR);
         return 0;
      }
   }

   //Fall through check?

   return 0;

}

inline
int andorCtrl::onPowerOff()
{
   m_powerOnCounter = 0;
   
   std::lock_guard<std::mutex> lock(m_indiMutex);
   
   updateIfChanged(m_indiP_ccdtemp, "current", -999);
   updateIfChanged(m_indiP_ccdtemp, "target", -999);
      
   updateIfChanged(m_indiP_mode, "current", std::string(""));
   updateIfChanged(m_indiP_mode, "target", std::string(""));

   updateIfChanged(m_indiP_fps, "current", 0);
   updateIfChanged(m_indiP_fps, "target", 0);
   updateIfChanged(m_indiP_fps, "measured", 0);
   
   updateIfChanged(m_indiP_emGain, "current", 0);
   updateIfChanged(m_indiP_emGain, "target", 0);
   
   return 0;
}

inline
int andorCtrl::whilePowerOff()
{
   return 0;
}

inline
int andorCtrl::appShutdown()
{
   dev::frameGrabber<andorCtrl>::appShutdown();
   
   return 0;
}

inline
int andorCtrl::cameraSelect(int camNo)
{
    at_32 lNumCameras;
    GetAvailableCameras(&lNumCameras);

    int iSelectedCamera = camNo;
 
    if (iSelectedCamera < lNumCameras && iSelectedCamera >= 0) 
    {
      at_32 lCameraHandle;
      GetCameraHandle(iSelectedCamera, &lCameraHandle);
      
      SetCurrentCamera(lCameraHandle);
      
      return 0;
    }
    else
      return -1;
  
}

inline
int andorCtrl::getTemp()
{
   std::string response;

   int temp, temp_low, temp_high;
   unsigned long error=GetTemperatureRange(&temp_low, &temp_high); ///\todo need error check
   unsigned long status=GetTemperature(&temp);
   
   std::cout << "Current Temperature: " << temp << " C" << std::endl;
   std::cout << "Temp Range: {" << temp_low << "," << temp_high << "}" << std::endl;
   std::cout << "Status             : ";
   switch(status)
   {
      case DRV_TEMPERATURE_OFF: std::cout << "Cooler OFF" << std::endl; break;
        case DRV_TEMPERATURE_STABILIZED: std::cout << "Stabilised" << std::endl; break;
        case DRV_TEMPERATURE_NOT_REACHED: std::cout << "Cooling" << std::endl; break;
        case DRV_TEMPERATURE_NOT_STABILIZED: std::cout << "Temp reached but not stablized" << std::endl; break;
        case DRV_TEMPERATURE_DRIFT: std::cout << "Temp had stabilized but has since drifted" << std::endl; break;
        default:std::cout << "Unknown" << std::endl;
   }     
      
      
   //log<ocam_temps>({temps.CCD, temps.CPU, temps.POWER, temps.BIAS, temps.WATER, temps.LEFT, temps.RIGHT, temps.COOLING_POWER});

   updateIfChanged(m_indiP_ccdtemp, "current", temp);
   return 0;


}

inline
int andorCtrl::setTemp(float temp)
{
   return 0;
}

inline
int andorCtrl::getFPS()
{
   return 0;

}

inline
int andorCtrl::setFPS(float fps)
{
   return 0;

}

inline
int andorCtrl::getEMGain()
{
   int state;
   int gain;
   int low, high;
   unsigned long out = GetEMAdvanced(&state);
   
   if(out==DRV_SUCCESS){
        std::cout << "The Current Advanced EM gain setting is: " << state << std::endl;
   }
   out = GetEMCCDGain(&gain);
   if(out==DRV_SUCCESS){
       std::cout << "Current EMCCD Gain: " << gain << std::endl;
   }
   
   out = GetEMGainRange(&low, &high);
   if(out==DRV_SUCCESS){
        std::cout << " The range of EMGain is: {" << low << "," << high << "}" << std::endl;
   }
}
   
inline
int andorCtrl::setEMGain( unsigned emg )
{
   
}

inline
int andorCtrl::configureAcquisition()
{
   //lock mutex
   std::unique_lock<std::mutex> lock(m_indiMutex);
   
   
   return 0;
}
   
inline
int andorCtrl::startAcquisition()
{
   return 0;
}

inline
int andorCtrl::acquireAndCheckValid()
{
   
   return 0;
}

inline
int andorCtrl::loadImageIntoStream(void * dest)
{
   return 0;
}
   
inline
int andorCtrl::reconfig()
{
   //lock mutex
   std::unique_lock<std::mutex> lock(m_indiMutex);
   
   if(pdvConfig(m_nextMode) < 0)
   {
      log<text_log>("error trying to re-configure with " + m_nextMode, logPrio::LOG_ERROR);
      sleep(1);
   }
   else
   {
      m_nextMode = "";
      
   }
   
   return 0;
}
   

      
         
   
     
   
   

         

         
         
     
           
    
     

INDI_NEWCALLBACK_DEFN(andorCtrl, m_indiP_ccdtemp)(const pcf::IndiProperty &ipRecv)
{
   if (ipRecv.getName() == m_indiP_ccdtemp.getName())
   {
      float current = 99, target = 99;

      try
      {
         current = ipRecv["current"].get<float>();
      }
      catch(...){}

      try
      {
         target = ipRecv["target"].get<float>();
      }
      catch(...){}

      
      //Check if target is empty
      if( target == 99 ) target = current;
      
      //Now check if it's valid?
      ///\todo implement more configurable max-set-able temperature
      if( target > 30 ) return 0;
      
      
      //Lock the mutex, waiting if necessary
      std::unique_lock<std::mutex> lock(m_indiMutex);
      
      updateIfChanged(m_indiP_ccdtemp, "target", target);
      
      return setTemp(target);
   }
   return -1;
}

INDI_NEWCALLBACK_DEFN(andorCtrl, m_indiP_mode)(const pcf::IndiProperty &ipRecv)
{
   if (ipRecv.getName() == m_indiP_mode.getName())
   {
      std::cerr << "New mode\n";
      std::string current;
      std::string target;
      try 
      {
         current = ipRecv["current"].get();
      }
      catch(...)
      {
         current = "";
      }
      
      try 
      {
         target = ipRecv["target"].get();
      }
      catch(...)
      {
         target = "";
      }
      
      if(target == "") target = current;
      
      if(m_cameraModes.count(target) == 0 )
      {
         return log<text_log, -1>("Unrecognized mode requested: " + target, logPrio::LOG_ERROR);
      }
      
      updateIfChanged(m_indiP_mode, "target", target);
      
      //Now signal the f.g. thread to reconfigure
      m_nextMode = target;
      m_reconfig = true;
      
      return 0;
   }
   return -1;
}

INDI_NEWCALLBACK_DEFN(andorCtrl, m_indiP_fps)(const pcf::IndiProperty &ipRecv)
{
   if (ipRecv.getName() == m_indiP_fps.getName())
   {
      float current = -99, target = -99;

      try
      {
         current = ipRecv["current"].get<float>();
      }
      catch(...){}
      
      try
      {
         target = ipRecv["target"].get<float>();
      }
      catch(...){}
      
      if(target == -99) target = current;
      
      if(target <= 0) return 0;
      
      //Lock the mutex, waiting if necessary
      std::unique_lock<std::mutex> lock(m_indiMutex);

      updateIfChanged(m_indiP_fps, "target", target);
      
      return setFPS(target);
      
   }
   return -1;
}

INDI_NEWCALLBACK_DEFN(andorCtrl, m_indiP_emGain)(const pcf::IndiProperty &ipRecv)
{
   if (ipRecv.getName() == m_indiP_emGain.getName())
   {
      unsigned current = 0, target = 0;

      if(ipRecv.find("current"))
      {
         current = ipRecv["current"].get<unsigned>();
      }

      if(ipRecv.find("target"))
      {
         target = ipRecv["target"].get<unsigned>();
      }
      
      if(target == 0) target = current;
      
      if(target == 0) return 0;
      
      //Lock the mutex, waiting if necessary
      std::unique_lock<std::mutex> lock(m_indiMutex);

      updateIfChanged(m_indiP_emGain, "target", target);
      
      return setEMGain(target);
      
   }
   return -1;
}

}//namespace app
} //namespace MagAOX
#endif