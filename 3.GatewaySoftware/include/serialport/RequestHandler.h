/* 
 * @file  RequestHandler.h
 * @author Jeroen Vennegoor op Nijhuis
 *
 * 
 */

#ifndef REQUESTHANDLER_H
#define	REQUESTHANDLER_H

class RequestHandler{
public:
    virtual void handler(std::vector<unsigned char> input )=0;
};

#endif	/* REQUESTHANDLER_H */

