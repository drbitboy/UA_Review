/** \file telem_observer.hpp
  * \brief The MagAO-X logger telem_observer log type.
  * \author Jared R. Males (jaredmales@gmail.com)
  *
  * \ingroup logger_types_files
  * 
  * History:
  * - 2018-09-06 created by JRM
  */
#ifndef logger_types_telem_observer_hpp
#define logger_types_telem_observer_hpp

#include "generated/telem_observer_generated.h"
#include "flatbuffer_log.hpp"

#include <cmath>

namespace MagAOX
{
namespace logger
{


/// Log entry recording the build-time git state.
/** \ingroup logger_types
  */
struct telem_observer : public flatbuffer_log
{
   ///The event code
   static const flatlogs::eventCodeT eventCode = eventCodes::TELEM_OBSERVER;

   ///The default level
   static const flatlogs::logPrioT defaultLevel = flatlogs::logPrio::LOG_TELEM;

   static timespec lastRecord; ///< The time of the last time this log was recorded.  Used by the telemetry system.

   
   ///The type of the input message
   struct messageT : public fbMessage
   {
      ///Construct from components
      messageT( const std::string & email,   /// <[in] observer email
                const std::string & obsName, /// <[in] observer email
                const bool & observing       /// <[in] status of observing
              )
      {
         auto _email = builder.CreateString(email);
         auto _obsName = builder.CreateString(obsName);
         
         auto fp = CreateTelem_observer_fb(builder, _email, _obsName, observing);
         builder.Finish(fp);
      }

   };
                 
 
   ///Get the message formatte for human consumption.
   static std::string msgString( void * msgBuffer,  /**< [in] Buffer containing the flatbuffer serialized message.*/
                                 flatlogs::msgLenT len  /**< [in] [unused] length of msgBuffer.*/
                               )
   {
      static_cast<void>(len);

      auto fbs = GetTelem_observer_fb(msgBuffer);

      std::string msg = "[observer] ";
      
      if(fbs->email())
      {
         msg += "email: ";
         msg += fbs->email()->c_str();
         msg += " ";
      }
      
      if(fbs->obsName())
      {
         msg += "obs: ";
         msg += fbs->obsName()->c_str();
         msg += " ";
      }
      
      msg += std::to_string(fbs->observing());
      
      return msg;
   
   }
   
   static std::string email( void * msgBuffer )
   {
      auto fbs = GetTelem_observer_fb(msgBuffer);
      if(fbs->email() != nullptr)
      {
         return std::string(fbs->email()->c_str());
      }
      else return "";
   }

   static std::string obsName( void * msgBuffer )
   {
      auto fbs = GetTelem_observer_fb(msgBuffer);
      if(fbs->email() != nullptr)
      {
         return std::string(fbs->obsName()->c_str());
      }
      else return "";
   }

   static bool observing( void * msgBuffer )
   {
      auto fbs = GetTelem_observer_fb(msgBuffer);
      return fbs->observing();
   }

   /// Get the logMetaDetail for a member by name
   /**
     * \returns the a logMetaDetail filled in with the appropriate details
     * \returns an empty logmegaDetail if member not recognized
     */ 
   static logMetaDetail getAccessor( const std::string & member /**< [in] the name of the member */ )
   {
      if(     member == "email")     return logMetaDetail({"EMAIL", logMeta::valTypes::String, logMeta::metaTypes::State, (void *) &email});
      else if(member == "obsName")   return logMetaDetail({"NAME", logMeta::valTypes::String, logMeta::metaTypes::State, (void *) &obsName}); 
      else if(member == "observing") return logMetaDetail({"OBSERVING", logMeta::valTypes::Bool, logMeta::metaTypes::State, (void *) &observing}); 
      else
      {
         std::cerr << "No string member " << member << " in telem_observer\n";
         return logMetaDetail();
      }
   }
   
   
}; //telem_observer



} //namespace logger
} //namespace MagAOX

#endif //logger_types_telem_observer_hpp

