#ifndef CHIVOS_H
#define CHIVOS_H

#include <Arduboy2.h>
#include <util/crc16.h>


extern Arduboy2 arduboy;
bool useEEPROM=true;


uint8_t crc8(const uint8_t *data,uint16_t len)
{
  uint8_t crc = 0;
  uint16_t idx;
  for (idx = 0; idx < len; idx++)
      crc = _crc_ibutton_update(crc, data[idx]);
  return crc; // must be 0
}


typedef enum
{
  CHIVO_FrostySnub,
  CHIVO_UserSnub,
  CHIVO_KilledFrosty,
  CHIVO_YouDied,
  CHIVO_FrostysHeart,
  CHIVO_FrostyFriendzone,
  CHIVO_UserFriendzone,
  CHIVO_Murderer
} Achievements;

const char Chivo01Text[] PROGMEM = "Snubbed by Frosty";
const char Chivo02Text[] PROGMEM = "Snubbed Frosty";
const char Chivo03Text[] PROGMEM = "You killed Frosty";
const char Chivo04Text[] PROGMEM = "You died";
const char Chivo05Text[] PROGMEM = "Won Frosty's heart";
const char Chivo06Text[] PROGMEM = "Frosty friendzone";
const char Chivo07Text[] PROGMEM = "Friendzone Frosty";
const char Chivo08Text[] PROGMEM = "Murderer";

const char * const ChivoLines[] PROGMEM =
{
  Chivo01Text,
  Chivo02Text,
  Chivo03Text,
  Chivo04Text,
  Chivo05Text,
  Chivo06Text,
  Chivo07Text,
  Chivo08Text
};

const char EEPROMInfo01Text[] PROGMEM = "Holidate can use the";
const char EEPROMInfo02Text[] PROGMEM = "EEPROM to store";
const char EEPROMInfo03Text[] PROGMEM = "progress.";
const char EEPROMMenuItem01Text[] PROGMEM = "Use EEPROM";
const char EEPROMMenuItem02Text[] PROGMEM = "Don't use EEPROM";
const char EEPROMMenuItem03Text[] PROGMEM = "Backup EEPROM";
const char EEPROMMenuItem04Text[] PROGMEM = "Restore EEPROM";


const char * const EEPROM_MenuLines[] PROGMEM =
{
  EEPROMMenuItem01Text,
  EEPROMMenuItem02Text,
  EEPROMMenuItem03Text,
  EEPROMMenuItem04Text
};


const char EEPROM_BackupText[] PROGMEM = "Backing up EEPROM";
const char EEPROM_RestoreText[] PROGMEM = "Restoring EEPROM";
const char CRCMismatchText[] PROGMEM = "CRC mistmatch!";
const char RestoreFailedText[] PROGMEM = "Restore failed!";

#define CHARWIDTH 6
#define CHARHEIGHT 8

void CText(int16_t nChars,int16_t nLines)
{
  arduboy.setCursor((WIDTH / 2) - ((nChars * CHARWIDTH) / 2),
                    (nLines < 0) ? arduboy.getCursorY() : (HEIGHT / 2) - ((nLines * CHARHEIGHT) / 2)
                    );
}

void EEPROM_MenuDisplay(char *tBuffer,uint8_t menuItem)
{
  CText(strlen_P(EEPROMInfo01Text),8);
  arduboy.println(strcpy_P(tBuffer,EEPROMInfo01Text));
  CText(strlen_P(EEPROMInfo02Text),-1);
  arduboy.println(strcpy_P(tBuffer,EEPROMInfo02Text));
  CText(strlen_P(EEPROMInfo03Text),-1);
  arduboy.println(strcpy_P(tBuffer,EEPROMInfo03Text));
  arduboy.println("");
  uint8_t idx;
  for (idx=0;idx<(sizeof(EEPROM_MenuLines)/sizeof(EEPROM_MenuLines[0]));idx++)
  {
    arduboy.setTextColor((idx==menuItem) ? BLACK : WHITE);
    arduboy.setTextBackground((idx==menuItem) ? WHITE : BLACK);      
    CText(strlen_P((char*)pgm_read_word(&(EEPROM_MenuLines[idx]))),-1);
    arduboy.println(strcpy_P(tBuffer,(char*)pgm_read_word(&(EEPROM_MenuLines[idx]))));
  }
  arduboy.setTextColor(WHITE);
  arduboy.setTextBackground(BLACK);
}


