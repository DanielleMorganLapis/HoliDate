#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "GameMath.h"
#include "assets.h"
#include "Music.h"
#include "Chivos.h"
#include "Dialog.h"
#include "Plot.h"
#include "Snow.h"


Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

//#define SDEBUG
//#define SCREENSTREAM


typedef enum
{
  MODE_SnowTest=0,
  MODE_EEPROM,
  MODE_Intro,
  MODE_FullScreenImage,
  MODE_Narration,
  MODE_DateDialog,
  MODE_UserResponse,
  MODE_UserNameQuery,
  MODE_GameOver,
  MODE_Chivos,
  MODE_ResetProgress
} GameModes;


uint8_t lastMode=0xFF,mode=MODE_EEPROM,dialogOfs=0,subMenuItem=0,gameOverMode=MODE_Intro,frostyKills=0;;
int8_t dateLove=0,lastLove=0;
bool waitRelease=true,musicPlayed=false,frostyDead=false;
uint8_t chivos=0;
DialogEnt *currentDialog=NULL;
Snow *gSnow=NULL;

// Notice the extra space between const & char, this is to cause the newline to be ignored because at the moment the string parser does not handle escape codes
const  char validNameChars[] PROGMEM = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
#define NUMVALIDNAMECHARS ((sizeof(validNameChars)-sizeof(const char))/sizeof(const char))

/*
 * Hi, I'm Frosty.  What's your name?
 * Your name? Giggle!
 * Nice to meet you, [YourName]!
 * I like to sing, dance and laugh. What do you like to do [YourName]?
 * > Long walks on the beach
 * Beach? Oh, I hope you like cold beaches . . . some of them are a little warm for me.
 * > Ice skating
 * You like ice skating? That sounds like fun! Nice and frosty!
 * > Video games
 * Video games?  Never heard of those before! You'll have to show me sometime!
 */

const char StartText[] PROGMEM = "Start!";
const char AchievementsText[] PROGMEM = "Achievements!";
const char ResetProgressText[]  PROGMEM = "Reset Progress";
const char ReturnToText[] PROGMEM = "Return to";
const char TitleText[] PROGMEM = "Title?";

const char * const IntroMenuLines[] PROGMEM =
{
  StartText,
  AchievementsText,
  ResetProgressText
};


void DrawPortrait()
{
  int8_t tLove=dateLove-lastLove;
  const unsigned char *tPortrait;
  #ifdef SDEBUG
  Serial.print("tLove=");
  Serial.println(tLove);
  #endif
  if (((!frostyDead) || (currentDialog==&FrostyMelted)) && (currentDialog!=&FrostyRestored))
  {
    switch(tLove)
    {
    case -3:
      tPortrait=&frostyFrown[2];
      break;
    case -2:
      tPortrait=&frostyMelted[2];
      break;
    case -1:
      tPortrait=&frostyFrownSweat[2];
      break;
    case 1:
      tPortrait=&frostyEmbarassed[2];
      break;
    case 2:
      tPortrait=&frostyLove[2];
      break;
    case 0:
    default:
      tPortrait=&frostyPortrait[2];
      break;
    }
  }
  else tPortrait=&disappointedSantaInvert2NoGlassesTweak2[2];
  arduboy.drawBitmap(64,
                      0,
                      tPortrait,
                      64,
                      64,
                      WHITE
                      );
}


void setup() {
  arduboy.boot();
  arduboy.safeMode();
  arduboy.audio.begin();
  arduboy.setFrameRate(FPS);
  arduboy.setTextSize(1);
  //Start serial
  #if (defined(SDEBUG) || defined(SCREENSTREAM))
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  //Serial.println("Hello Debug!");
  #endif
  /*for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 255);
  }*/
}


