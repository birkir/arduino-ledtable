/**
 * Arduino Mega 1280
 *
 * Analog:  1 - 16
 * Digital: 22 - 53
 *
 * Serial: 0 (RX) +  1 (TX)
 *        19 (RX) + 18 (TX)
 *        17 (RX) + 16 (TX)
 *        15 (RX) + 14 (TX)
 *
 * Interrupts: 2, 3, 18, 19, 20, 21
 *
 * I2C: 20 (SDA)
 *      21 (SCL)
 *
 * SPI: 50 (MISO)
 *      51 (MOSI)
 *      52 (SCK)
 *      53 (SS)
 * 
 * PWM: 0 - 13
 * 
 * LED: 13
**/

// Buttons
#define b00_ANL_pin 1
#define b01_ANL_pin 2

// Cyrstal LCD
#define  rs_DIG_pin 22
#define db7_DIG_pin 23
#define db5_DIG_pin 24
#define db6_PWM_pin 2
#define db4_PWM_pin 3
#define   e_PWM_pin 4

// ShiftBrite
#define sbC_SPI_pin 52 // clock
#define sbE_SPI_pin 53 // enable
#define sbL_PWM_pin 5  // latch
#define sbD_SPI_pin 51 // data


// shiftbrite
unsigned long SB_CommandPacket;
int SB_CommandMode;
int SB_BlueCommand;
int SB_RedCommand;
int SB_GreenCommand;

// led array
#define numleds 4
int LED[numleds][3] = {0};
int rows = 8;
int cols = 4;
byte rgb[3];
double hsl[3];
bool rowfix = false;

// colors
int   color_mode = 0;
float color_sat = 1.0;
int   color_dir = 0;
int   color_pos = 0;

// effects
int effect_name = 1;
int effect_storage[][3] = {};
int effect_pos = 0;
int effect_dir = 0;
int effect_speed = 100;
float effect_float = 0.0;

// equalizer
int equalizer[7];

/**
 * initialize
**/
void setup()
{
	Serial.begin(9600);
	Serial.println("project :: led-table");
	Serial.println("[ ] initializing...");
	pinMode(datapin, OUTPUT);
	pinMode(latchpin, OUTPUT);
	pinMode(enablepin, OUTPUT);
	pinMode(clockpin, OUTPUT);
	digitalWrite(latchpin, LOW);
	digitalWrite(enablepin, LOW);
	Serial.println("[ ] successfully initialized!");
	color_change(0);
	effect_change(1);
	speed_change(100);
}

/**
 * write out led array
**/
void writeleds()
{
	for (int i = (numleds - 1); i >= 0; i--)
	{
		int row = floor(i / cols);
		if (row % 2 == 1 && rowfix == true){
			int num = row * cols;
			switch (i - num)
			{
				case 0: i = num + 3; break;
				case 1: i = num + 2; break;
				case 2: i = num + 1; break;
				case 3: i = num; break;
			}
		}
		SB_RedCommand = LED[i][0];
		SB_GreenCommand = LED[i][1];
		SB_BlueCommand = LED[i][2];
		SB_SendPacket();
	}
}

/**
 * set specific led color
**/
void led(int i, float h, float s, float l)
{
	hsl2rgb(h, s, l);
	LED[i][0] = rgb[0] * 4;
	LED[i][1] = rgb[1] * 4;
	LED[i][2] = rgb[2] * 4;
}

/**
 * main loop
**/
void loop()
{
	color_process();
	effect_loop();
}

/**
 * color process
**/
void color_process()
{
	switch (color_mode) {
		case 0: color_pos++; if (color_pos >= 360) color_pos = 0; break;
		case 1: color_pos--; if (color_pos <= 0 || color_pos > 360) color_pos = 360; break;
		case 2: color_pos++; if (color_pos >= 3600) color_pos = 0; break;
		case 3: color_pos = random(0, 360); break;
		case 4: color_pos++; if (color_pos >= 36) color_pos = 0; break;
	}
}

