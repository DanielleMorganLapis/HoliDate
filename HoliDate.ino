#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "GameMath.h"
#include "assets.h"
#include "Music.h"



Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

//#define SDEBUG
//#define SCREENSTREAM


typedef enum
{
  MODE_Intro = 0,
  MODE_Narration,
  MODE_DateDialog,
  MODE_UserResponse,
  MODE_UserNameQuery,
  MODE_GameOver
} GameModes;


uint8_t lastMode=0xFF,mode=MODE_Intro,dialogOfs=0,subMenuItem=0,gameOverMode=MODE_Intro;
int8_t dateLove=0,lastLove=0;
bool waitRelease=true;

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
 

struct Response;
typedef struct Response ResponseEnt;


typedef struct
{
  int8_t love;
  uint8_t numResponse;
  const char *dialog;  
  ResponseEnt *responses;
} DialogEnt;


struct Response
{
  int8_t love;
  uint8_t numDialog;
  char *response;  
  DialogEnt *dialogs;
};


namespace {
  extern const DialogEnt FrostyGreeting PROGMEM;
  extern const DialogEnt FrostyComfy PROGMEM;
  const char UserYesText[] PROGMEM = "Yes";
  const char UserNoText[] PROGMEM = "No";
  const char UserSkipText[] PROGMEM = "-";
  const char UserColdText[] PROGMEM = "It's a little cold";

  const char UserHypothermiaText[] PROGMEM = "It was too cold for you inside Frosty's igloo and you passed away of hypothermia.";
  const ResponseEnt UserHypothermia PROGMEM= {0,0,UserHypothermiaText,NULL};

  const DialogEnt FrostyUserDeadSkip PROGMEM = {0,1,UserSkipText,(ResponseEnt*)&UserHypothermia};

  const ResponseEnt UserColdVeryConcerned[] PROGMEM =
  {
    {-3,1,UserYesText,&FrostyUserDeadSkip},
    {0,1,UserColdText,&FrostyComfy}
  };
  
  const char FrostyColdVeryConceredText[] PROGMEM = "Okay, [N]. You shiver and your teeth chatter. Are really sure you are sure?";
  const DialogEnt FrostyColdVeryConcered PROGMEM = {0,2,FrostyColdVeryConceredText,(ResponseEnt*)&UserColdVeryConcerned};

  const char FrostyNoKissText[] PROGMEM = "Oh! I'm so sorry. I didn't mean to be pushy. Um, I'd still like to be best friends though! I hope that's OK! Thank you for spending Christmas with me!";
  const DialogEnt FrostyNoKiss PROGMEM = {0,0,FrostyNoKissText,(ResponseEnt*)NULL};

   const char FrostyKissText[] PROGMEM = "*kiss* Your skin is warm, [N]. Warm is difficult for me, but you're not that bad. You kind of like the cold like me, even though you are human, don't you? I don't know how we can make things work long term, [N]. Me, being a snowman and all, and you human. But I think we really shared some special moments together. Thank you for being my holi-date this Christmas. I already know this is a day I will remember forever! <Merry Christmas, Good ending: You won the heart of Frosty>";
  const DialogEnt FrostyKiss PROGMEM = {0,0,FrostyKissText,(ResponseEnt*)NULL};

  const ResponseEnt UserComfy[] PROGMEM =
  {
    {0,1,UserYesText,&FrostyNoKiss},
    {+2,1,UserNoText,&FrostyKiss}
  };

  const char FrostyComfyText[] PROGMEM = "Let me give you this heavy coat. I got it just in case I have a human visitor. I know the cold is a little hard for you. But I like you a lot, and I would like for us to get closer. Um. I'm not sure how to say this. I know I'm just a snowman, and you are a human. But would you mind if I gave you a kiss?";
  const DialogEnt FrostyComfy PROGMEM = {0,2,FrostyComfyText,(ResponseEnt*)&UserComfy};

  const ResponseEnt UserColdConcern[] PROGMEM =
  {
    {0,1,UserYesText,&FrostyColdVeryConcered},
    {0,1,UserColdText,&FrostyComfy}
  };

  const char FrostyColdConcernText[] PROGMEM = "Are you sure [N]? I know it might be a little cold for humans. You are looking a little blue.";
  const DialogEnt FrostyColdConcern PROGMEM = {0,2,FrostyColdConcernText,(ResponseEnt*)&UserColdConcern};
  
