/* 
 * File:   Time.h
 * Author: jeroen
 *
 * Created on April 12, 2015, 10:31 AM
 */

#ifndef TIME_H
#define	TIME_H

#include <ctime>

class Time {
public:
    Time();
  
    virtual ~Time();
    
    tm* getTime();
    
private:
    tm* p_time;

};

#endif	/* TIME_H */