/**
 * color value
**/
float color_value()
{
	switch (color_mode) {
		case 0:
		case 1:
		case 3: return (float) color_pos / 360; break;
		case 2: return (float) color_pos / 3600; break;
		case 4: return (float) color_pos / 36; break;
		case 5: return (float) color_pos; break;
	}
}

/**
 * change effect
**/
void effect_change(int name)
{
	Serial.print("[ ] set effect to '");
	Serial.print(name);
	Serial.println("'.");

	effect_name = name;
	effect_pos = 0;
	effect_float = 0.0;

	for (int i = 0; i < numleds; i++){ led(i, 0.0, 0.0, 0.0); }

	switch (name)
	{
		case 1:  rowfix = 0; break; // row
		case 2:  rowfix = 0; break; // fade
		case 3:  rowfix = 0; break; // strobe
		case 4:  rowfix = 0; break; // test
		case 5:  rowfix = 0; break; // blinks
		case 6:  rowfix = 0; break; // sparkle
		case 7:  rowfix = 0; break; // plasma
		case 8:  rowfix = 0; break; // police
		case 9:  rowfix = 1; break; // snake
		case 10: rowfix = 1; break; // spectrum
		case 11: rowfix = 0; break; // basspulse
		default:
			Serial.print("[ ] could not find effect '"); Serial.print(effect_name); Serial.println("'.");
	}
}

void speed_change(int speed)
{
	Serial.print("[ ] set speed to '");
	Serial.print(speed);
	Serial.println("'.");
	effect_speed = speed;
}

void color_change(int mode)
{
	Serial.print("[ ] set color mode to '");
	Serial.print(mode);
	Serial.println("'.");
	color_mode = mode;
}

/**
 * loop effect
**/
void effect_loop()
{
	switch (effect_name)
	{
		case 1: effect_loop_row(); break;        // row
		case 2: effect_loop_fade(); break;       // fade
		case 3: effect_loop_strobe(); break;     // strobe
		case 4: effect_loop_test(); break;       // test
		case 5: effect_loop_blinks(); break;     // blinks
		case 6: effect_loop_sparkle(); break;    // sparkle
		case 7: effect_loop_plasma(); break;     // plasma
		case 8: effect_loop_police(); break;     // police
		case 9: effect_loop_snake(); break;      // snake
		case 10: effect_loop_spectrum(); break;  // spectrum
		case 11: effect_loop_basspulse(); break; // basspulse
	}
	writeleds();
	delay(effect_speed);
}

/**
 * effect: row
**/
void effect_loop_row()
{
	int i = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (effect_pos == r)
			{
				led(i, color_value(), color_sat, 0.5);
			}
			else if (effect_dir == 1 && r < effect_pos)
			{
				led(i, color_value(), color_sat, (effect_pos - r <= 3 ? (50.0 / ((float) effect_pos - (float) r)) / 100.0 : 0));
			}
			else if (effect_dir == 0 && r > effect_pos)
			{
				led(i, color_value(), color_sat, (r - effect_pos <= 3 ? (50.0 / ((float) r - (float) effect_pos)) / 100.0 : 0));
			}
			else
			{
				led(i, color_value(), color_sat, 0);
			}
			i++;
		}
	}
	if (effect_dir == 1) { effect_pos++; } else { effect_pos--; }
	if (effect_pos >= rows) { effect_dir = 0; effect_pos--; }
	if (effect_pos == 0) { effect_dir = 1; effect_pos = 0; }
}

/**
 * effect: fade
**/
void effect_loop_fade()
{
	int i = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			led(i, color_value(), color_sat, 0.5);
			i++;
		}
	}
}

/**
 * effect: strobe
**/
void effect_loop_strobe()
{
	for (int i = 0; i < numleds; i++)
	{
		led(i, color_value(), color_sat, effect_pos % 2 == 1 ? 0 : 0.5);
	}
	effect_pos++;
}