  const char UserNotColdText[] PROGMEM = "Not at all";

  const ResponseEnt UserIgloo[] PROGMEM =
  {
    {0,1,UserNotColdText,&FrostyColdConcern},
    {0,1,UserColdText,&FrostyComfy}
  };

  const char FrostyIglooText[] PROGMEM = "Well, this is my place. Come on inside. I find it comfy here. I hope it is not too cold for you.";
  const DialogEnt FrostyIgloo PROGMEM = {0,2,FrostyIglooText,(ResponseEnt*)&UserIgloo};
  const char FrostySnubbedText[] PROGMEM = "Oh, okay. Well, I look forward to seeing you again [N].";
  const DialogEnt FrostySnubbed PROGMEM = {0,0,FrostySnubbedText,(ResponseEnt*)NULL};

  const ResponseEnt UserIglooQuery[] PROGMEM =
  {
    {0,1,UserYesText,&FrostyIgloo},
    {0,1,UserNoText,&FrostySnubbed}
  };

  const char FrostyWashHairText[] PROGMEM = "It was nice meeting you, [N]. But I think, uh, I need to go wash my hair now. See you later. <Game over>";
  const char FrostyFriendZoneText[] PROGMEM = "I am so happy that we have become such close friends [N]. Let us hug closely. I hope that my icy body is not that cold for you. Let's hang out again sometime. <Game over>";
  const char FrostyLovesYouText[] PROGMEM = "I don't know how to say this [N]. I have not been around that long, but I think you are the most special person I ever met. Would you like to come back to my igloo?";

  const DialogEnt FrostyFeelz[] PROGMEM =
  {
    {0,0,FrostyWashHairText,NULL},
    {2,0,FrostyFriendZoneText,NULL},
    {4,2,FrostyLovesYouText,(ResponseEnt*)&UserIglooQuery}
  };

  const ResponseEnt UserFoodSkip PROGMEM= {0,3,UserSkipText,(DialogEnt*)&FrostyFeelz};

  const char FrostyMeltedText[] PROGMEM = "<Frosty melts. Game over.>";
  const DialogEnt FrostyMelted PROGMEM = {0,0,FrostyMeltedText,(ResponseEnt*)NULL};
  
  const ResponseEnt UserStayHibachiSkip PROGMEM= {-2,1,UserSkipText,&FrostyMelted};

  const char FrostyLeaveHibachiText[] PROGMEM = "Okay, whew, thanks.";
  const DialogEnt FrostyLeaveHibachi PROGMEM = {0,1,FrostyLeaveHibachiText,(ResponseEnt*)&UserFoodSkip};
  const char FrostyStayHibachiText[] PROGMEM = "Please, [N]. Why did you take me here? I'm melting...";
  const DialogEnt FrostyStayHibachi PROGMEM = {0,1,FrostyStayHibachiText,(ResponseEnt*)&UserStayHibachiSkip};

  const ResponseEnt UserHibachi[] PROGMEM =
  {
    {-1,1,UserYesText,&FrostyLeaveHibachi},
    {-1,1,UserNoText,&FrostyStayHibachi}
  };

  const char FrostyPizzaText[] PROGMEM = "Um. I don't think I like this Pizza very much. This is too hot. I think once it cools off it might be OK though.";
  const DialogEnt FrostyPizza PROGMEM = {0,1,FrostyPizzaText,(ResponseEnt*)&UserFoodSkip};
  const char FrostyIceCreamText[] PROGMEM = "Yum. This is pretty good [N]! I can see why humans like to eat things.";
  const DialogEnt FrostyIceCream PROGMEM = {0,1,FrostyIceCreamText,(ResponseEnt*)&UserFoodSkip};
  const char FrostyHibachiText[] PROGMEM = "*sweats* Hey [N], I don't like it in here. There are fires on these tables. Can we leave?";
  const DialogEnt FrostyHibachi PROGMEM = {0,2,FrostyHibachiText,(ResponseEnt*)&UserHibachi};

  const char UserPizzaText[] PROGMEM = "Pizza";
  const char UserIceCreamText[] PROGMEM = "Ice cream";
  const char UserHibachiText[] PROGMEM = "Hibachi";
  const ResponseEnt UserFood[] PROGMEM =
  {
    {0,1,UserPizzaText,&FrostyPizza},
    {+1,1,UserIceCreamText,&FrostyIceCream},
    {-1,1,UserHibachiText,&FrostyHibachi}
  };

