// MSGEQ7
#define msgeq7_analog 0
#define msgeq7_reset 43
#define msgeq7_strobe 42

// ShiftBrite
#define clockpin   13 // CI
#define enablepin  10 // EI
#define latchpin    9 // LI
#define datapin    11 // DI

// Defines for direct port access
#define CLKPORT PORTB
#define ENAPORT PORTB
#define LATPORT PORTH
#define DATPORT PORTB
#define CLKPIN  7
#define ENAPIN  4
#define LATPIN  6
#define DATPIN  5

// LiquidCrystal
#include <LiquidCrystal.h>
LiquidCrystal lcd(24, 25, 26, 27, 28, 29);

// Variables for communication
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

// LED array
#define NumLEDs 32
int LEDChannels[NumLEDs][3] = {0};
int rows = 8;
int cols = 4;

int rgb[3];
int effect = 5;
int eposition = 0;
int edirection = 0;
int edelay = 25;
float saturation = 1.00;
float color = 0.00;
int buttonstate = 0;
bool fixrow = false;



/**
 * Setup function
**/
void setup()
{
  // setup shiftbrites
  pinMode(datapin, OUTPUT);
  pinMode(latchpin, OUTPUT);
  pinMode(enablepin, OUTPUT);
  pinMode(clockpin, OUTPUT);
  digitalWrite(latchpin, LOW);
  digitalWrite(enablepin, LOW);

  // setup msgeq7 chip
  pinMode(msgeq7_analog, INPUT);
  pinMode(msgeq7_strobe, OUTPUT);
  pinMode(msgeq7_reset, OUTPUT);
  analogReference(DEFAULT);
  digitalWrite(msgeq7_reset, LOW);
  digitalWrite(msgeq7_strobe, HIGH);

  // setup crystal lcd screen
  lcd.begin(16, 2);
  lcd.print("project::led-tbl");

  // load equalizer characters
  lcd_equalizer();
}



/**
 * Send shiftbrite packet
**/
void SB_SendPacket()
{
  if (SB_CommandMode == B01)
  {
    SB_RedCommand = 127;
    SB_GreenCommand = 110;
    SB_BlueCommand = 110;
  }

  SB_CommandPacket = SB_CommandMode & B11;
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
  SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

  for (int j = 0; j < 32; j++)
  {
    if ((SB_CommandPacket >> (31 - j)) & 1)
      DATPORT |= (1 << DATPIN);
    else
      DATPORT &= ~(1 << DATPIN);
    CLKPORT |= (1 << CLKPIN);
    CLKPORT &= ~(1 << CLKPIN);
  }
}



/**
 * Send shiftbrite latch
**/
void SB_Latch()
{
  delayMicroseconds(1);
  LATPORT |= (1 << LATPIN);
  ENAPORT |= (1 << ENAPIN);
  delayMicroseconds(1);
  ENAPORT &= ~(1 << ENAPIN);
  LATPORT &= ~(1 << LATPIN);
}



/**
 * Write LED array
**/
void WriteLEDArray()
{
  // set command mode
  SB_CommandMode = B00;

  // loop through leds
  for (int i = NumLEDs - 1; i >= 0; i--)
  {
    // find row
    int row = i / cols;

    // fix led number if row mod 2
    int y = fixrow == true ? (row % 2 == 1 ? ((row * cols) + - (i - (row * cols) - 3)) : i) : i;

    // set led values
    SB_RedCommand = LEDChannels[y][0];
    SB_GreenCommand = LEDChannels[y][1];
    SB_BlueCommand = LEDChannels[y][2];

    // send packet
    SB_SendPacket();
  }

  // send latch
  SB_Latch();

  // set command mode
  SB_CommandMode = B01;

  // loop through leds
  for (int z = 0; z < NumLEDs; z++)
    SB_SendPacket();

  // send one last latch
  SB_Latch();
}



