/*
 * SerialMP3Player.h - Library for Serial MP3 Player board from Catalex (YX5300 chip)
 * Created by Salvador Rueda Pau, July 23, 2016.
 * License as GPL v3 (http://www.gnu.org/licenses/gpl.html)
 *
 */

#include "Arduino.h"
#include "YX5300.h"

// Uncomment next line if you are using an Arduino Mega.
//#define mp3 Serial3    // Connect the MP3 Serial Player to the Arduino MEGA Serial3 (14 TX3 -> RX, 15 RX3 -> TX)

//YX5300::YX5300(Stream &stream){
   //SoftwareSerial Serial3 = SoftwareSerial(10, 11);
//  this->serial = &stream;
  //serial->begin(9600);
//}
/*
YX5300::YX5300(){
   //SoftwareSerial Serial3 = SoftwareSerial(10, 11);
   Serial3 = new SoftwareSerial(10, 11);
   _showDebugMessages = false;
}
YX5300::YX5300(int RX, int TX){
   //SoftwareSerial Serial3 = SoftwareSerial(RX, TX);
   Serial3 = new SoftwareSerial(RX, TX);
   _showDebugMessages = false;
}
*/
void YX5300::showDebug(bool op){
  // showDebug (op) 0:OFF 1:ON
    _showDebugMessages = op;
}


void YX5300::begin(Stream &s){
  serial = &s;
}

//int YX5300::available(){
//  return serial->available();
//}

//char YX5300::read(){
//  return serial->read();
//}

void YX5300::playNext(){
  sendCommand(CMD_NEXT);
}

void YX5300::playPrevious(){
  sendCommand(CMD_PREV);
}

void YX5300::volUp(){
   sendCommand(CMD_VOL_UP);
}

void YX5300::volDown(){
   sendCommand(CMD_VOL_DOWN);
}

void YX5300::setVol(byte v){
   // Set volumen (0-30)
   sendCommand(CMD_SET_VOL, v);
}

void YX5300::playSL(byte n){
   // Play single loop n file
   sendCommand(CMD_PLAY_SLOOP, n);
}

void YX5300::playSL(byte f, byte n){
   // Single loop play n file from f folder
   sendCommand(CMD_PLAY_SLOOP, f, n);
}

void YX5300::play(){
   sendCommand(CMD_PLAY);
}

void YX5300::pause(){
   sendCommand(CMD_PAUSE);
}

void YX5300::play(byte n){
   // n number of the file that must be played.
   // n possible values (1-255)
   sendCommand(CMD_PLAYN, n);
}

void YX5300::play(byte n, byte vol){
   // n number of the file that must be played

   sendCommand(CMD_PLAY_W_VOL, vol, n);
}

void YX5300::playF(byte f){
   // Play all files in the f folder

   sendCommand(CMD_FOLDER_CYCLE, f, 0);
}

void YX5300::stop(){
   sendCommand(CMD_STOP_PLAY);
}

void YX5300::qPlaying(){
  // Ask for the file is playing
   sendCommand(CMD_PLAYING_N);
}

void YX5300::qStatus(){
   // Ask for the status.
   sendCommand(CMD_QUERY_STATUS);
}

void YX5300::qVol(){
  // Ask for the volumen
   sendCommand(CMD_QUERY_VOLUME);
}

void YX5300::qFTracks(){    // !!! Nonsense answer
  // Ask for the number of tracks folders
   sendCommand(CMD_QUERY_FLDR_TRACKS);
}

void YX5300::qTTracks(){
  // Ask for the total of tracks
   sendCommand(CMD_QUERY_TOT_TRACKS);
}

void YX5300::qTFolders(){
  // Ask for the number of folders
   sendCommand(CMD_QUERY_FLDR_COUNT);
}

void YX5300::sleep(){
  // Send sleep command
  sendCommand(CMD_SLEEP_MODE);
}

void YX5300::wakeup(){
  // Send wake up command
  sendCommand(CMD_WAKE_UP);
}

void YX5300::reset(){
  // Send reset command
  sendCommand(CMD_RESET);
}



void YX5300::sendCommand(byte command){
  sendCommand(command, 0, 0);
}

void YX5300::sendCommand(byte command, byte dat2){
  sendCommand(command, 0, dat2);
}