  const char FrostyFoodText[] PROGMEM = "I hear humans need to eat food. I don't think I need to eat, but I want to try it anyway. Can you take me somewhere?";
  const DialogEnt FrostyFood PROGMEM = {0,3,FrostyFoodText,(ResponseEnt*)&UserFood};

  const ResponseEnt UserDanceSkip PROGMEM= {0,1,UserSkipText,&FrostyFood};

  const char FrostyTwistText[] PROGMEM = "Well, I never heard that song before. But that was pretty groovy.";
  const DialogEnt FrostyTwist PROGMEM = {0,1,FrostyTwistText,(ResponseEnt*)&UserDanceSkip};
  const char FrostyWinterWonderText[] PROGMEM = "I really like this song! Except the part about the fire.";
  const DialogEnt FrostyWinterWonder PROGMEM = {0,1,FrostyWinterWonderText,(ResponseEnt*)&UserDanceSkip};
  const char FrostyLMFText[] PROGMEM = "Um sorry but I really don't like this song very much. There is a lot of talk about fire. I don't like fire...";
  const DialogEnt FrostyLMF PROGMEM = {0,1,FrostyLMFText,(ResponseEnt*)&UserDanceSkip};

  const char UserTwistText[] PROGMEM = "The Twist";
  const char UserWinterWonderText[] PROGMEM = "Winter Wonderland";
  const char UserLMFText[] PROGMEM = "Light My Fire";
  const ResponseEnt UserDance[] PROGMEM =
  {
    {0,1,UserTwistText,&FrostyTwist},
    {+1,1,UserWinterWonderText,&FrostyWinterWonder},
    {-1,1,UserLMFText,&FrostyLMF}
  };

  const char FrostyDanceText[] PROGMEM = "What do you say we dance to a song? You can pick the song.";
  const DialogEnt FrostyDance PROGMEM = {0,3,FrostyDanceText,(ResponseEnt*)&UserDance};
 
  const ResponseEnt UserHolidaySkip PROGMEM= {0,1,UserSkipText,&FrostyDance};

  const char FrostyXmasText[] PROGMEM = "Oh! That's my favorite holiday too!";
  const DialogEnt FrostyXmas PROGMEM = {0,1,FrostyXmasText,&UserHolidaySkip};
  const char FrostyBdayText[] PROGMEM = "Oh. My birthday is on Christmas!";
  const DialogEnt FrostyBday PROGMEM = {0,1,FrostyBdayText,&UserHolidaySkip};
  const char Frosty4thText[] PROGMEM = "Oh. July sounds like it would be warm...";
  const DialogEnt Frosty4th PROGMEM = {0,1,Frosty4thText,&UserHolidaySkip};
  
  const char UserXmasText[] PROGMEM = "Christmas";
  const char UserBdayText[] PROGMEM = "My birthday";
  const char User4thText[] PROGMEM = "4th of July";
  const ResponseEnt UserHoliday[] PROGMEM =
  {
    {+1,1,UserXmasText,&FrostyXmas},
    {0,1,UserBdayText,&FrostyBday},
    {-1,1,User4thText,&Frosty4th}
  };
  const char FrostyHolidayText[] PROGMEM = "It's a beautiful Christmas day today. What's your favorite holiday?";
  const DialogEnt FrostyHoliday PROGMEM = {0,3,FrostyHolidayText,(ResponseEnt*)&UserHoliday};

  const ResponseEnt UserLikeSkip PROGMEM= {0,1,UserSkipText,&FrostyHoliday};
  const char FrostyBeachText[] PROGMEM = "Beach? Oh, I hope you like cold beaches... some of them are a little warm for me.";
  const DialogEnt FrostyBeach PROGMEM ={0,1,FrostyBeachText,&UserLikeSkip};
  const char FrostySkateText[] PROGMEM = "You like ice skating? That sounds like fun! Nice and frosty!";
  const DialogEnt FrostyIceSkate PROGMEM ={0,1,FrostySkateText,&UserLikeSkip};
  const char FrostyGameText[] PROGMEM = "Video games?  Never heard of those before! You'll have to show me sometime!";
  const DialogEnt FrostyVideoGame PROGMEM ={0,1,FrostyGameText,&UserLikeSkip};
  
  const char UserLikeBeachText[] PROGMEM = "Long walks on the beach";
  const char UserLikeSkateText[] PROGMEM = "Ice skating";
  const char UserLikeGameText[] PROGMEM = "Video games";
  