void SetupSerial()
{
  if (!Serial)
  {
    Serial.begin(115200);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  }
}

void BackupEEPROM()
{
  SetupSerial();
  unsigned int idx;
  uint8_t *tBuffer=arduboy.getBuffer();
  for (idx=0;idx<EEPROM.length();idx++)
  {
    tBuffer[idx]=EEPROM[idx];
  }
  uint8_t tCRC=crc8(tBuffer,EEPROM.length());
  idx=0;
  while (idx<EEPROM.length())
  {
    Serial.write(EEPROM[idx]);
    idx++;
  }
  Serial.write(tCRC);
}


void RestoreEEPROM(char *tBuffer)
{
  SetupSerial();
  unsigned int idx=0;
  while (idx<EEPROM.length())
  {
    if (Serial.available()>0)
    {
      EEPROM.update(idx,Serial.read());
      idx++;
    }
  }
  bool crcRead=false;
  uint8_t tCRC=0;
  while(!crcRead)
  {
    if (Serial.available()>0)
    {
      tCRC=Serial.read();
      crcRead=true;
    }
  }
  uint8_t *cBuffer=arduboy.getBuffer();
  for (idx=0;idx<EEPROM.length();idx++)
  {
    cBuffer[idx]=EEPROM[idx];
  }
  if (tCRC!=crc8(cBuffer,EEPROM.length()))
  {
    arduboy.clear();
    CText(strlen_P(CRCMismatchText),2);
    arduboy.println(strcpy_P(tBuffer,CRCMismatchText));
    CText(strlen_P(RestoreFailedText),-1);
    arduboy.println(strcpy_P(tBuffer,RestoreFailedText));
    arduboy.display();
    arduboy.pollButtons();
    while (!arduboy.justPressed(B_BUTTON)) arduboy.pollButtons();
  }
}


bool EEPROM_MenuInput(char *tBuffer,uint8_t &menuItem,uint8_t &lastMode)
{
  bool done=false;
  if (arduboy.justPressed(UP_BUTTON))
  {
    if (menuItem>0)
    {
      menuItem--;
      lastMode=-1;
    }
  } else if (arduboy.justPressed(DOWN_BUTTON))
  {
    if (menuItem<((sizeof(EEPROM_MenuLines)/sizeof(EEPROM_MenuLines[0]))-1))
    {
      menuItem++;
      lastMode=-1;
    }
  }
  else if (arduboy.justPressed(B_BUTTON))
  {
    switch(menuItem)
    {
    case 0:
      useEEPROM=true;
      done=true;
      break;
    case 1:
      useEEPROM=false;
      done=true;
      break;
    case 2:
      arduboy.clear();
      CText(strlen_P(EEPROM_BackupText),1);
      arduboy.println(strcpy_P(tBuffer,EEPROM_BackupText));
      arduboy.display();
      BackupEEPROM();
      lastMode=-1;
      break;
    case 3:
      arduboy.clear();
      CText(strlen_P(EEPROM_RestoreText),1);
      arduboy.println(strcpy_P(tBuffer,EEPROM_RestoreText));
      arduboy.display();
      RestoreEEPROM(tBuffer);
      lastMode=-1;
      break;
    }
  }
  return done;
}


typedef struct
{
  uint8_t index;
  uint8_t chivos;
  char name[11];
  uint8_t crc;
} ProgressEntry;


