/** \file outletController.hpp
 * \author Jared R. Males
 * \brief Declares and defines a power control device framework in the MagAOXApp context
 *
 * \ingroup
 *
 */

#ifndef app_outletController_hpp
#define app_outletController_hpp

#include <mx/mxlib.hpp>
#include <mx/app/application.hpp>
#include <mx/timeUtils.hpp>

#include "../../INDI/libcommon/IndiProperty.hpp"

#include "../indiUtils.hpp"


#define OUTLET_STATE_UNKNOWN (-1)
#define OUTLET_STATE_OFF (0)
#define OUTLET_STATE_INTERMEDIATE (1)
#define OUTLET_STATE_ON (2)


#define OUTLET_E_NOOUTLETS (-10)
#define OUTLET_E_NOCHANNELS (-15)
#define OUTLET_E_NOVALIDCH (-20)

namespace MagAOX
{
namespace app
{
namespace dev
{

/// A generic power controller
/**
  * \ingroup appdev
  */
template<class parentT>
struct outletController
{

   std::vector<int> m_outletStates; ///< The current states of each outlet.  These MUST be updated by derived classes in the overridden \ref updatedOutletState.

   pcf::IndiProperty m_indiP_outletStates; ///< Indi Property to show individual outlet states.

   /// Structure containing the specification of one channel.
   /** A channel may include more than one outlet, may specify the order in which
     * outlets are turned on and/or off, and may specify a delay between turning outlets on
     * and/or off.
     */
   struct channelSpec
   {
      std::vector<size_t> m_outlets; ///< The outlets in this channel

      std::vector<size_t> m_onOrder; ///< [optional] The order in which outlets are turned on.  This contains the indices of m_outlets, not the outlet numbers of the device.
      std::vector<size_t> m_offOrder; ///< [optional] The order in which outlets are turned off.  This contains the indices of m_outlets, not the outlet numbers of the device.

      std::vector<unsigned> m_onDelays; ///< [optional] The delays between outlets in a multi-oultet channel.  The first entry is always ignored.  The second entry is the dealy between the first and second outlet, etc.
      std::vector<unsigned> m_offDelays; ///< [optional] The delays between outlets in a multi-oultet channel.  The first entry is always ignored.  The second entry is the dealy between the first and second outlet, etc.

      pcf::IndiProperty m_indiP_prop;
   };

   /// The map of channel specifications, which can be accessed by their names.
   std::unordered_map<std::string, channelSpec> m_channels;

   ///Setup an application configurator for an outletController
   /** This is currently a no-op
     *
     * \returns 0 on success
     * \returns -1 on failure
     */
   int setupConfig( appConfigurator & config /**< [in] an application configuration to setup */);

   /// Load the [channel] sections from an application configurator
   /** Any "unused" section from the config parser is analyzed to determine if it is a channel specification.
     * If it contains the `outlet` or `outlets` keyword, then it is considered a channel. `outlet` and `outlets`
     * are equivalent, and specify the one or more device outlets included in this channel (i.e. this may be a vector
     * value entry).
     *
     * This function then looks for `onOrder` and `offOrder` keywords, which specify the order outlets are turned
     * on or off by their indices in the vector specified by the `outlet`/`outlets` keyword (i.e not the outlet numbers).
     *
     * Next it looks for `onDelays` and `offDelays`, which specify the delays between outlet operations in milliseconds.
     * The first entry is always ignored, then the second entry specifies the delay between the first and second outlet
     * operation, etc.
     *
     * An example config file section is:
     \verbatim
     [sue]           #this channel will be named sue
     outlets=4,5     #this channel uses outlets 4 and 5
     onOrder=1,0     #outlet 5 will be turned on first
     offOrder=0,1    #Outlet 4 will be turned off first
     onDelays=0,150  #a 150 msec delay between outlet turn on
     offDelays=0,345 #a 345 msec delay between outlet turn off
     \endverbatim
     *
     * \returns 0 on success
     * \returns -1 on failure
     */
   int loadConfig( appConfigurator & config /**< [in] an application configuration from which to load values */);

