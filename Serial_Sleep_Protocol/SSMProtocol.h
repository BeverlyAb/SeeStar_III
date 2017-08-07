#include <Arduino.h>

/************************* STRUCTURES *************************/

/*
 * Struct: RequestPacket
 * --------------------------------
 * For formatting a request from the Camera Module (CM).
 * 
 * destID:   Unique ID of destination module.
 * 
 * op0:      First opcode in request.
 * 
 * op1:      Second opcode in request.
 * 
 * channel:  Channel to apply operation to.
 */
typedef struct RequestPacket {
  char destID;
  char op0;
  char op1;
  char channel;
/*  char yearTens;
  char yearOnes;
  char monthTens;
  char monthOnes;
  char dayTens;
  char dayOnes;
  char hourTens;
  char hourOnes;
  char minTens;
  char minOnes;
  char secTens;
  */
};

/*
 * Struct: ResponsePacket
 * --------------------------------
 * For sending measurements or other information to the Camera
 * Module (CM). Ordinarilly sent in response to a CM request.
 * 
 * destID:   Unique ID of destination module.
 * 
 * numBytes: size of data in bytes.
 * 
 * *data:    Pointer to data. Actual data will be dereferenced upon
 *           transmission.
 */
typedef struct ResponsePacket {
  char destID;
  unsigned short dataLen;
  char *data;
};

/************************* FUNCTIONS *************************/

void reset(RequestPacket *req);
void decodeRequest(char *buf, RequestPacket *req);

ResponsePacket newResponsePacket();
void reset(ResponsePacket *resp);

int isValidRequest(RequestPacket *req);
void handleRequest(RequestPacket *req, ResponsePacket *resp);

int processRequest(RequestPacket *req, ResponsePacket *resp);