  const ResponseEnt UserLike[] PROGMEM=
  {
    {-1,1,UserLikeBeachText,&FrostyBeach},
    {+1,1,UserLikeSkateText,&FrostyIceSkate},
    {0,1,UserLikeGameText,&FrostyVideoGame},
  };
  
  const char FrostyLikeText[] PROGMEM ="I like to sing, dance and laugh. What do you like to do [N]?";
  const DialogEnt FrostyLike PROGMEM ={0,3,FrostyLikeText,(ResponseEnt*)&UserLike};
  
  const ResponseEnt UserNameCheck[] PROGMEM=
  {
    {0,1,UserYesText,&FrostyLike},
    {0,1,UserNoText,&FrostyGreeting}
  };
  const char DateNameCheckText[] PROGMEM = "[N], is that correct?";
  const DialogEnt DateNameCheck PROGMEM ={0,2,DateNameCheckText,(ResponseEnt*)&UserNameCheck};
  const char UserGreetingText[] PROGMEM = "?";
  const ResponseEnt UserGreeting PROGMEM ={0,1,UserGreetingText,&DateNameCheck};
  const char FrostyGreetingText[] PROGMEM = "Hi, I'm Frosty.  What's your name?";
  const DialogEnt FrostyGreeting PROGMEM ={0,1,FrostyGreetingText,&UserGreeting};
}


const char StartText[] PROGMEM = "Start!";
const char ReturnToText[] PROGMEM = "Return to";
const char TitleText[] PROGMEM = "Title?";


//6x8
//10 char, 6 lines
//uint8_t userNameLen=0;
char username[11]="";
char lines[6][11];
uint8_t linesLen[6]={0,0,0,0,0,0},menuItem=0, textChar=0;
bool scrollDown=false,scrollUp=false;
DialogEnt *currentDialog=NULL;

void AddWord(char *tWord,uint8_t &wordLen,uint8_t &lidx,uint8_t &numLines,uint8_t lineOfs,bool punctComing)
{
  if (!tWord) return;
  #ifdef SDEBUG
  Serial.println("Entering AddWord");
  #endif
  tWord[wordLen]=0;
  #ifdef SDEBUG
  Serial.print("tWord=");
  Serial.println(tWord);
  Serial.print("wordLen=");
  Serial.println(wordLen);
  #endif
  // Check for name formatter
  if ((wordLen==3) && (!strcmp(tWord,"[N]")))
  {
    memcpy(tWord,username,strlen(username));
    wordLen=strlen(username);
    tWord[wordLen]=0;
  }
  while(wordLen && (lidx<6))
  {
    #ifdef SDEBUG
    Serial.print("wordLen=");
    Serial.println(wordLen);
    #endif
    if (wordLen<=10)
    {
      #ifdef SDEBUG
      Serial.print("linesLen[");
      Serial.print(lidx);
      Serial.print("]=");
      Serial.println(linesLen[lidx]);
      #endif
      if (wordLen<=(10-linesLen[lidx]))
      {
        memcpy(&lines[lidx][linesLen[lidx]],tWord,wordLen);
        linesLen[lidx]+=wordLen;
        lines[lidx][linesLen[lidx]]=0;
        #ifdef SDEBUG
        Serial.print("lines[");
        Serial.print(lidx);
        Serial.print("]=");
        Serial.println(lines[lidx]);
        #endif
        wordLen=0;
        if (linesLen[lidx]<10)
        {
          if (!punctComing)
          {
            lines[lidx][linesLen[lidx]]=' ';
            linesLen[lidx]++;
            lines[lidx][linesLen[lidx]]=0;
          }
        }
        //break;
      }
      else
      {
        #ifdef SDEBUG
        Serial.println("Newline");
        Serial.print("numLines=");
        Serial.println(numLines);
        Serial.print("lidx=");
        Serial.println(lidx);
        #endif
        numLines++;
        if (numLines>lineOfs)
        {
          lines[lidx][linesLen[lidx]]=0;
          lidx++;
        }
        else
        {
          // We discard lines below the line ofs
          linesLen[lidx]=0;
        }
        #ifdef SDEBUG
        Serial.print("numLines=");
        Serial.println(numLines);
        Serial.print("lidx=");
        Serial.println(lidx);
        #endif
      }
    }
    else
    {
      if ((10-linesLen[lidx])>=2)
      {
        #ifdef SDEBUG
        Serial.println("Wordbreak");
        #endif
        memcpy(&lines[lidx][linesLen[lidx]],tWord,(10-linesLen[lidx])-1);
        memmove(tWord,&tWord[(10-linesLen[lidx])-1],wordLen-((10-linesLen[lidx])-1));
        wordLen-=(10-linesLen[lidx])-1;
        linesLen[lidx]+=((10-linesLen[lidx])-1);
        lines[lidx][linesLen[lidx]]='-';
        linesLen[lidx]++;
        lines[lidx][linesLen[lidx]]=0;
        #ifdef SDEBUG
        Serial.print("lines[");
        Serial.print(lidx);
        Serial.print("]=");
        Serial.println(lines[lidx]);
        #endif
      }
      else
      {
        #ifdef SDEBUG
        Serial.println("Newline");
        #endif
        lines[lidx][linesLen[lidx]]=0;
        lidx++;
      }
    }
  }
  #ifdef SDEBUG
  Serial.print("wordLen=");
  Serial.println(wordLen);
  Serial.println("Exiting AddWord");
  #endif
}