   /// Sets the number of outlets.  This should be called by the derived class constructor.
   /**
     * \returns 0 on success
     * \returns -1 on failure
     */
   int setNumberOfOutlets( int numOuts /**< [in] the number of outlets to allocate */);

   /// Get the currently stored outlet state, without updating from device.
   int outletState( int outletNum );

   /// Get the state of the outlet from the device.
   /** This will be implemented in derived classes to update the outlet state.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   virtual int updateOutletState( int outletNum /**< [in] the outlet number to update */) = 0;

   /// Get the states of all outlets from the device.
   /** The default implementation for-loops through each outlet, calling \ref updateOutletState.
     * Can be re-implemented in derived classes to update the outlet state.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   virtual int updateOutletStates();

   /// Turn an outlet on.
   /** This will be implemented in derived classes to turn an outlet on.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   virtual int turnOutletOn( int outletNum /**< [in] the outlet number to turn on */) = 0;

   /// Turn an outlet off.
   /** This will be implemented in derived classes to turn an outlet off.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   virtual int turnOutletOff( int outletNum /**< [in] the outlet number to turn off */) = 0;

   /// Get the number of channels
   /**
     * \returns the number of entries in m_channels.
     */
   size_t numChannels();

   /// Get the vector of outlet indices for a channel.
   /** Mainly used for testing.
     *
     * \returns the m_outlets member of the channelSpec specified by its name.
     */
   std::vector<size_t> channelOutlets( const std::string & channel /**< [in] the name of the channel */);

   /// Get the vector of outlet on orders for a channel.
   /** Mainly used for testing.
     *
     * \returns the m_onOrder member of the channelSpec specified by its name.
     */
   std::vector<size_t> channelOnOrder( const std::string & channel /**< [in] the name of the channel */);

   /// Get the vector of outlet off orders for a channel.
   /** Mainly used for testing.
     *
     * \returns the m_offOrder member of the channelSpec specified by its name.
     */
   std::vector<size_t> channelOffOrder( const std::string & channel /**< [in] the name of the channel */);

   /// Get the vector of outlet on delays for a channel.
   /** Mainly used for testing.
     *
     * \returns the m_onDelays member of the channelSpec specified by its name.
     */
   std::vector<unsigned> channelOnDelays( const std::string & channel /**< [in] the name of the channel */);

   /// Get the vector of outlet off delays for a channel.
   /** Mainly used for testing.
     *
     * \returns the m_offDelays member of the channelSpec specified by its name.
     */
   std::vector<unsigned> channelOffDelays( const std::string & channel /**< [in] the name of the channel */);

   /// Get the state of a channel.
   /**
     * \returns OUTLET_STATE_UNKNOWN if the state is not known
     * \returns OUTLET_STATE_OFF if the channel is off (all outlets off)
     * \returns OUTLET_STATE_INTERMEDIATE if outlets are intermediate or not in the same state
     * \returns OUTLET_STATE_ON if channel is on (all outlets on)
     */
   int channelState( const std::string & channel /**< [in] the name of the channel */);

   /// Turn a channel on.
   /** This implements the outlet order and delay logic.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   int turnChannelOn( const std::string & channel /**< [in] the name of the channel to turn on*/);

   /// Turn a channel off.
   /** This implements the outlet order and delay logic.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   int turnChannelOff( const std::string & channel /**< [in] the name of the channel to turn on*/);


   /** \name INDI Setup
     *@{
     */

   /// The static callback function to be registered for the channel properties.
   static int st_newCallBack_channels( void * app,
                                       const pcf::IndiProperty &ipRecv
                                    );

   /// The callback called by the static version, to actually process the new request.
   int newCallBack_channels( const pcf::IndiProperty &ipRecv );

   /// Setup the INDI properties for this device controller
   /** This should be called in the `appStartup` function of the derived MagAOXApp.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   int setupINDI();

   /// Update the INDI properties for this device controller
   /** You should call this after updating the outlet states.
     * It is not called automatically.
     *
     * \returns 0 on success.
     * \returns -1 on error.
     */
   int updateINDI();

   ///@}
};

