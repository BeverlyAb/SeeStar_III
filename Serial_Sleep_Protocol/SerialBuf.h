#define BUFFER_MAX_COUNT 16
//1XXYYMMDDHHMMSSC

#define ST_SERBUF_DISABLED 0
#define ST_SERBUF_ENABLED 1

/************************* STRUCTURES *************************/

/*
 * Struct: SerialBuf
 * --------------------------------
 * For safely managing serial messages from Camera Module (CM). Use by
 * setting state to ENABLED when a '$' is recieved from CM, and append subsequent
 * CM bytes until the buffer is full. Any meaningful message will only just
 * fill the buffer. Then, process the contents of the buffer to interpret
 * the CM request.
 * 
 * buffer: pointer to a serial buffer struct.
 *  
 * count: number of characters stored in the buffer
 * 
 * state: Determines if the buffer should be written to.Set to ENABLED when a
 *        designated 'begin' character is recieved. Set to DISABLED once the 
 *        buffer is full.
 */
typedef struct SerialBuf {
  char string[BUFFER_MAX_COUNT];
  unsigned short count;
  unsigned short state;
};

/************************* FUNCTIONS *************************/

SerialBuf newSerialBuf();
void appendChar(SerialBuf *buf, char c);
int isFull(SerialBuf *buf);
void reset(SerialBuf *buf);