void DialogText(char *text,uint8_t lineOfs)
{
  // Clearing line buffer lengths
  memset(linesLen,0,sizeof(linesLen));
  if (!text) return;
  #ifdef SDEBUG
  Serial.println("Entering DialogText");
  Serial.print("lineOfs=");
  Serial.println(lineOfs);
  #endif
  uint8_t len=0,lidx=0,numLines=0,wordLen=0;
  unsigned int idx=0;
  char tWord[25];
  bool punctComing=false;
  
  if (lineOfs) scrollUp=true; else scrollUp=false;
  #ifdef SDEBUG
  Serial.print("text[");
  Serial.print(idx);
  Serial.print("]=");
  Serial.println((char)pgm_read_byte(&text[idx]));
  #endif
  while ((pgm_read_byte(&text[idx])!=0) && (lidx<6))
  {
    switch ((char)pgm_read_byte(&text[idx]))
    {
    case '?':
    case '.':
    case '!':
    case ',':
      if (!wordLen)
      {
        // Since tWord is empty we can treat the punctionation as a word
        tWord[wordLen]=pgm_read_byte(&text[idx]);
        wordLen++;
        switch (pgm_read_byte(&text[idx+1]))
        {
          case '?':
          case '.':
          case '!':
          case ',':
            punctComing=true;
            break;
        }
      }
      // Rewinding so we can treat the punctionation as a word on the next pass.
      else
      {
        idx--;
        punctComing=true;
      }
      //Falling through to space processing
    case ' ':
      AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
      punctComing=false;
      break;
    default:
      tWord[wordLen]=pgm_read_byte(&text[idx]);
      wordLen++;
      break;
    }
    idx++;
  }
  if ((!pgm_read_byte(&text[idx])) && wordLen)
  {
    AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
  }
  if (pgm_read_byte(&text[idx]) || wordLen) scrollDown=true; else scrollDown=false;
  #ifdef SDEBUG
  Serial.println("Exiting DialogText");
  #endif
}