/**
 * effect: test
**/
void effect_loop_test()
{
	for (int i = 0; i < numleds; i++)
	{
		int trail = 3;
		if (i == effect_pos)
		{
			led(i, color_value(), color_sat, 0.5);
		}
		else if (effect_dir == 0 && (i >= (effect_pos - trail) && i < effect_pos))
		{
			led(i, color_value(), color_sat, ((100.0 / ((float) effect_pos - (float) i)) / 100.0 / 2.0));
		}
		else if (effect_dir == 1 && (i <= (effect_pos + trail) && i > effect_pos))
		{
			led(i, color_value(), color_sat, ((100.0 / ((float) i - (float) effect_pos)) / 100.0 / 2.0));
		}
		else
		{
			led(i, 0, 1, 0);
		}
		if (effect_dir == 0 && effect_pos == (numleds - 1)) { effect_dir = 1; }
		else if (effect_dir == 1 && effect_pos == 0) { effect_dir = 0; }
	}
	effect_pos = (effect_dir == 1 ? effect_pos - 1 : effect_pos + 1);
}

/**
 * effect: blinks
**/
void effect_loop_blinks()
{
	int random_length = random(1, 6);
	int randoms[random_length];
	for (int x = 0; x < random_length; x++)
	{
		randoms[x] = random(0, numleds);
	}
	for (int i = 0; i < numleds; i++)
	{
		if (effect_storage[i] && effect_storage[i][1] >= 0.00)
		{
			effect_storage[i][1] = effect_storage[i][1] - 0.05;
			led(i, effect_storage[i][0], color_sat, effect_storage[i][1]);
		}
		for (int y = 0; y < random_length; y++)
		{
			if (randoms[y] == i)
			{
				effect_storage[i][0] = color_value();
				effect_storage[i][1] = 0.50;
				led(i, color_value(), color_sat, 0.5);
			}
		}
	}
}

/**
 * effect: sparkle
**/
void effect_loop_sparkle()
{
	for (int i = 0; i < numleds; i++)
	{
		led(i, color_value(), color_sat, (i % random(0, 5) == 1 ? 0.5 : (random(0, 5) == 1 ? 0.4 : 0.45)));
	}
}

/**
 * effect: plasma
**/
void effect_loop_plasma()
{
	for (int c = 0; c < cols; c++)
	{
		float b = (1 + sin(3.14159 * (float) c * 0.75 / ((float) cols - 1.0) + (float) effect_float)) * 0.3;
		for (int r = 0; r < rows; r++)
		{
			float a = (1 + sin(3.14159 * (float) r * 0.75 / ((float) rows + 2.0 - 1.0) + (float) effect_float)) * 0.3;
			rgb2hsl(250.0 * (((float) a + (float) b - 1.33) / 0.667), 0, 255.0 - (250.0 * ((a + b - 1.33) / 0.667)));
			if ((a + b) < 0.667)
			{
				rgb2hsl(255 - (250.0 * (((float) a + (float) b) / 0.667)), 250.0 * (((float) a + (float) b) / 0.667), 0);
			}
			else if((a + b) < 1.333)
			{
				rgb2hsl(0, 255 - (250.0 * (((float) a + (float) b - 0.667) / 0.667)), 250.0 * (((float) a + (float) b - 0.667) / 0.667));
			}
			led(r + (c * rows), hsl[0], color_sat, hsl[2]);
		}
	}
	effect_float = (float) effect_float + (3.14159 / 50.0) * 0.75;
	if (effect_float > (3.14159 * 2))
	{
		effect_float = 0.0;
	}
}

/**
 * effect: police
**/
void effect_loop_police()
{
	float p = (float) effect_pos;
	for (int i = 0; i < numleds; i++)
	{
		if (i < numleds / 2)
		{
			led(i, 0, color_sat, (p < 10.0 ? ((float) p / 10.0) * 0.5 : (p >= 10 ? 0.5 - (((float) p - 10.0) / 10) * 0.5 : 0.0)));
		}
		else
		{
			led(i, 0.65, color_sat, (p < 10.0 ? 0.5 - ((float) p / 10.0) * 0.5 : (p >= 10.0 ? (((float) p - 10) / 10.0) * 0.5 : 0.5)));
		}
	}
	effect_pos++;
	if (effect_pos > 20){ effect_pos = 0; }
}