void YX5300::sendCommand(byte command, byte dat1, byte dat2){
  byte Send_buf[8] = {0}; // Buffer for Send commands.
  String mp3send = "";

  // Command Structure 0x7E 0xFF 0x06 CMD FBACK DAT1 DAT2 0xEF

  //#ifndef NO_SERIALMP3_DELAY
  delay(20);
  //#endif

  Send_buf[0] = 0x7E;    // Start byte
  Send_buf[1] = 0xFF;    // Version
  Send_buf[2] = 0x06;    // Command length not including Start and End byte.
  Send_buf[3] = command; // Command
  Send_buf[4] = 0x01;    // Feedback 0x00 NO, 0x01 YES
  Send_buf[5] = dat1;    // DATA1 datah
  Send_buf[6] = dat2;    // DATA2 datal
  Send_buf[7] = 0xEF;    // End byte

  for(int i=0; i<8; i++)
  {
    serial->write(Send_buf[i]) ;
    mp3send+=sbyte2hex(Send_buf[i]);
  }
  if (_showDebugMessages){
     Serial.print("Sending: ");
     Serial.println(mp3send); // watch what are we sending
  }

 // #ifndef NO_SERIALMP3_DELAY
  delay(1000);
  // Wait between sending commands.
 // #endif
}

//String sanswer(void);
//int iansbuf = 0;                 // Index for answer buffer.
//static uint8_t ansbuf[10] = {0}; // Buffer for the answers.
uint8_t val;

String YX5300::decodeMP3Answer(){
 // Response Structure  0x7E 0xFF 0x06 RSP 0x00 0x00 DAT 0xFE 0xBA 0xEF
  //
  // RSP Response code
  // DAT Response additional data

  String decodedMP3Answer="";

   decodedMP3Answer=sanswer();

     switch (ansbuf[3])
     {
    case 0x3A:
      decodedMP3Answer += " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer += " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer += " -> Error";
      break;

    case 0x41:
      decodedMP3Answer += " -> Data recived correctly. ";
      break;

    case 0x42:
      switch(ansbuf[6]){
        case 0: decodedMP3Answer += " -> Status: stopped";break;
        case 1: decodedMP3Answer += " -> Status: playing";break;
        case 2: decodedMP3Answer += " -> Status: paused"; break;
      }
      break;

    case 0x43:
      decodedMP3Answer += " -> Vol playing: " + String(ansbuf[6], DEC);
      break;

    case 0x48:
      decodedMP3Answer += " -> File count: " + String(ansbuf[6], DEC);
      break;


    case 0x4C:
      decodedMP3Answer += " -> Playing: " + String(ansbuf[6], DEC);
      break;

    case 0x4E:
      decodedMP3Answer += " -> Folder file count: " + String(ansbuf[6], DEC);
      break;

    case 0x4F:
      decodedMP3Answer += " -> Folder count: " + String(ansbuf[6], DEC);
      break;
     }


   ansbuf[3] = 0; // Clear ansbuff.
   return decodedMP3Answer;
}

/******************************************************************************/
/*funcion :MP3Answer() devuelve el bit correspondiente a la solicitud de estatus*/
/*/* Valores que devuelve MP3Answer(); 
  * 0x0001 -> memory card insert
  * 0x0002 -> Complete play
  * 0x0003 -> Error
  * 0x0004 -> Data recive correct
  *  -> status
  *  - 0x000A-> stopped
  *  - 0x000B-> playing  
  *  - 0x000C-> pause  
  * 0x0005 -> Vol playing
  * 0x0006 -> File count
  * 0x0007 -> playing
  * 0x0008 -> folder file  count
  * 0x0009 -> folder count
  * 
  */
  /* a15 a14 a13 a12 a11 a10 a9 a8 a7 a6 a5 a4 a3 a2 a1 a0
  */