void ResponseText(uint8_t numResponse,ResponseEnt *responses,uint8_t selectedResponse)
{
  // Clearing line buffer lengths
  memset(linesLen,0,sizeof(linesLen));
  if (!numResponse || !responses) return;
  #ifdef SDEBUG
  Serial.println("Entering ResponseText");
  #endif
  unsigned int idx;
  uint8_t len=0,jdx,kdx,lidx=0,numLines=0,wordLen,lineOfs=0;
  char *text;
  char tWord[25];
  bool punctComing=false;
  for (jdx=0;jdx<numResponse;jdx++)
  {
    idx=0;
    wordLen=0;
    text=(char*)pgm_read_word(&(responses[jdx].response));
    #ifdef SDEBUG
    Serial.print("text[");
    Serial.print(jdx);
    Serial.print("]=");
    Serial.println((char)pgm_read_byte(&text[jdx]));
    #endif

    (jdx==selectedResponse) ? tWord[0]='>' : tWord[0]='*';
    wordLen++;
    tWord[wordLen]=0;
    AddWord(tWord,wordLen,lidx,numLines,lineOfs,true);
    
    while ((pgm_read_byte(&text[idx])!=0) && (lidx<6))
    {
      switch ((char)pgm_read_byte(&text[idx]))
      {
      case '?':
      case '.':
      case '!':
      case ',':
        if (!wordLen)
        {
          // Since tWord is empty we can treat the punctionation as a word
          tWord[wordLen]=pgm_read_byte(&text[idx]);
          wordLen++;
        }
        // Rewinding so we can treat the punctionation as a word on the next pass.
        else
        {
          idx--;
          punctComing=true;
        }
        //Falling through to space processing
      case ' ':
        AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
        if (!wordLen) punctComing=false;
        break;
      default:
        tWord[wordLen]=pgm_read_byte(&text[idx]);
        wordLen++;
        break;
      }
      idx++;
      if (jdx<=selectedResponse)
      {
        #ifdef SDEBUG
        Serial.print("lidx=");
        Serial.println(lidx);
        #endif
        if ((lidx>5)
            /*||
            ((lidx==5)
              &&
              pgm_read_byte(&text[idx])
              )*/
              )
        {
          while (wordLen)
          {
            // Scrolling up one line
            lineOfs++;
            #ifdef SDEBUG
            Serial.println("Scrolling");
            Serial.print("lineOfs=");
            Serial.println(lineOfs);
            #endif
            for (kdx=0;kdx<5;kdx++)
            {
              memcpy(lines[kdx],lines[kdx+1],linesLen[kdx+1]+1);
              linesLen[kdx]=linesLen[kdx+1];
            }
            linesLen[5]=0;
            lidx=5;
            AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
            if (!wordLen) punctComing=false;
          }
        }
      }
    }
    if ((!pgm_read_byte(&text[idx])) && wordLen)
    {
      AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
      if (jdx<=selectedResponse)
      {
        if ((lidx>5)
            /*||
            ((lidx==5)
              &&
              pgm_read_byte(&text[idx])
              )*/
              )
        {
          while (wordLen)
          {
            // Scrolling up one line
            lineOfs++;
            #ifdef SDEBUG
            Serial.println("Scrolling");
            Serial.print("lineOfs=");
            Serial.println(lineOfs);
            #endif
            for (kdx=0;kdx<5;kdx++)
            {
              memcpy(lines[kdx],lines[kdx+1],linesLen[kdx+1]+1);
              linesLen[kdx]=linesLen[kdx+1];
            }
            linesLen[5]=0;
            lidx=5;
            AddWord(tWord,wordLen,lidx,numLines,lineOfs,punctComing);
            if (!wordLen) punctComing=false;
          }
        }
      }
    }
    if (lidx<6)
    {
      if (linesLen[lidx]) lidx++;
    }   
  }
  if (lineOfs) scrollUp=true; else scrollUp=false;
  if (pgm_read_byte(&text[idx]) || wordLen) scrollDown=true; else scrollDown=false;
  #ifdef SDEBUG
  Serial.print("lineOfs=");
  Serial.println(lineOfs);
  Serial.println("Exiting ResponseText");
  #endif
}


// Text bubble for the date
const int8_t dateTextBubble[] PROGMEM =
{
  2,
  0,
  60,
  0,
  62,
  2,
  62,
  37,
  69,
  44,
  62,
  51,
  62,
  58,
  60,
  60,
  2,
  60,
  0,
  58,
  0,
  2,
  2,
  0
};


// Text bubble for the user
const int8_t userTextBubble[] PROGMEM =
{
  2,
  0,
  60,
  0,
  62,
  2,
  62,
  58,
  60,
  60,
  55,
  60,
  49,
  63,
  -128,
  50,
  63,
  43,
  60,
  2,
  60,
  0,
  58,
  0,
  2,
  2,
  0
};

const int8_t upArrow[] PROGMEM = 
{
  31,2,
  31,2,
  -128,
  30,3,
  32,3,
  -128,
  29,4,
  33,4
};

const int8_t downArrow[] PROGMEM = 
{
  29,56,
  33,56,
  -128,
  30,57,
  32,57,
  -128,
  31,58,
  31,58
};