/**
 * Loop function
**/
void loop()
{
  // read button value
  int button = digitalRead(50);

  // if button was pressed down
  if (button == 1 && buttonstate == 0)
    buttonstate = 1;

  // if button was released
  if (button == 0 && buttonstate == 1)
  {
    // iterate effect
    effect++;

    // reset button state
    buttonstate = 0;

    // set effect position to zero
    eposition = 0;

    // set effect direction to forward
    edirection = 0;

    // set color to red
    color = 0.00;

    // disable fix rows
    fixrow = false;

    // set effect to zero if greater than effect count
    if (effect == 12)
      effect = 0;
  }

  // select effect
  switch (effect)
  {
    case 1: effect_crossfade(); break;
    case 2: effect_gradient(); break;
    case 3: effect_strobe(); break;
    case 4: effect_sparkle(); break;
    case 5: effect_marquee(); break;
    case 6: effect_police(); break;
    case 7: effect_spectrum(); break;
    case 8: effect_basspulse(); break;
    case 9: effect_plasma(); break;
    case 10: effect_sinwave(); break;
    case 11: effect_pulse(); break;
    default: effect_test();
  }

  // write leds to shiftbrites
  WriteLEDArray();

  // set effect speed
  delay(edelay);

  // iterate through color
  color += 0.001;

  // reset color if through
  if (color > 1.00)
    color = 0.00;
}



/**
 * Test effect
 * (tests each led with current color)
 * @author Birkir R Gudjonsson
**/
void effect_test()
{
  // loop through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // set base brightness
    float brightness = i == eposition ? 0.5 : 0.0;

    // fade down led if next to current led
    if ((edirection == 0 && i == eposition - 1) || (edirection == 1 && i == eposition + 1)) brightness = 0.1;
    if ((edirection == 0 && i == eposition - 2) || (edirection == 1 && i == eposition + 2)) brightness = 0.01;

    // calulate color value
    hsl2rgb(color, saturation, brightness);

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }

  // reverse effect direction if at last led
  if (edirection == 0 && eposition + 1 == NumLEDs) edirection = 1;

  // reverse effect direction if at first led
  if (edirection == 1 && eposition == 0) edirection = 0;

  // iterate through effect position based on direction
  edirection == 0 ? eposition++ : eposition--;
}



/**
 * Crossfade effect
 * (crossfades between colors smoothly)
 * @author Birkir R Gudjonsson
**/
void effect_crossfade()
{
  // loop through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate color value
    hsl2rgb(color, saturation, 0.5);

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }
}



/**
 * Gradient effect
 * (colors each row by base color * position ratio)
 * @author Birkir R Gudjonsson
**/
void effect_gradient()
{
  // led iterator
  int i = 0;

  // loop through rows
  for (int r = 0; r < rows; r++)
  {
    // loop through columns
    for (int c = 0; c < cols; c++)
    {
      // calculate led color
      hsl2rgb(color + (((float) r / (float) rows) * 0.25), saturation, 0.5);

      // set led colors
      LEDChannels[i][0] = rgb[0] * 4;
      LEDChannels[i][1] = rgb[1] * 4;
      LEDChannels[i][2] = rgb[2] * 4;

      // iterate led count
      i++;
    }
  }
}


/**
 * Strobe effect
 * (turns whole led array on/off at delay speed)
 * @author Birkir R Gudjonsson
**/
void effect_strobe()
{
  // iterate through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate led color
    hsl2rgb(color, saturation, eposition % 2 == 1 ? 0.5 : 0.0);

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }

  // iterate effect position
  eposition++;

  // reset effect position if equal rows
  if (eposition == rows)
    eposition = 0;
}




/**
 * Sparkle effect
 * (randomly dim leds with random dim value)
 * @author Birkir R Gudjonsson
**/
void effect_sparkle()
{
  // loop through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate color value
    hsl2rgb(color, saturation, (i % random(0, 5) == 1 ? 0.5 : (random(0, 5) == 1 ? 0.35 : 0.45)));

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }

  // iterate effect position
  eposition++;

  // if effect position equal rows
  if (eposition == rows)
    eposition = 0;
}



/**
 * Marquee effect
 * (swaps two cells with 50% and 100% brightness)
 * @author Birkir R Gudjonsson
**/
void effect_marquee()
{
  // loop through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate led color
    hsl2rgb(color, saturation, eposition % 2 == 1 ? (i % 2 == 1 ? 0.5 : 0.1) : (i % 2 == 1 ? 0.1 : 0.5));

    // set led colors
    LEDChannels[i][0] = rgb[0];
    LEDChannels[i][1] = rgb[1];
    LEDChannels[i][2] = rgb[2];
  }

  // iterate effect position
  eposition++;

  // reset effect position if equal rows
  if (eposition == rows)
    eposition == 0;
}



