/*
 * FILE:        SSMProtocol.cpp
 * AUTHOR:      Matt Ducasse
 * DATE:        4/7/17
 * DESCRIPTION: Defines formats for request and response packets
 *              in communication protocol between Camera Module
 *              and Sensor Module. Also validates and processes
 *              request packets.
 * 
 * EDITOR:      Beverly Abadines
 * DATE:        8/6/17
 * DESCRIPTION: Modified to fit basic serial request onto Feather M0, which does not use 
 *              SoftwareSerial.h
 */

#include "SSMProtocol.h"
#include "SerialBuf.h"
//#include "FakeData.h"
#include <stdlib.h>

#include "Arduino.h"
/*
 * Function: reset
 * --------------------------------
 * Clears the information in a request packet.
 * 
 * *req: Pointer to the RequestPacket to reset.
 */
void reset(RequestPacket *req) {
  req->destID = '0';
  req->op0 = '0';
  req->op1 = '0';
  req->channel = '0';
}

/*
 * Function: decodeRequest
 * --------------------------------
 * Converts the contents of a SerialBuffer to a RequestPacket
 * for processing. Currently copies ASCII chars directly from
 * buffer, but may become more complex if a binary protocol is 
 * adopted.
 * 
 * *string: String contents of buffer to be decoded and copied.
 * 
 * *req: RequestPacket to copy request into.
 */
void decodeRequest(char *string, RequestPacket *req) {
  req->destID = string[1];
  req->op0 = string[2];
  req->op1 = string[3];
  req->channel = string[4];
}

/*
 * Function: reset
 * --------------------------------
 * Clears the information in a response packet and frees
 * allocated data.
 * 
 * *resp: Pointer to the ResponsePacket to reset.
 */
void reset(ResponsePacket *resp) {
  resp->destID = '0';
  resp->dataLen = 0;

  if(resp->data != NULL) {
    free(resp->data);
  }
  
  resp->data = NULL;
}

/*
 * Function: isValidRequest
 * --------------------------------
 * Checks each member of a RequestPacket to verify that
 * it only contains meaningful codes.
 * 
 * *req: Pointer to the RequestPacket to check.
 */
int isValidRequest(RequestPacket *req) {
  if( !(req->op0 == 'T'  || req->op0 == 'S') ||
      !(req->op0 == 'R'  || req->op1 == 'L') ||
      !(req->channel >= '0' && req->channel <= '9')) {
    return 0;
  }      
  return 1;
}

/*
 * Function: handleRequest
 * --------------------------------
 * Processes a RequestPacket and returns a ResponsePacket 
 * if valid, else returns an error message.
 * 
 * *req: Pointer to the RequestPacket to handle.
 * 
 * *resp: Pointer to the ResponsePacket to send back.
 */
void handleRequest(RequestPacket *req, ResponsePacket *resp) {
  if(isValidRequest(req)) {
    processRequest(req, resp);
  }
  else {
    //Return error message
  }
}

/*
 * Function: processRequest
 * --------------------------------
 * Operates the SSM based on the contents of the RequestPacket.
 * If information is requested by the CM, it is packed into
 * a ResponsePacket and sent back.
 * 
 * *req: Pointer to the RequestPacket to process.
 * 
 * *resp: Pointer to the ResponsePacket to send back.
 */
int processRequest(RequestPacket *req, ResponsePacket *resp) {
    if(req->op0 == 'T' && req->op1 == 'S')
      return 1;
    else if(req->op0 == 'R' && req->op1 == 'L')
      return 2;
    else 
      return 0;
}