void Plot(int8_t tX, int8_t tY, const int8_t *points, uint8_t numPoints)
{
  //Serial.println("Entering Plot");
  uint8_t idx;
  for (idx = 0; idx < (numPoints - 2); idx += 2)
  {
    /*Serial.print("idx=");Serial.print(idx);
    Serial.print(", x1=");Serial.print(pgm_read_byte(&points[idx]));
    Serial.print(", y1=");Serial.print(pgm_read_byte(&points[idx+1]));
    Serial.print(", x2=");Serial.print(pgm_read_byte(&points[idx+2]));
    Serial.print(", y2=");Serial.println(pgm_read_byte(&points[idx+3]));*/
    if ((int8_t)pgm_read_byte(&points[idx + 2]) != (int8_t)-128)
    {
      arduboy.drawLine( tX + pgm_read_byte(&points[idx]),
                        tY + pgm_read_byte(&points[idx + 1]),
                        tX + pgm_read_byte(&points[idx + 2]),
                        tY + pgm_read_byte(&points[idx + 3])
                      );
    }
    else idx++;//Skipping past line reset
  }
  //Serial.println("Exiting Plot");
}


void DrawPortrait()
{
  int8_t tLove=dateLove-lastLove;
  const unsigned char *tPortrait;
  #ifdef SDEBUG
  Serial.print("tLove=");
  Serial.println(tLove);
  #endif
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
  sound.tones(frosty);
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
    char tBuffer[128];
    arduboy.clear();
    switch (mode)
    {
    case MODE_Intro:
      {
        int x=HolidateLogo[0],y=HolidateLogo[1];
        x=(WIDTH/2)-(x/2);
        y=(HEIGHT/2)-(y/2);
        arduboy.drawBitmap(x,
                            y,
                            &HolidateLogo[2],
                            HolidateLogo[0],
                            HolidateLogo[1],
                            WHITE
                            );
        x=(WIDTH/2)-((strlen_P(StartText)*6)/2);
        arduboy.setCursor(x,50);
        arduboy.println(strcpy_P(tBuffer,StartText));
      }
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
        uint8_t idx,len=strlen(username);
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
                arduboy.print(username[idx]);
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
    }
    #ifdef SCREENSTREAM
    Serial.write(arduboy.getBuffer(), 128 * 64 / 8);
    #endif
    arduboy.display();
  }
  arduboy.pollButtons();
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
      username[menuItem]=pgm_read_byte(&validNameChars[textChar]);
      if (menuItem>0) menuItem--;
      if (username[menuItem])
      {
        char *cPtr=strchr_P(validNameChars,username[menuItem]);
        if (cPtr)
        {
          textChar=cPtr-(char*)(&validNameChars);
        }
      }
      lastMode=-1;
      break;
    }
  }
  else if (arduboy.justPressed(RIGHT_BUTTON))
  {
    switch (mode)
    {
    case MODE_Narration:
      gameOverMode=mode;
      mode=MODE_GameOver;
      subMenuItem=0;
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
            memset(&username,0,sizeof(username));
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
        }
      }
      break;
    case MODE_UserNameQuery:
      username[menuItem]=pgm_read_byte(&validNameChars[textChar]);
      #ifdef SDEBUG
      Serial.print("sizeof(username)-sizeof(char)=");
      Serial.println(sizeof(username)-sizeof(char));
      Serial.print("menuItem=");
      Serial.println(menuItem);
      #endif
      if (menuItem<(sizeof(username)-(sizeof(char)*2))) menuItem++;
      if (username[menuItem])
      {
        char *cPtr=strchr_P(validNameChars,username[menuItem]);
        if (cPtr)
        {
          textChar=cPtr-(char*)(&validNameChars);
        }
      }
      lastMode=-1;
      break;
    }
  }
  else if (arduboy.justPressed(UP_BUTTON))
  {
    switch (mode)
    {
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
    }
  }
  else if (arduboy.justPressed(DOWN_BUTTON))
  {
    switch (mode)
    {
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
    }
  }
  else if (arduboy.justPressed(A_BUTTON))
  {
    switch (mode)
    {
    case MODE_UserNameQuery:
      {
        uint8_t len=strlen(username);
        len--;
        if (len>menuItem)
        {
          memmove(&username[menuItem],&username[menuItem+1],sizeof(char)*(len-menuItem));
        }
        username[len]=username[len+1]=0;
        if (menuItem>=len)
        {
          menuItem=len;
        }
        lastMode=-1;
      }
      break;
    }
  }
  else if (arduboy.justPressed(B_BUTTON))
  {
    switch (mode)
    {
    case MODE_Intro:
      dateLove=0;
      lastLove=0;
      currentDialog=&FrostyGreeting;
      mode=MODE_DateDialog;      
      dialogOfs=0;
      menuItem=0;      
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
      break;
    case MODE_UserNameQuery:    
      username[menuItem]=pgm_read_byte(&validNameChars[textChar]);
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
    }
  }
}