/**
 * Police effect
 * (swaps red/blue, works best at 10ms speed)
 * @author Birkir R Gudjonsson
**/
void effect_police()
{
  // cast effect position to float
  float p = (float) eposition;

  // loop through leds
  for (int i = 0; i < NumLEDs; i++)
  {
    // if left side of led array
    if (i < NumLEDs / 2)
      hsl2rgb(0, saturation, (p < 10.0 ? ((float) p / 10.0) * 0.5 : (p >= 10 ? 0.5 - (((float) p - 10.0) / 10) * 0.5 : 0.0)));
    else
      hsl2rgb(0.65, saturation, (p < 10.0 ? 0.5 - ((float) p / 10.0) * 0.5 : (p >= 10.0 ? (((float) p - 10) / 10.0) * 0.5 : 0.5)));

    // write led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }

  // iterate effect position
  eposition++;

  // if effect position greater than 20
  if (eposition > 20)
    eposition = 0;
}



/**
 * Spectrum effect
 * (graphical sound spectrum with colorized bars)
 * @author Birkir R Gudjonsson
**/
void effect_spectrum()
{
  // fix rows
  fixrow = true;

  // storage for equalizer values
  int eq[7] = {};

  // reset the msgeq7 chip
  digitalWrite(msgeq7_reset, HIGH);
  digitalWrite(msgeq7_reset, LOW);

  // loop through equalizer channels
  for (int i = 0; i < 7; i++)
  {
    // strobe pin off
    digitalWrite(msgeq7_strobe, LOW);

    // delay to sanitalize the chip
    delayMicroseconds(30);

    // read analog pin
    eq[i] = analogRead(msgeq7_analog);

    // strobe pin on
    digitalWrite(msgeq7_strobe, HIGH);
  }

  // set lcd cursor
  lcd.setCursor(0, 1);

  // write letters
  for (int i = 0; i < 8; i++)
  {
    lcd.write(eq[i] / 1023 * 8);
  }

  // led count iterator
  int i = 0;

  // loop through rows
  for (int r = 0; r < rows; r++)
  {
    // calculate equalizer value
    int eqval = (r == 1 ? (float) (eq[0] + eq[1]) / 2046.0 : (r > 1 ? (float) eq[r - 1] / 1023 : (float) eq[r] / 1023)) * (cols + 1);

    // loop through columns
    for (int c = 0; c < cols; c++)
    {
      // calculate color value
      hsl2rgb(c * 0.15, saturation, (c < eqval) ? (0.5 - (((float) c) / (float) cols) * 0.5) : 0.0);

      // set led colors
      LEDChannels[i][0] = rgb[0] * 4;
      LEDChannels[i][1] = rgb[1] * 4;
      LEDChannels[i][2] = rgb[2] * 4;
      i++;
    }
  }
}



/**
 * Bass pulse effect
 * (pulses whole led array in order to 63Hz frequency)
 * @author Birkir R Gudjonsson
**/
void effect_basspulse()
{
  // storage for equalizer values
  int eq[7] = {};

  // reset the msgeq7 chip
  digitalWrite(msgeq7_reset, HIGH);
  digitalWrite(msgeq7_reset, LOW);

  // loop through equalizer channels
  for (int i = 0; i < 7; i++)
  {
    // strobe pin off
    digitalWrite(msgeq7_strobe, LOW);

    // delay to sanitalize the chip
    delayMicroseconds(30);

    // read analog pin
    eq[i] = analogRead(msgeq7_analog);

    // strobe pin on
    digitalWrite(msgeq7_strobe, HIGH);
  }

  // loop through led array
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate color
    hsl2rgb(color, saturation, (eq[0] / 1023.0) * 0.45);

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }
}