template<class parentT>
int outletController<parentT>::setupConfig( appConfigurator & config )
{
   static_cast<void>(config);

   return 0;
}

template<class parentT>
int outletController<parentT>::loadConfig( appConfigurator & config )
{
   if( m_outletStates.size() == 0) return OUTLET_E_NOOUTLETS;

   //Get the "unused" sections.
   std::vector<std::string> sections;

   config.unusedSections(sections);

   if( sections.size() == 0 ) return OUTLET_E_NOCHANNELS;

   //Now see if any are channels, which means they have an outlet= or outlets= entry
   std::vector<std::string> chSections;

   for(size_t i=0;i<sections.size(); ++i)
   {
      if( config.isSetUnused( iniFile::makeKey(sections[i], "outlet" ))
              || config.isSetUnused( iniFile::makeKey(sections[i], "outlets" )) )
      {
         chSections.push_back(sections[i]);
      }
   }

   if( chSections.size() == 0 ) return OUTLET_E_NOVALIDCH;

   //Now configure the chanels.
   for(size_t n = 0; n < chSections.size(); ++n)
   {
      m_channels.emplace( chSections[n] , channelSpec());

      //---- Set outlets ----
      std::vector<size_t> outlets;
      if( config.isSetUnused( iniFile::makeKey(chSections[n], "outlet" )))
      {
         config.configUnused( outlets, iniFile::makeKey(chSections[n], "outlet" ) );
      }
      else
      {
         config.configUnused( outlets, iniFile::makeKey(chSections[n], "outlets" ) );
      }

      m_channels[chSections[n]].m_outlets = outlets;
      ///\todo error checking on outlets

      //---- Set optional configs ----
      if( config.isSetUnused( iniFile::makeKey(chSections[n], "onOrder" )))
      {
         std::vector<size_t> onOrder;
         config.configUnused( onOrder, iniFile::makeKey(chSections[n], "onOrder" ) );
         m_channels[chSections[n]].m_onOrder = onOrder;
         ///\todo error checking on onOrder
      }

      if( config.isSetUnused( iniFile::makeKey(chSections[n], "offOrder" )))
      {
         std::vector<size_t> offOrder;
         config.configUnused( offOrder, iniFile::makeKey(chSections[n], "offOrder" ) );
         m_channels[chSections[n]].m_offOrder = offOrder;
         ///\todo error checking on offOrder
      }

      if( config.isSetUnused( iniFile::makeKey(chSections[n], "onDelays" )))
      {
         std::vector<unsigned> onDelays;
         config.configUnused( onDelays, iniFile::makeKey(chSections[n], "onDelays" ) );
         m_channels[chSections[n]].m_onDelays = onDelays;
         ///\todo error checking on onDelays
      }

      if( config.isSetUnused( iniFile::makeKey(chSections[n], "offDelays" )))
      {
         std::vector<unsigned> offDelays;
         config.configUnused( offDelays, iniFile::makeKey(chSections[n], "offDelays" ) );
         m_channels[chSections[n]].m_offDelays = offDelays;
         ///\todo error checking on offDelays
      }
   }

   return 0;
}

template<class parentT>
int outletController<parentT>::setNumberOfOutlets( int numOuts )
{
   m_outletStates.resize(numOuts, -1);
   return 0;
}

template<class parentT>
int outletController<parentT>::outletState( int outletNum )
{
   return m_outletStates[outletNum];
}

template<class parentT>
int outletController<parentT>::updateOutletStates()
{
   for(size_t n=0; n<m_outletStates.size(); ++n)
   {
      int rv = updateOutletState(n);
      if(rv < 0) return rv;
   }

   return 0;
}

