/* 
 * File:   RequestHandler.h
 * Author: jeroen
 *
 * Created on April 12, 2015, 1:10 PM
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H



class RequestHandler{
public:
    virtual void handler(std::vector<unsigned char> input )=0;
    
};

#endif	/* REQUESTHANDLER_H */