/* Misael Reyes */
uint16_t YX5300::MP3Answer(){
 // Response Structure  0x7E 0xFF 0x06 RSP 0x00 0x00 DAT 0xFE 0xBA 0xEF
  //
  // RSP Response code
  // DAT Response additional data
  
  uint16_t decodedMP3Answer = 0x0000;
  
  String answer = sanswer();

   

     switch (ansbuf[3])
     {
    case 0x3A:
      decodedMP3Answer = 0x0001;//|= (1<<0);//+= " -> Memory card inserted.";
      break;
   case 0x3B:
      decodedMP3Answer = 0x0011;//|= (1<<0);//+= " -> Memory card inserted.";
      break;

    case 0x3D:
      decodedMP3Answer = 0x0002;/// |= (1<<1); //+= " -> Completed play num " + String(ansbuf[6], DEC);
      break;

    case 0x40:
      decodedMP3Answer = 0x0003;//|= (1<<2); //+= " -> Error";
      break;

    case 0x41: 
	   
       decodedMP3Answer = 0x0004;// |= (1<<3); //+= " -> Data recived correctly. ";
	  
      break;

    case 0x42:
	     
      switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x0A;break;//|= (1<<4) | (1<<12); break;//" -> Status: stopped"; break;
        case 1: decodedMP3Answer = 0x0B;break;//|= (1<<4) | (1<<11); break;//" -> Status: playing"; break;
        case 2: decodedMP3Answer = 0x0C;break;//|= (1<<4) | (1<<10); break;// " -> Status: paused"; break;
      }
      break;

    case 0x43:
       //+= " -> Vol playing: " + String(ansbuf[6], DEC);
	  switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x05;break;//|= (1<<5);                              break;
        case 1: decodedMP3Answer = 0x15;break;//|= (1<<5) | (1<<13);                    break;
        case 2: decodedMP3Answer = 0x25;break;//|= (1<<5) | (1<<14);                    break;
		case 3: decodedMP3Answer = 0x35;break;//|= (1<<5) | (1<<13) | (1<<14);          break;
		case 4: decodedMP3Answer = 0x45;break;//|= (1<<5) | (1<<15);                    break;
		case 5: decodedMP3Answer = 0x55;break;//|= (1<<5) | (1<<13) | (1<<15);          break;
		case 6: decodedMP3Answer = 0x65;break;//|= (1<<5) | (1<<14) | (1<<15);          break;
		case 7: decodedMP3Answer = 0x75;break;//|= (1<<5) | (1<<13) | (1<<14) | (1<<15);break;
		case 8: decodedMP3Answer = 0x0085;break;//|= (1<<5) | (1<<16);                    break;
		case 9: decodedMP3Answer = 0x0095;break;//|= (1<<5) | (1<<13) | (1<<16);          break;
		case 10: decodedMP3Answer = 0x00A5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 11: decodedMP3Answer = 0x00B5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 12: decodedMP3Answer = 0x00C5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 13: decodedMP3Answer = 0x00D5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 14: decodedMP3Answer = 0x00E5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 15: decodedMP3Answer = 0x00F5;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		}
      break;

    case 0x48://" -> File count: "
      switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x06;break;//|= (1<<5);                              break;
        case 1: decodedMP3Answer = 0x16;break;//|= (1<<5) | (1<<13);                    break;
        case 2: decodedMP3Answer = 0x26;break;//|= (1<<5) | (1<<14);                    break;
		case 3: decodedMP3Answer = 0x36;break;//|= (1<<5) | (1<<13) | (1<<14);          break;
		case 4: decodedMP3Answer = 0x46;break;//|= (1<<5) | (1<<15);                    break;
		case 5: decodedMP3Answer = 0x56;break;//|= (1<<5) | (1<<13) | (1<<15);          break;
		case 6: decodedMP3Answer = 0x66;break;//|= (1<<5) | (1<<14) | (1<<15);          break;
		case 7: decodedMP3Answer = 0x76;break;//|= (1<<5) | (1<<13) | (1<<14) | (1<<15);break;
		case 8: decodedMP3Answer = 0x0086;break;//|= (1<<5) | (1<<16);                    break;
		case 9: decodedMP3Answer = 0x0096;break;//|= (1<<5) | (1<<13) | (1<<16);          break;
		case 10: decodedMP3Answer = 0x00A6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 11: decodedMP3Answer = 0x00B6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 12: decodedMP3Answer = 0x00C6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 13: decodedMP3Answer = 0x00D6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 14: decodedMP3Answer = 0x00E6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 15: decodedMP3Answer = 0x00F6;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 16: decodedMP3Answer = 0x0106;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 17: decodedMP3Answer = 0x0116;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 18: decodedMP3Answer = 0x0126;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 19: decodedMP3Answer = 0x0136;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		case 20: decodedMP3Answer = 0x0146;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		}
      break;


    case 0x4C://" -> Playing: "
	  switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x07;break;//|= (1<<5);                              break;
        case 1: decodedMP3Answer = 0x17;break;//|= (1<<5) | (1<<13);                    break;
        case 2: decodedMP3Answer = 0x27;break;//|= (1<<5) | (1<<14);                    break;
		case 3: decodedMP3Answer = 0x37;break;//|= (1<<5) | (1<<13) | (1<<14);          break;
		case 4: decodedMP3Answer = 0x47;break;//|= (1<<5) | (1<<15);                    break;
		case 5: decodedMP3Answer = 0x57;break;//|= (1<<5) | (1<<13) | (1<<15);          break;
		case 6: decodedMP3Answer = 0x67;break;//|= (1<<5) | (1<<14) | (1<<15);          break;
		case 7: decodedMP3Answer = 0x77;break;//|= (1<<5) | (1<<13) | (1<<14) | (1<<15);break;
		case 8: decodedMP3Answer = 0x0087;break;//|= (1<<5) | (1<<16);                    break;
		case 9: decodedMP3Answer = 0x0097;break;//|= (1<<5) | (1<<13) | (1<<16);          break;
		case 10: decodedMP3Answer = 0x00A7;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		
      }
      break;

    case 0x4E: //" -> Folder file count: "
      switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x08;break;//|= (1<<5);                              break;
        case 1: decodedMP3Answer = 0x18;break;//|= (1<<5) | (1<<13);                    break;
        case 2: decodedMP3Answer = 0x28;break;//|= (1<<5) | (1<<14);                    break;
		case 3: decodedMP3Answer = 0x38;break;//|= (1<<5) | (1<<13) | (1<<14);          break;
		case 4: decodedMP3Answer = 0x48;break;//|= (1<<5) | (1<<15);                    break;
		case 5: decodedMP3Answer = 0x58;break;//|= (1<<5) | (1<<13) | (1<<15);          break;
		case 6: decodedMP3Answer = 0x68;break;//|= (1<<5) | (1<<14) | (1<<15);          break;
		case 7: decodedMP3Answer = 0x78;break;//|= (1<<5) | (1<<13) | (1<<14) | (1<<15);break;
		case 8: decodedMP3Answer = 0x0088;break;//|= (1<<5) | (1<<16);                    break;
		case 9: decodedMP3Answer = 0x0098;break;//|= (1<<5) | (1<<13) | (1<<16);          break;
		case 10: decodedMP3Answer = 0x00A8;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		
      }
      break;

    case 0x4F://" -> Folder count: "
	
      switch(ansbuf[6]){
        case 0: decodedMP3Answer = 0x09;break;//|= (1<<5);                              break;
        case 1: decodedMP3Answer = 0x19;break;//|= (1<<5) | (1<<13);                    break;
        case 2: decodedMP3Answer = 0x29;break;//|= (1<<5) | (1<<14);                    break;
		case 3: decodedMP3Answer = 0x39;break;//|= (1<<5) | (1<<13) | (1<<14);          break;
		case 4: decodedMP3Answer = 0x49;break;//|= (1<<5) | (1<<15);                    break;
		case 5: decodedMP3Answer = 0x59;break;//|= (1<<5) | (1<<13) | (1<<15);          break;
		case 6: decodedMP3Answer = 0x69;break;//|= (1<<5) | (1<<14) | (1<<15);          break;
		case 7: decodedMP3Answer = 0x79;break;//|= (1<<5) | (1<<13) | (1<<14) | (1<<15);break;
		case 8: decodedMP3Answer = 0x0089;break;//|= (1<<5) | (1<<16);                    break;
		case 9: decodedMP3Answer = 0x0099;break;//|= (1<<5) | (1<<13) | (1<<16);          break;
		case 10: decodedMP3Answer = 0x00A9;break;//|= (1<<5) | (1<<14) | (1<<16);         break;
		}
      break;
     }


   ansbuf[3]= 0; // Clear ansbuff.
   //ansbuf[6] = 0; // Clear ansbuff.
   return decodedMP3Answer;
}