template<class parentT>
size_t outletController<parentT>::numChannels()
{
   return m_channels.size();
}

template<class parentT>
std::vector<size_t> outletController<parentT>::channelOutlets( const std::string & channel )
{
   return m_channels[channel].m_outlets;
}

template<class parentT>
std::vector<size_t> outletController<parentT>::channelOnOrder( const std::string & channel )
{
   return m_channels[channel].m_onOrder;
}

template<class parentT>
std::vector<size_t> outletController<parentT>::channelOffOrder( const std::string & channel )
{
   return m_channels[channel].m_offOrder;
}

template<class parentT>
std::vector<unsigned> outletController<parentT>::channelOnDelays( const std::string & channel )
{
   return m_channels[channel].m_onDelays;
}

template<class parentT>
std::vector<unsigned> outletController<parentT>::channelOffDelays( const std::string & channel )
{
   return m_channels[channel].m_offDelays;
}

template<class parentT>
int outletController<parentT>::channelState( const std::string & channel )
{
   int st = outletState(m_channels[channel].m_outlets[0]);

   for( size_t n = 1; n < m_channels[channel].m_outlets.size(); ++n )
   {
      if( st != outletState(m_channels[channel].m_outlets[n]) ) st = 1;
   }

   return st;
}

template<class parentT>
int outletController<parentT>::turnChannelOn( const std::string & channel )
{
   //If order is specified, get first outlet number
   size_t n = 0;
   if( m_channels[channel].m_onOrder.size() == m_channels[channel].m_outlets.size() ) n = m_channels[channel].m_onOrder[0];

   //turn on first outlet.
   turnOutletOn(m_channels[channel].m_outlets[n]);

   //Now do the rest
   for(size_t i = 1; i< m_channels[channel].m_outlets.size(); ++i)
   {
      //If order is specified, get next outlet number
      n=i;
      if( m_channels[channel].m_onOrder.size() == m_channels[channel].m_outlets.size() ) n = m_channels[channel].m_onOrder[i];

      //Delay if specified
      if( m_channels[channel].m_onDelays.size() == m_channels[channel].m_outlets.size() )
      {
         mx::milliSleep(m_channels[channel].m_onDelays[i]);
      }

      //turn on next outlet
      turnOutletOn(m_channels[channel].m_outlets[n]);
   }

   return 0;
}

template<class parentT>
int outletController<parentT>::turnChannelOff( const std::string & channel )
{
   //If order is specified, get first outlet number
   size_t n = 0;
   if( m_channels[channel].m_offOrder.size() == m_channels[channel].m_outlets.size() ) n = m_channels[channel].m_offOrder[0];

   //turn on first outlet.
   turnOutletOff(m_channels[channel].m_outlets[n]);

   //Now do the rest
   for(size_t i = 1; i< m_channels[channel].m_outlets.size(); ++i)
   {
      //If order is specified, get next outlet number
      n=i;
      if( m_channels[channel].m_offOrder.size() == m_channels[channel].m_outlets.size() ) n = m_channels[channel].m_offOrder[i];

      //Delay if specified
      if( m_channels[channel].m_offDelays.size() == m_channels[channel].m_outlets.size() )
      {
         mx::milliSleep(m_channels[channel].m_offDelays[i]);
      }

      //turn off next outlet
      turnOutletOff(m_channels[channel].m_outlets[n]);
   }

   return 0;
}

template<class parentT>
int outletController<parentT>::st_newCallBack_channels( void * app,
                                                         const pcf::IndiProperty &ipRecv
                                                       )
{
   return static_cast<parentT *>(app)->newCallBack_channels(ipRecv);
}