/**
 * Plasma effect
 * (colorize two sinus waves for each axis)
 * @author Birkir R Gudjonsson
**/
float plasmapos = 0.00;
void effect_plasma()
{
  // loop through columns
  for (int c = 0; c < cols; c++)
  {
    // calculate Y-axis sinus wave
    float b = (1.00 + sin(3.14159 * c * 0.75 / (cols - 1.0) + plasmapos)) * 0.5;

    // loop through rows
    for (int r = 0; r < rows; r++)
    {
      // calculate X-axis sinus wave
      float a = (1.00 + sin(3.14159 * r * 0.75 / (rows + 1.0) + plasmapos)) * 0.5;

      // set led position in array
      int i = r + (c * rows);

      // x&y calculation is less than 2/3
      if ((a + b) < 0.667)
      {
        LEDChannels[i][0] = 1023.0 - (1010.0 * ((a + b) / 0.667));
        LEDChannels[i][1] = 1010.0 * ((a + b) / 0.667);
        LEDChannels[i][2] = 0;
      }
      // x&y calculation is less than 4/3
      else if ((a + b) < 1.333)
      {
        LEDChannels[i][0] = 0;
        LEDChannels[i][1] = 1023.0 - (1010.0 * ((a + b - 0.667) / 0.667));
        LEDChannels[i][2] = 1010.0 * ((a + b - 0.667) / 0.667);
		  }
      else
      {
        LEDChannels[i][0] = 1010.0 * ((a + b - 1.333) / 0.667);
        LEDChannels[i][1] = 0;
        LEDChannels[i][2] = 1023.0 - (1010.0 * ((a + b - 1.33) / 0.667));
      }
    }
  }

  // iterate plasma position
  plasmapos = plasmapos + 0.0314159 * 0.75;

  // reset plasma position if greater than PI x 2
  if (plasmapos > 6.2831)
    plasmapos = 0.00;
}



/**
 * Sinus wave effect
 * (calculates sinus wave with different color in background)
 * @author Birkir R Gudjonsson
**/
void effect_sinwave()
{
  // led count iterator
  int i = 0;

  // loop through rows
  for (int r = 0; r < rows; r++)
  {
    // loop through columns
    for (int c = 0; c < cols; c++)
    {
      // calculate sinus wave
      if (c <= sin(3.14159 + (0.31415 * (r + (eposition < rows ? eposition + 1 : r - eposition + 1)))) * -cols)
        hsl2rgb(color, saturation, 0.5);
      else
        hsl2rgb(color + 0.5, saturation, 0.5);

      // set led colors
      LEDChannels[i][0] = rgb[0] * 4;
      LEDChannels[i][1] = rgb[1] * 4;
      LEDChannels[i][2] = rgb[2] * 4;

      // iterate through leds
      i++;
    }
  }
}



/**
 * Pulse effect
 * (pulses whole led array)
 * @author Birkir R Gudjonsson
**/
void effect_pulse()
{
  for (int i = 0; i < NumLEDs; i++)
  {
    // calculate color values
    hsl2rgb(color, saturation, eposition * 0.05);

    // set led colors
    LEDChannels[i][0] = rgb[0] * 4;
    LEDChannels[i][1] = rgb[1] * 4;
    LEDChannels[i][2] = rgb[2] * 4;
  }

  // reverse effect direction if at last led
  if (edirection == 0 && eposition == 10) edirection = 1;

  // reverse effect direction if at first led
  if (edirection == 1 && eposition == 0) edirection = 0;

  // iterate through effect position based on direction
  edirection == 0 ? eposition++ : eposition--;
}



/**
 * Create characters for equalizer
**/
void lcd_equalizer()
{
  byte e0[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111};
  byte e1[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111};
  byte e2[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111};
  byte e3[8] = {B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111};
  byte e4[8] = {B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111};
  byte e5[8] = {B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111};
  byte e6[8] = {B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
  byte e7[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
  lcd.createChar(0, e0);
  lcd.createChar(1, e1);
  lcd.createChar(2, e2);
  lcd.createChar(3, e3);
  lcd.createChar(4, e4);
  lcd.createChar(5, e5);
  lcd.createChar(6, e6);
  lcd.createChar(7, e7);
}



/**
 * Convert HSL to RGB
 * (converts hue, saturation & lightness to rgb value)
 * @author Birkir R Gudjonsson
**/
void hsl2rgb(double h, double s, double l)
{
  double r, g, b;
  if (s == 0.0) r = g = b = l;
  else
  {
    double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    double p = 2 * l - q;
    r = hue2rgb(p, q, h + 0.333);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 0.333);
  }
  rgb[0] = r * 255;
  rgb[1] = g * 255;
  rgb[2] = b * 255;
}



/**
 * Convert HUE to RGB
 * (converts hue value to rgb value)
 * @author Birkir R Gudjonsson
**/
double hue2rgb(double p, double q, double t)
{
  if(t < 0.000) t += 1;
  if(t > 1.000) t -= 1;
  if(t < 0.167) return p + (q - p) * 6 * t;
  if(t < 0.500) return q;
  if(t < 0.667) return p + (q - p) * (0.667 - t) * 6;

 return p;
}