void SelectDialog()
{
  {
    uint8_t tNumDialog=(int8_t)pgm_read_byte(&((((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))[menuItem]).numDialog));
    #ifdef SDEBUG
    Serial.print("tNumDialog=");
    Serial.println(tNumDialog);
    #endif
    if (tNumDialog>1)
    {
      uint8_t idx;
      int8_t tLove=dateLove<0 ? 0 : dateLove,dLove;
      #ifdef SDEBUG
      Serial.print("tLove=");
      Serial.println(tLove);
      #endif
      DialogEnt *tDialog;
      tDialog=(DialogEnt*)pgm_read_word(&((((ResponseEnt*)pgm_read_word(&currentDialog->responses))[menuItem]).dialogs));
      for (idx=0;idx<tNumDialog;idx++)
      {                  
        dLove=(int8_t)pgm_read_byte(&((tDialog[idx]).love));                  
        #ifdef SDEBUG
        Serial.print("dLove=");
        Serial.println(dLove);
        #endif
        if (tLove>=dLove)
        {
          currentDialog=&(tDialog[idx]); 
        }
      }
      if (currentDialog==(&FrostyFeelz[2])) dateLove+=2;
    }
    else currentDialog=(DialogEnt*)pgm_read_word(&((((ResponseEnt*)pgm_read_word(&currentDialog->responses))[menuItem]).dialogs));
  }
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;
  if (mode!=lastMode)
  {
    lastMode=mode;
    char tBuffer[22];
    arduboy.clear();
    switch (mode)
    {
    /*case MODE_SnowTest:
      if (!gSnow)
      {
        srand(rand());
        gSnow=new Snow(20);
      }
      if (gSnow)
      {
        gSnow->Display(arduboy);
        int16_t tVect=subMenuItem;
        gSnow->Move(tVect<<SUBPIXELBITS);
        subMenuItem=0;
      }
      lastMode=-1;
      break;*/
    case MODE_EEPROM:
      EEPROM_MenuDisplay(tBuffer,menuItem);
      break;
    case MODE_Intro:
      {
        if (!musicPlayed)
        {
          sound.tones(frosty);
          musicPlayed=true;
        }
        int x=HolidateLogo[0],y=HolidateLogo[1];
        x=(WIDTH/2)-(x/2);
        y=0/*(HEIGHT/2)-(y/2)*/;
        arduboy.drawBitmap(x,
                            y,
                            &HolidateLogo[2],
                            HolidateLogo[0],
                            HolidateLogo[1],
                            WHITE
                            );
        arduboy.setCursor(2,HolidateLogo[1]+(((HEIGHT-HolidateLogo[1])/2)-(((sizeof(IntroMenuLines)/sizeof(IntroMenuLines[0]))*CHARHEIGHT)/2)));
        uint8_t idx;
        for (idx=0;idx<(sizeof(IntroMenuLines)/sizeof(IntroMenuLines[0]));idx++)
        {
          (menuItem==idx) ? tBuffer[0]='>' : tBuffer[0]='*';
          strcpy_P(&tBuffer[1],(char*)pgm_read_word(&(IntroMenuLines[idx])));
          arduboy.println(tBuffer);
          arduboy.setCursor(2,arduboy.getCursorY());
        }
        if (!gSnow)
        {
          srand(rand());
          gSnow=new Snow(20);
        }
        if (gSnow)
        {
          gSnow->Display(arduboy);
          int16_t tVect=(int8_t)subMenuItem;
          gSnow->Move((tVect<<SUBPIXELBITS)/FPS);
          subMenuItem=0;
        }
        lastMode=-1;
      }
      break;
    case MODE_FullScreenImage:
      if (currentDialog==&FrostyIgloo)
      {
        arduboy.drawBitmap(0,
                            0,
                            &frostysIgloo[2],
                            frostysIgloo[0],
                            frostysIgloo[1],
                            WHITE
                            );
      }
      else if (currentDialog==&FrostyUserDeadSkip)
      {
        arduboy.drawBitmap(0,
                            0,
                            &tombstone3[2],
                            tombstone3[0],
                            tombstone3[1],
                            WHITE
                            );
        arduboy.setCursor(22+((60/2)-((strlen(userName)*CHARWIDTH)/2)),28);
        arduboy.println(userName);        
      }
      if (gSnow)
      {
        gSnow->Display(arduboy);
        int16_t tVect=(int8_t)subMenuItem;
        gSnow->Move((tVect<<SUBPIXELBITS)/FPS);
        subMenuItem=0;
      }
      lastMode=-1;
      break;
    case MODE_Narration:
      DrawPortrait();
      //Plot(0,0,(int8_t*)&dateTextBubble,sizeof(dateTextBubble));
      arduboy.setCursor(2,6);
      DialogText(pgm_read_word(&(currentDialog->responses)) ? (char*)pgm_read_word(&(((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))->response)) : NULL,dialogOfs);
      if (!currentDialog)
      {
        scrollUp=false;scrollDown=false;
      }
      if (scrollUp) Plot(0,0,(int8_t*)&upArrow,sizeof(upArrow));
      #ifdef SDEBUG
      Serial.println("Displaying Narration");
      #endif
      {
        uint8_t lidx=0;
        while ((lidx<6) && linesLen[lidx])
        {
          arduboy.println(lines[lidx]);
          arduboy.setCursor(2,arduboy.getCursorY());
          lidx++;
        }
      }
      if (scrollDown) Plot(0,0,(int8_t*)&downArrow,sizeof(downArrow));
      break;
      break;
    case MODE_DateDialog:
      DrawPortrait();
      Plot(0,0,(int8_t*)&dateTextBubble,sizeof(dateTextBubble));
      arduboy.setCursor(2,6);
      DialogText(currentDialog ? (char*)pgm_read_word(&(currentDialog->dialog)) : NULL,dialogOfs);
      if (!currentDialog)
      {
        scrollUp=false;scrollDown=false;
      }
      if (scrollUp) Plot(0,0,(int8_t*)&upArrow,sizeof(upArrow));
      #ifdef SDEBUG
      Serial.println("Displaying dialog");
      #endif
      {
        uint8_t lidx=0;
        while ((lidx<6) && linesLen[lidx])
        {
          arduboy.println(lines[lidx]);
          arduboy.setCursor(2,arduboy.getCursorY());
          lidx++;
        }
      }
      if (scrollDown) Plot(0,0,(int8_t*)&downArrow,sizeof(downArrow));
      break;
    case MODE_UserResponse:
      DrawPortrait();
      Plot(0,0,(int8_t*)&userTextBubble,sizeof(userTextBubble));
      arduboy.setCursor(2,6);
      ResponseText((uint8_t)pgm_read_byte(&(currentDialog->numResponse)),(ResponseEnt*)pgm_read_word(&(currentDialog->responses)),menuItem);
      if (!pgm_read_word(&(currentDialog->responses)))
      {
        scrollUp=false;scrollDown=false;
      }
      if (scrollUp) Plot(0,0,(int8_t*)&upArrow,sizeof(upArrow));
      #ifdef SDEBUG
      Serial.println("Displaying dialog");
      #endif
      {
        uint8_t lidx=0;
        while ((lidx<6) && linesLen[lidx])
        {
          arduboy.println(lines[lidx]);
          arduboy.setCursor(2,arduboy.getCursorY());
          lidx++;
        }
      }
      if (scrollDown) Plot(0,0,(int8_t*)&downArrow,sizeof(downArrow));
      break;
    case MODE_UserNameQuery:
      DrawPortrait();
      Plot(0,0,(int8_t*)&userTextBubble,sizeof(userTextBubble));
      {
        arduboy.setCursor(2,6);
        uint8_t idx,len=strlen(userName);
        for (idx=0; idx<len; idx++)
        {
            if (menuItem==idx)
            {
                arduboy.setTextBackground(WHITE);
                arduboy.setTextColor(BLACK);
                arduboy.print((char)pgm_read_byte(&validNameChars[textChar]));
            }
            else
            {
                arduboy.setTextBackground(BLACK);
                arduboy.setTextColor(WHITE);
                arduboy.print(userName[idx]);
            }
        }
        if (menuItem==idx)
        {
            arduboy.setTextBackground(WHITE);
            arduboy.setTextColor(BLACK);
            arduboy.print((char)pgm_read_byte(&validNameChars[textChar]));
        }
        // Leave the textcolor set to WHITE
        arduboy.setTextBackground(BLACK);
        arduboy.setTextColor(WHITE);
      }
      break;
    case MODE_GameOver:
      DrawPortrait();
      arduboy.setCursor(2,6);
      arduboy.println(strcpy_P(tBuffer,ReturnToText));
      arduboy.setCursor(2,arduboy.getCursorY());
      arduboy.println(strcpy_P(tBuffer,TitleText));
      arduboy.setCursor(2,arduboy.getCursorY());
      arduboy.print(subMenuItem ? "*" : ">") ;
      arduboy.println(strcpy_P(tBuffer,UserNoText));
      arduboy.setCursor(2,arduboy.getCursorY());
      arduboy.print(subMenuItem==0 ? "*" : ">") ;
      arduboy.println(strcpy_P(tBuffer,UserYesText));
      break;
    case MODE_Chivos:      
      {
        uint8_t idx;
        if (subMenuItem) Plot(0,0,(int8_t*)&upArrow,sizeof(upArrow));
        arduboy.setCursor(2,6);
        for (idx=subMenuItem;
              ((idx<(sizeof(ChivoLines)/sizeof(ChivoLines[0])))
              &&
              (idx-subMenuItem<6)
              );
              idx++
              )
        {
          if (chivos & (1<<idx)) strcpy_P(tBuffer,(char*)pgm_read_word(&(ChivoLines[idx])));
          else
          {
            tBuffer[0]='?';
            tBuffer[1]=0;
          }
          arduboy.println(tBuffer);
          arduboy.setCursor(2,arduboy.getCursorY());
        }        
        if (subMenuItem+6<(sizeof(ChivoLines)/sizeof(ChivoLines[0]))) Plot(0,0,(int8_t*)&downArrow,sizeof(downArrow));
      }
      break;
    case MODE_ResetProgress:
      arduboy.setCursor(2,6);
      arduboy.print(strcpy_P(tBuffer,ResetProgressText));
      arduboy.println('?');
      arduboy.setCursor(2,arduboy.getCursorY());
      (subMenuItem) ? arduboy.print('*') : arduboy.print('>');
      arduboy.println(strcpy_P(tBuffer,UserNoText));
      arduboy.setCursor(2,arduboy.getCursorY());
      (subMenuItem) ? arduboy.print('>') : arduboy.print('*');
      arduboy.println(strcpy_P(tBuffer,UserYesText));
      arduboy.setCursor(2,arduboy.getCursorY());
      break;
    }
    #ifdef SCREENSTREAM
    Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
    #endif
    arduboy.display();
  }
  arduboy.pollButtons();
  if (mode==MODE_EEPROM)
  {
    char tBuffer[22];
    if (EEPROM_MenuInput(tBuffer,menuItem,lastMode))
    {
      mode=MODE_Intro;
      menuItem=0;
      if (useEEPROM)
      {
        ProgressEntry tProgress;
        if (ReadProgress(tProgress)>-1)
        {
          chivos=tProgress.chivos;
          strcpy((char*)&userName,(char*)&tProgress.name);
          if (chivos & (1<<CHIVO_Murderer))
          {
            frostyDead=true;
            frostyKills=3;
          }          
        }
        else
        {
          chivos=0;
          userName[0]=0;
        }
      }
    }
  }
  else
  {
    if (waitRelease)
    {
      if (arduboy.notPressed(LEFT_BUTTON | RIGHT_BUTTON | UP_BUTTON | DOWN_BUTTON | A_BUTTON | B_BUTTON))
      {
        waitRelease=false;
      }
    }
    else if (arduboy.justPressed(LEFT_BUTTON))
    {
      switch (mode)
      {
      case MODE_UserResponse:
        if (strlen_P((char*)pgm_read_word(&(currentDialog->dialog)))==1)
        {
          switch((char)pgm_read_byte(pgm_read_word(&(currentDialog->dialog))))
          {
          case '-':
            #ifdef SDEBUG
            Serial.println("SkipDialog");
            #endif            
            lastMode=MODE_Narration;
            break;
          default:
            mode=MODE_DateDialog;
            break;
          }
        }
        else mode=MODE_DateDialog;
        break;
      case MODE_UserNameQuery:
        userName[menuItem]=pgm_read_byte(&validNameChars[textChar]);
        if (menuItem>0) menuItem--;
        if (userName[menuItem])
        {
          char *cPtr=strchr_P(validNameChars,userName[menuItem]);
          if (cPtr)
          {
            textChar=cPtr-(char*)(&validNameChars);
          }
        }
        lastMode=-1;
        break;
      }
    }
    else if (arduboy.pressed(LEFT_BUTTON))
    {
      switch (mode)
      {
      case MODE_Intro:
        subMenuItem=-1;
        break;
      }        
    }
    else if (arduboy.justPressed(RIGHT_BUTTON))
    {
      switch (mode)
      {
      case MODE_FullScreenImage:
        if (currentDialog==&FrostyIgloo)
        {
          mode=MODE_DateDialog;
        }
        else if (currentDialog==&FrostyUserDeadSkip)
        {
          mode=MODE_GameOver;
        }
        break;
      case MODE_Narration:
        gameOverMode=mode;
        mode=MODE_FullScreenImage;
        subMenuItem=0;
        if (currentDialog==&FrostyUserDeadSkip)
        {
          chivos|=1<<CHIVO_YouDied;
          WriteChivos(chivos,(char*)&userName);
        }
        break;
      case MODE_DateDialog:
        gameOverMode=mode;
        if (((uint8_t)pgm_read_byte(&(currentDialog->numResponse))==1) && pgm_read_word(&(currentDialog->responses)))
        {
          if (strlen_P((char*)pgm_read_word(&(((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))->response)))==1)
          {
            switch((char)pgm_read_byte(pgm_read_word(&(((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))->response))))
            {
            case '?':              
              #ifdef SDEBUG
              Serial.println("NameQuery");
              #endif
              mode=MODE_UserNameQuery;
              memset(&userName,0,sizeof(userName));
              menuItem=0;
              textChar=0;              
              break;
            case '-':
              #ifdef SDEBUG
              Serial.println("SkipResponse");
              #endif
              lastLove=dateLove;
              dateLove+=(int8_t)pgm_read_byte(&(((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))->love));
              SelectDialog();
              lastMode=-1;
              dialogOfs=0;
              menuItem=0;
              break;
            default:
              mode=MODE_UserResponse;
              break;
            }
          }
          else mode=MODE_UserResponse;
        }
        else mode=MODE_UserResponse;
        if (mode==MODE_UserResponse)
        {
          if (!(uint8_t)pgm_read_byte(&(currentDialog->numResponse)))
          {
            //gameOverMode=mode;
            mode=MODE_GameOver;
            subMenuItem=0;
            uint16_t lastChivos=chivos;
            if (currentDialog==&FrostyNoKiss)
            {
              chivos|=1<<CHIVO_UserFriendzone;
            }
            if (currentDialog==&FrostySnubbed)
            {
              chivos|=1<<CHIVO_UserSnub;
            }
            if (currentDialog==&FrostyKiss)
            {
              chivos|=1<<CHIVO_FrostysHeart;
            }
            if (currentDialog==&(FrostyFeelz[0]))
            {
              chivos|=1<<CHIVO_FrostySnub;
            }
            if (currentDialog==&(FrostyFeelz[1]))
            {
              chivos|=1<<CHIVO_FrostyFriendzone;
            }
            if (currentDialog==&FrostyMelted)
            {
              if (!frostyDead)
              {
                frostyDead=true;
                frostyKills++;
              }
              chivos|=1<<CHIVO_KilledFrosty;
            }
            if (chivos!=lastChivos)
            {
              WriteChivos(chivos,(char*)&userName);
            }
          }
        }
        break;
      case MODE_UserNameQuery:
        userName[menuItem]=pgm_read_byte(&validNameChars[textChar]);
        #ifdef SDEBUG
        Serial.print("sizeof(userName)-sizeof(char)=");
        Serial.println(sizeof(userName)-sizeof(char));
        Serial.print("menuItem=");
        Serial.println(menuItem);
        #endif
        if (menuItem<(sizeof(userName)-(sizeof(char)*2))) menuItem++;
        if (userName[menuItem])
        {
          char *cPtr=strchr_P(validNameChars,userName[menuItem]);
          if (cPtr)
          {
            textChar=cPtr-(char*)(&validNameChars);
          }
        }
        lastMode=-1;
        break;
      }
    }
    else if (arduboy.pressed(RIGHT_BUTTON))
    {
      switch (mode)
      {
      case MODE_Intro:
        subMenuItem=1;
        break;
      }        
    }
    else if (arduboy.justPressed(UP_BUTTON))
    {
      switch (mode)
      {
      case MODE_Intro:
        if (menuItem>0) menuItem--;
        lastMode=-1;
        break;
      case MODE_Narration:
      case MODE_DateDialog:
        if (dialogOfs>0) dialogOfs--;
        lastMode=-1;
        break;
      case MODE_UserResponse:
        if (menuItem>0) menuItem--;
        lastMode=-1;
        break;
      case MODE_UserNameQuery:
        if (textChar>0) textChar--; else textChar=(NUMVALIDNAMECHARS-1);
        //Serial.println(textChar);
        lastMode=-1;
        break;
      case MODE_GameOver:
        if (subMenuItem>0) subMenuItem--;
        lastMode=-1;
        break;
      case MODE_Chivos:
        if (subMenuItem) subMenuItem--;
        lastMode=-1;
        break;
      case MODE_ResetProgress:
        if (subMenuItem) subMenuItem--;
        lastMode=-1;
        break;
      }
    }
    else if (arduboy.justPressed(DOWN_BUTTON))
    {
      switch (mode)
      {
      case MODE_Intro:
        if (menuItem<((sizeof(IntroMenuLines)/sizeof(IntroMenuLines[0]))-1)) menuItem++;
        lastMode=-1;
        break;
      case MODE_Narration:
      case MODE_DateDialog:
        if (scrollDown) dialogOfs++;
        lastMode=-1;
        break;
      case MODE_UserResponse:
        if (menuItem<((uint8_t)pgm_read_byte(&(currentDialog->numResponse))-1)) menuItem++;
        lastMode=-1;
        break;
      case MODE_UserNameQuery:
        if (textChar<(NUMVALIDNAMECHARS-1)) textChar++; else textChar=0;
        //Serial.println(textChar);
        lastMode=-1;
        break;
      case MODE_GameOver:
        if (subMenuItem<1) subMenuItem++;
        lastMode=-1;
        break;
      case MODE_Chivos:
        if (subMenuItem+6<(sizeof(ChivoLines)/sizeof(ChivoLines[0]))) subMenuItem++;
        lastMode=-1;
        break;
      case MODE_ResetProgress:
        if (subMenuItem<1) subMenuItem++;
        lastMode=-1;
        break;
      }
    }
    else if (arduboy.justPressed(A_BUTTON))
    {
      switch (mode)
      {
      case MODE_UserNameQuery:
        {
          uint8_t len=strlen(userName);
          if (len)
          {
            //len--;
            if ((len-1)>menuItem)
            {
              memmove(&userName[menuItem],&userName[menuItem+1],sizeof(char)*((len-1)-menuItem));
              if (userName[menuItem])
              {
                char *cPtr=strchr_P(validNameChars,userName[menuItem]);
                if (cPtr)
                {
                  textChar=cPtr-(char*)(&validNameChars);
                }
              }
            }
            if (menuItem && (menuItem>=len))
            {
              userName[menuItem]=0;
              menuItem--;
              if (userName[menuItem])
              {
                char *cPtr=strchr_P(validNameChars,userName[menuItem]);
                if (cPtr)
                {
                  textChar=cPtr-(char*)(&validNameChars);
                }
                userName[menuItem]=0;
              }              
            }
            else
            {
              userName[len-1]=0;
            }
            lastMode=-1;
          }
        }
        break;
      case MODE_Chivos:
        mode=MODE_Intro;
        subMenuItem=0;
        break;
      }
    }
    else if (arduboy.justPressed(B_BUTTON))
    {
      switch (mode)
      {
      case MODE_Intro:
        switch(menuItem)
        {
        case 0:
          dateLove=0;
          lastLove=0;
          if (!frostyDead)
          {
            if (!strlen(userName))
            {
              currentDialog=&FrostyGreeting;
            }
            else
            {
              currentDialog=&FrostyLike;
            }
          }
          else
          {
            switch(frostyKills)
            {
            case 1:
              currentDialog=&SantaRestoreFrosty;
              break;
            case 2:
              currentDialog=&SantaRestoreFrosty2;
              break;
            case 3:
            default:
              {
                uint8_t lastChivos=chivos;
                chivos|=1<<CHIVO_Murderer;
                if (lastChivos!=chivos)
                {
                  WriteChivos(chivos,userName);
                }
              }
              currentDialog=&SantaCoal;
              break;
            }
          }
          mode=MODE_DateDialog;      
          dialogOfs=0;
          menuItem=0;
          break;
        case 1:
          mode=MODE_Chivos;
          subMenuItem=0;
          break;
        case 2:
          mode=MODE_ResetProgress;
          subMenuItem=0;
          break;
        }
        break;
      case MODE_FullScreenImage:
        if (currentDialog==&FrostyIgloo)
        {
          mode=MODE_DateDialog;
        }
        else if (currentDialog==&FrostyUserDeadSkip)
        {
          mode=MODE_GameOver;
        }
        break;
      case MODE_UserResponse:
        lastLove=dateLove;
        dateLove+=(int8_t)pgm_read_byte(&((((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))[menuItem]).love));
        SelectDialog();
        if (strlen_P((char*)pgm_read_word(&(currentDialog->dialog)))==1)
        {
          switch((char)pgm_read_byte(pgm_read_word(&(currentDialog->dialog))))
          {
          case '-':
            #ifdef SDEBUG
            Serial.println("SkipDialog");
            #endif            
            mode=MODE_Narration;
            break;
          default:
            mode=MODE_DateDialog;
            break;
          }
        }
        else mode=MODE_DateDialog;
        dialogOfs=0;
        menuItem=0;
        if (currentDialog==&FrostyRestored)
        {
          frostyDead=false;
        }
        if (currentDialog==&FrostyIgloo)
        {
          mode=MODE_FullScreenImage;
        }
        break;
      case MODE_UserNameQuery:    
        userName[menuItem]=pgm_read_byte(&validNameChars[textChar]);
        mode=MODE_DateDialog;
        currentDialog=(DialogEnt*)pgm_read_word(&(((ResponseEnt*)pgm_read_word(&(currentDialog->responses)))->dialogs));
        dialogOfs=0;
        menuItem=0;
        //lastMode=-1;
        break;
      case MODE_GameOver:
        if (subMenuItem) mode=MODE_Intro;
        else mode=gameOverMode;
        break;
      case MODE_ResetProgress:
        if (subMenuItem)
        {
          chivos=0;
          userName[0]=0;
          frostyDead=false;
          frostyKills=0;
          WriteChivos(chivos,(char*)&userName);
        }
        mode=MODE_Intro;
        subMenuItem=0;
        break;
      }
    }
  }
}

