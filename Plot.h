#ifndef PLOT_H
#define PLOT_H

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
#endif //PLOT_H