/**
 * effect: snake
**/
void effect_loop_snake()
{
	if (random(0,11) > 8)
	{
		effect_dir = random(1,3) + (effect_dir < 2 ? 2 : -1);
	}
	led(0, 0, 0, 0);
	led(effect_storage[5][0], 0, 0, 0);
	for (int i = 4; i >= 0; i--)
	{
		if (effect_storage[i][0])
		{
			led(effect_storage[i][0], color_value(), color_sat, 0.5 - (((float) i / 5) * 0.5));
			effect_storage[i + 1][0] = effect_storage[i][0];
		}
	}
	int _c = effect_pos % cols;
	int _r = floor((float) effect_pos / (float) cols);
	int _l = effect_pos - ((_r - 1) * cols);
	if (effect_dir == 0)
	{
		effect_pos = ((_r >= (rows - 1) ? -1 : _r) * (float) cols) + _l;
	}
	else if (effect_dir == 1)
	{
		effect_pos = ((_r <= 0 ? (rows - 2) : _r - 2) * (float) cols) + _l;
	}
	else if (effect_dir == 2)
	{
		effect_pos = ((_r - 1) * cols) + (_l == cols ? ((cols * 2) - 1) : _l - 1);
	}
	else if (effect_dir == 3)
	{
		effect_pos = ((_r - 1) * cols) + (_l == ((cols * 2) - 1) ? cols : _l + 1);
	}
	effect_storage[0][0] = effect_pos;
	led(effect_pos, color_value(), color_sat, 0.5);
}

/**
 * effect: spectrum
**/
void effect_loop_spectrum()
{
	equalizer_process();
	int i = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int eqval = (r == 1 ? (float) (equalizer[0] + equalizer[1]) / 2046.0 : (r > 1 ? (float) equalizer[r - 1] / 1023 : (float) equalizer[r] / 1023)) * (float) cols;
			led(i, color_value(), color_sat, (c < eqval) ? (0.5 - (((float) c + 1) / (float) cols) * 0.5) : 0.0);
			i++;
		}
	}
}

/**
 * effect: bass pulse
**/
void effect_loop_basspulse()
{
	equalizer_process();
	for (int i = 0; i < numleds; i++)
	{
		led(i, color_value(), color_sat, (((float) equalizer[0] * 0.5) + ((float) equalizer[1] * 1.5) + ((float) equalizer[2] * 0.5)) / 1023.0 / 3.0);
	}
}


/**
 * equalizer process
**/
void equalizer_process()
{
	for (int i = 0; i < 7; i++)
	{
		equalizer[i] = random(0, 1024);
	}
}

/**
 * send shiftbrite packet
**/
void SB_SendPacket()
{
	SB_CommandPacket = SB_CommandMode & B11;
	SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_BlueCommand & 1023);
	SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_RedCommand & 1023);
	SB_CommandPacket = (SB_CommandPacket << 10)  | (SB_GreenCommand & 1023);

	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 24);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 16);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket >> 8);
	shiftOut(datapin, clockpin, MSBFIRST, SB_CommandPacket);

	delay(1);
	digitalWrite(latchpin, HIGH);
	delay(1);
	digitalWrite(latchpin, LOW);
}

/**
 * convert rgb to hsl value
**/
void rgb2hsl(byte r, byte g, byte b)
{
	double rd = (double) r/255;
	double gd = (double) g/255;
	double bd = (double) b/255;
	double max = max(rd, max(gd, bd));
	double min = min(rd, min(gd, bd));
	double h, s, l = (max + min) * 0.5;

	if (max == min)
	{
		h = s = 0;
	}
	else
	{
		double d = max - min;
		s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
		if (max == rd) {
			h = (gd - bd) / d + (gd < bd ? 6 : 0);
		} else if (max == gd) {
			h = (bd - rd) / d + 2;
		} else if (max == b) {
			h = (rd - gd) / d + 4;
		}
		h /= 6;
	}
	hsl[0] = h;
	hsl[1] = s;
	hsl[2] = l;
}

/**
 * convert hsl to rgb value
**/
void hsl2rgb(double h, double s, double l)
{
	double r, g, b;

	if (s == 0.0)
	{
		r = g = b = l;
	}
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
 * convert hue to rgb value
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
