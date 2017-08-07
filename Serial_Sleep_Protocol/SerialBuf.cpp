/*
 * FILE:        SerialBuf.cpp
 * AUTHOR:      Matt Ducasse
 * DATE:        4/7/17
 * DESCRIPTION: Defines a buffer structure used to
 *              collect incoming serial request packets
 *              from the host module.
 */

#include "SerialBuf.h"
#include <stdlib.h>

/*
 * Function: newSerialBuf
 * --------------------------------
 * Allocates memory for a serial buffer and returns its address.
 */
SerialBuf newSerialBuf() {
  SerialBuf *buf = (SerialBuf*)malloc(sizeof(SerialBuf));
  buf->count = 0;
  buf->state = ST_SERBUF_DISABLED;
  return *buf;
}

/*
 * Function: appendChar
 * --------------------------------
 * Appends a character to a serial buffer (unless full).
 * 
 * *buf: pointer to a serial buffer struct.
 *  
 * c: new character to be appended.
 */
void appendChar(SerialBuf *buf, char c) {
  if(!isFull(buf)) {
    int i = buf->count;
    buf->string[i] = c;
    buf->count++;
  }
}

/*
 * Function: isFull
 * --------------------------------
 * Determines if a serial buffer is full.
 * 
 * *buf: pointer to a serial buffer struct.
 *  
 * Returns: 1 if full, else 0.
 */
int isFull(SerialBuf *buf) {
  return buf->count >= BUFFER_MAX_COUNT;
}

/*
 * Function: reset
 * --------------------------------
 * Empties and disables a serial buffer..
 * 
 * *buf: pointer to a serial buffer struct.
 */
void reset(SerialBuf *buf) {
  buf->count = 0;
  buf->state = ST_SERBUF_DISABLED;
}

