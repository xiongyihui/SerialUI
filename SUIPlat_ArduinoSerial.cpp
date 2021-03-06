/*
 * SUIPlat_Arduino.cpp
 *
 *  Created on: 2013-12-13
 *      Author: malcalypse
 */


#include "includes/SUIConfig.h"

#ifdef SUI_PLATFORM_ARDUINOSERIAL
#include "includes/SUIPlat_ArduinoSerial.h"



// SUI is the namespace in which we keep all our goodies.
namespace SUI {

// default to using standard HardwareSerial as implementation stream to use.
StreamInstanceType * StreamImplementation::stream_to_use = &Serial;
bool StreamImplementation::stream_to_use_override = false;

void StreamImplementation::setStream(StreamInstanceType * strm) {
	stream_to_use = strm;
	stream_to_use_override = true;
}

void StreamImplementation::begin(unsigned long speed) {


   	if (!stream_to_use_override)
	{
   		Serial.begin(speed);
   	}
   	// we also set a sane timeout for reads, as this was causing issues
   	// assuming code will call begin() prior to setTimeout() (if used)
   	stream_to_use->setTimeout(SUI_SERIALUI_READBYTES_TIMEOUT_DEFAULT_MS);
}

unsigned long SUIStream::timeout() { return timeout_ms; }
void SUIStream::setTimeout(unsigned long timeout) { timeout_ms = timeout ; Stream::setTimeout(timeout); }
#define NO_SKIP_CHAR  1

unsigned long SUIStream::parseULong()
{
	return parseULong(NO_SKIP_CHAR);
}
unsigned long SUIStream::parseULong(char skipChar)
{
	uint32_t value = 0;
	int c;

	c = peekNextDigit();
	// ignore non numeric leading characters
	if (c < 0)
		return 0; // zero returned if timeout

	do {
		if (c == skipChar)
			; // ignore this character
		else if (c >= '0' && c <= '9') // is c a digit?
			value = value * 10 + c - '0';
		read(); // consume the character we got with peek
		c = timedPeek();
	} while ((c >= '0' && c <= '9') || c == skipChar);


	return value;

}


unsigned long SUIStream::parseIntHex()
{
	return parseIntHex(NO_SKIP_CHAR);
}

unsigned long SUIStream::parseIntHex(char skipChar)
{
	uint32_t value = 0;
	int c;

	c = peekNextDigit(true);
	// ignore non numeric leading characters
	if (c < 0)
		return 0; // zero returned if timeout

	do {
		if (c == skipChar)
			; // ignore this character
		else if (c >= '0' && c <= '9') // is c a digit?
			value = (value * 16) + c - '0';
		else if (c >= 'A' && c <= 'F')
			value = (value * 16) + ((c - 'A') + 10);
		else if (c >= 'a' && c <= 'f')
			value = (value * 16) + ((c - 'a') + 10);

		read(); // consume the character we got with peek
		c = timedPeek();
	} while ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || c == skipChar);


	return value;

}

int SUIStream::timedPeek()
{
  int c;
  unsigned long startMillis = millis();
  do {
    c = peek();
    if (c >= 0) return c;
  } while(millis() - startMillis < timeout());
  return -1;     // -1 indicates timeout
}
int SUIStream::peekNextDigit(bool includeHex)
{
  int c;
  while (1) {
    c = timedPeek();
    if (c < 0) return c;  // timeout
    if (c == '-') return c;
    if (c >= '0' && c <= '9') return c;
    if (includeHex)
    {
    	if ( (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
    		return c;
    }
    read();  // discard non-numeric
  }
  return -1;
}


} /* namespace SUI */
#endif