template<class parentT>
int outletController<parentT>::newCallBack_channels( const pcf::IndiProperty &ipRecv )
{
   //Interogate ipRecv to figure out which channel it is.
   //And then call turn on or turn off based on requested state.
   std::string name = ipRecv.getName();

   std::string state, target;

   try
   {
      state = ipRecv["state"].get<std::string>();
   }
   catch(...){}

   try
   {
      target = ipRecv["target"].get<std::string>();
   }
   catch(...){}

   if( target == "" ) target = state;
   target = mx::ioutils::toUpper(target);

   if( target == "ON" )
   {
      return turnChannelOn(name);
   }

   if(target == "OFF")
   {
      return turnChannelOff(name);
   }

   return 0;
}

template<class parentT>
int outletController<parentT>::setupINDI()
{
   //Create channel properties and register callback.
   for(auto it = m_channels.begin(); it != m_channels.end(); ++it)
   {
      it->second.m_indiP_prop = pcf::IndiProperty (pcf::IndiProperty::Text);
      it->second.m_indiP_prop.setDevice(static_cast<parentT *>(this)->configName());
      it->second.m_indiP_prop.setName(it->first);
      it->second.m_indiP_prop.setPerm(pcf::IndiProperty::ReadWrite);
      it->second.m_indiP_prop.setState( pcf::IndiProperty::Idle );

      //add elements 'state' and 'target'
      it->second.m_indiP_prop.add (pcf::IndiElement("state"));
      it->second.m_indiP_prop.add (pcf::IndiElement("target"));

      //auto result = indiNewCallBacks.insert( {it->first, {&it->second.m_indiP_prop, newCallBack_channels}});
      auto result = static_cast<parentT *>(this)->m_indiNewCallBacks.insert( {it->first, {&it->second.m_indiP_prop, st_newCallBack_channels}});

      if(!result.second)
      {
         return -1;
      }
   }

   //Register the outletStates INDI property, and add an element for each outlet.
   m_indiP_outletStates = pcf::IndiProperty (pcf::IndiProperty::Text);
   m_indiP_outletStates.setDevice(static_cast<parentT *>(this)->configName());
   m_indiP_outletStates.setName("outlet");
   m_indiP_outletStates.setPerm(pcf::IndiProperty::ReadWrite);
   m_indiP_outletStates.setState( pcf::IndiProperty::Idle );


   auto result =  static_cast<parentT *>(this)->m_indiNewCallBacks.insert( { "outlet", {&m_indiP_outletStates, nullptr}});

   if(!result.second)
   {
      return -1;
   }

   for(size_t i=0; i< m_outletStates.size(); ++i)
   {
      m_indiP_outletStates.add (pcf::IndiElement(std::to_string(i+1)));
   }

   return 0;
}

std::string stateIntToString(int st)
{
   if( st == OUTLET_STATE_OFF ) return "Off";
   else if( st == OUTLET_STATE_INTERMEDIATE ) return "Int";
   else if( st == OUTLET_STATE_ON ) return "On";
   else return "Unk";
}

template<class parentT>
int outletController<parentT>::updateINDI()
{
   if( !static_cast<parentT*>(this)->m_indiDriver ) return 0;

   //Publish outlet states (only bother if they've changed)
   for(size_t i=0; i< m_outletStates.size(); ++i)
   {
      indi::updateIfChanged(m_indiP_outletStates, std::to_string(i+1), stateIntToString(m_outletStates[i]), static_cast<parentT*>(this)->m_indiDriver);
   }

   //Publish channel states (only bother if they've changed)
   for(auto it = m_channels.begin(); it != m_channels.end(); ++it)
   {
      std::string state = stateIntToString( channelState( it->first ));

      std::string target;
      //target = (it->second.m_indiP_prop)["target"].get<std::string>();

      if(target == state) target = "";

      indi::updateIfChanged( it->second.m_indiP_prop, "state", state, static_cast<parentT*>(this)->m_indiDriver );
      indi::updateIfChanged( it->second.m_indiP_prop, "target", target, static_cast<parentT*>(this)->m_indiDriver );
   }

   return 0;
}

} //namespace dev
} //namespace app
} //namespace MagAOX

#endif //app_outletController_hpp