/* 
 * File:   Time.cpp
 * Author: jeroen
 * 
 * Created on April 12, 2015, 10:31 AM
 */

#include "../../include/time/Time.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include <chrono>

using namespace boost::posix_time;

Time::Time() {
}


Time::~Time() {
}


  tm* Time::getTime(){
       std::time_t t = std::time(NULL);
       p_time=std::localtime(&t);
       
    //std::cout << "UTC:   " << std::put_time(std::gmtime(&t), "%c %Z") << '\n';
    //std::cout << "local: " << std::put_time(std::localtime(&t), "%c %Z") << '\n';
      //ptime t(second_clock::local_time());
      //unsigned long T =std::chrono::system_clock::now().time_since_epoch() ;
      
      //unsigned long T = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
      
      return p_time;
  }
