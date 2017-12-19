#ifndef DIALOG_H
#define DIALOG_H
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
  extern const ResponseEnt UserRestoreFrosty[] PROGMEM;
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

  const ResponseEnt UserFrostyRestoredSkip PROGMEM= {0,1,UserSkipText,(DialogEnt*)&FrostyLike};
  
  const char FrostyNotRestoredText[] PROGMEM = "Well bye then.";
  const DialogEnt FrostyNotRestored PROGMEM ={0,1,FrostyNotRestoredText,NULL};

  const char FrostyRestoredText[] PROGMEM = "Okay then. Here he is.";
  const DialogEnt FrostyRestored PROGMEM ={0,1,FrostyRestoredText,&UserFrostyRestoredSkip};

  const ResponseEnt UserSantaMarriedSkip PROGMEM= {0,1,UserSkipText,(DialogEnt*)&FrostyLike};

  //const char SantaMarriedText[] PROGMEM = "Um. I'm married. Ever hear of Mrs. Claus? Anyway, do you want me to bring him back or not?";
  //const DialogEnt SantaMarried PROGMEM ={0,2,SantaMarriedText,(ResponseEnt*)&UserRestoreFrosty};
 
  //const char UserDateSantaText[] PROGMEM = "I want to date you instead";
  const ResponseEnt UserRestoreFrosty[] PROGMEM=
  {
    {0,1,UserYesText,&FrostyRestored},
    {0,1,UserNoText,&FrostyNotRestored},
    //{0,1,UserDateSantaText,&SantaMarried}
  };

  const char SantaRestoreFrostyText[] PROGMEM = "[N]. Yes, it's Santa Claus. You were expecting Frosty? You killed him, you know? I hope you didn't do that on PURPOSE. Well. Luckily Frosty is made out of Christmas snow and it is still Christmas, so I can bring him back to life.. Do you want me to bring Frosty back now?";
  const DialogEnt SantaRestoreFrosty PROGMEM ={0,2,SantaRestoreFrostyText,(ResponseEnt*)&UserRestoreFrosty};

  const char SantaRestoreFrosty2Text[] PROGMEM = "[N]. You killed Frosty TWICE. I have an awful hard time believing that was an accident the second time. However, I will give you the benefit of the doubt. Do you want me to bring Frosty back again?";
  const DialogEnt SantaRestoreFrosty2 PROGMEM ={0,2,SantaRestoreFrosty2Text,(ResponseEnt*)&UserRestoreFrosty};

  const char SantaCoalText[] PROGMEM = "[N]. You have killed Frosty THREE times. I am sure that at this point that you are not doing it by accident. And for what? Are you expecting some kind of achievement? I'm sorry, but I'm not bringing him back for you this time. Don't expect anything but coal for Christmas from now on.";
  const DialogEnt SantaCoal PROGMEM ={0,0,SantaCoalText,NULL};
}





//6x8
//10 char, 6 lines
//uint8_t userNameLen=0;
char userName[11]="";
char lines[6][11];
uint8_t linesLen[6]={0,0,0,0,0,0},menuItem=0, textChar=0;
bool scrollDown=false,scrollUp=false;


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
    memcpy(tWord,userName,strlen(userName));
    wordLen=strlen(userName);
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

#endif //DIALOG_H