// -1 return indicates no valid score entry was found
int16_t ReadProgress(ProgressEntry &tScore)
{
  //Serial.println("Entering ReadProgress");
  int16_t address=EEPROM_STORAGE_SPACE_START,lastGood=-1;
  uint8_t *data=(uint8_t*)&tScore;
  int16_t ofs;
  uint8_t lastIndex=0;
  uint8_t tCrc;
  char tInitials[4];
  
  while ((address+sizeof(ProgressEntry))<EEPROM.length())
  {
    /*Serial.print("address=0x");
    Serial.println(address,HEX);*/
    for (ofs=0;ofs<sizeof(ProgressEntry);ofs++) data[ofs]=EEPROM[address+ofs];
    tCrc=crc8(data,sizeof(ProgressEntry)-sizeof(tScore.crc));
    /*Serial.print("tScore.index=");
    Serial.println(tScore.index);
    Serial.print("tScore.stage=");
    Serial.println(tScore.stage);
    Serial.print("tScore.initials=");
    sprintf(tInitials,"%c%c%c",tScore.initials[0],tScore.initials[1],tScore.initials[2]);
    Serial.println(tInitials);
    Serial.print("tScore.score=");
    Serial.println(tScore.score);
    Serial.print("tScore.crc=0x");
    Serial.println(tScore.crc,HEX);*/
    if (tCrc==tScore.crc)
    {      
      if ((address==EEPROM_STORAGE_SPACE_START)
          ||
          (lastIndex==(tScore.index-1))
          )
      {
        //Serial.println("Found current entry.");
        lastIndex=tScore.index;
        lastGood=address;
      }
      else
      {
        //Serial.println("Found old entry.");
        // Since crc is valid this is likely an old entry
        // We have found the last good entry already unless lastGood is -1
        break;
      }        
      address+=sizeof(ProgressEntry);
    }
    else
    {
      // Invalid crc
      //Serial.println("Invalid CRC.");
      // We have found the last good entry already unless lastGood is -1
      break;
    }
  }
  if ((lastGood>=EEPROM_STORAGE_SPACE_START)
      &&
      ((lastGood+sizeof(ProgressEntry))<EEPROM.length())
      )
  {
    for (ofs=0;ofs<sizeof(ProgressEntry);ofs++) data[ofs]=EEPROM[lastGood+ofs];
  }
  else
  {
    //Default entry to 0 score
    memset(data,0,sizeof(ProgressEntry));
  }
  /*Serial.print("Exiting ReadProgress(return=");
  Serial.print(lastGood);
  Serial.println(")");*/
  return lastGood;
}

// address should be the lastGood entry that ReadProgress returns.  Index and crc are calculated automatically so user only needs to set initials, stage and score.
void WriteProgress(int16_t address,ProgressEntry &tScore)
{
  //Serial.println("Entering WriteProgress");
  int16_t ofs;
  uint8_t *data=(uint8_t*)&tScore;
  //char tInitials[4];
  if ((address>=EEPROM_STORAGE_SPACE_START)
      &&
      // Making sure we have enough room after the last good entry
      ((address+(sizeof(ProgressEntry)*2))<EEPROM.length())
      )
  {
    // Using valid entry to set index
    tScore.index=EEPROM[address]+1;
    // And of course we want the next slot
    address+=sizeof(ProgressEntry);
  }
  else
  {
    if (address==-1)
    {
      tScore.index=0;
    }
    // Using valid entry to set index.  Trusting ReadProgress won't give us anything but -1 if the address is invalid
    else
    {
      tScore.index=EEPROM[address]+1;
    }
    // Starting over at the beginning of user EEPROM
    address=EEPROM_STORAGE_SPACE_START;
  }
  /*Serial.print("address=0x");
  Serial.println(address,HEX);*/
  tScore.crc=crc8(data,sizeof(ProgressEntry)-sizeof(tScore.crc));
  /*Serial.print("tScore.index=");
  Serial.println(tScore.index);
  Serial.print("tScore.stage=");
  Serial.println(tScore.stage);
  Serial.print("tScore.initials=");
  sprintf(tInitials,"%c%c%c",tScore.initials[0],tScore.initials[1],tScore.initials[2]);
  Serial.println(tInitials);
  Serial.print("tScore.score=");
  Serial.println(tScore.score);
  Serial.print("tScore.crc=0x");
  Serial.println(tScore.crc,HEX);*/
  for (ofs=0;ofs<sizeof(ProgressEntry);ofs++)
  {
    //Heard update can reduce wear on the flash
    EEPROM.update(address+ofs,data[ofs]);
  }
  //Serial.println("Exiting WriteProgress");
}


void WriteChivos(uint16_t tChivos,char *name)
{
  if (useEEPROM)
  {
    ProgressEntry tProgress;
    uint16_t lastGood=ReadProgress(tProgress);
    tProgress.chivos=tChivos;
    if (name)
    {
      strcpy((char*)tProgress.name,name);
    }
    WriteProgress(lastGood,tProgress);
  }
}

#endif //CHIVOS_H