//=================================================================================
/********************************************************************************/
/*Function: sbyte2hex. Returns a byte data in HEX format.	                */
/*Parameter:- uint8_t b. Byte to convert to HEX.                                */
/*Return: String                                                                */


String YX5300::sbyte2hex(byte b)
{
  String shex;

  //Serial.print("0x");
  shex="0X";

  //if (b < 16) Serial.print("0");
  if (b < 16) shex+="0";
  //Serial.print(b, HEX);
  shex+=String(b,HEX);
  //Serial.print(" ");
  shex+=" ";
  return shex;
}


/********************************************************************************/
/*Function: sanswer. Returns a String answer from mp3 UART module.	            */
/*Return: String.  the answer                                                   */


String YX5300::sanswer(void){
  // Response Structure  0x7E 0xFF 0x06 RSP 0x00 0x00 DAT 0xFE 0xBA 0xEF
  //
  // RSP Response code
  // DAT Response additional data

  // if there are something available start to read from mp3 serial.
  // if there are "0x7E" it's a beginning.
  //
  //  read while something readed and it's not the end "0xEF"


  byte b;
  String mp3answer="";                // Answer from the Serial3.
  int iansbuf = 0;

  while (serial->available() ){//&& b!=0xef
   //do{
    b = serial->read();

    if(b == 0x7E){  // if there are "0x7E" it's a beginning.
      iansbuf=0;    //  ansbuf index to zero.
      mp3answer="";
	  
    }

    ansbuf[iansbuf] = b;
    mp3answer+=sbyte2hex(ansbuf[iansbuf]);
    iansbuf++; //  increase this index.
    
	if(b==0x3A){
	    ansbuf[3] = 0x3a;
	}
	if(b==0x3b){
	    ansbuf[3] = 0x3b;
	}
	if(b==0x3D){
	    ansbuf[3] = 0x3d;
		
	}
	if(b==0xef){
	   // Serial.print("ansbuf[3]:0x");
       // Serial.println(ansbuf[3],HEX);
	    //Serial.print("ansbuf[6]:0x");
	    //Serial.println(ansbuf[6],HEX);
		
	}
   //}while(b != 0xEF);
   // while there are something to read and it's not the end "0xEF"

  }
  
       
  
  return mp3answer;
 }
