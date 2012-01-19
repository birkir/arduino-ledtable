/**
 * Matrix object
 *
 * @author Birkir R Gudjonsson
**/
var matrix = {

	/** Effect **/
	effect: {
		name: 'blinks',
		storage: [],
		pos: 0,
		dir: 0
	},

	/** Color **/
	color: {
		sat: 1,
		mode: 0,
		dir: 0,
		pos: 0,
		value: function(){
			switch (matrix.color.mode) {
				case 0:
				case 1:
				case 3: return matrix.color.pos / 360; break;
				case 2: return matrix.color.pos / 3600; break;
				case 4: return matrix.color.pos / 36; break;
				case 5: return matrix.color.pos; break;
			}
		},
		process: function() {
			switch (matrix.color.mode) {
				case 0: matrix.color.pos++; if (matrix.color.pos >= 360) matrix.color.pos = 0; break;
				case 1: matrix.color.pos--; if (matrix.color.pos == 0 || matrix.color.pos > 360) matrix.color.pos = 360; break;
				case 2: matrix.color.pos++; if (matrix.color.pos >= 3600) matrix.color.pos = 0; break;
				case 3: matrix.color.pos = Math.random() * 360; break;
				case 4: matrix.color.pos++; if (matrix.color.pos >= 36) matrix.color.pos = 0; break;
			}
		}
	},

	/** Speed **/
	speed: 82,

	/** Columns **/
	cols: 4,

	/** Rows **/
	rows: 8,

	/** Total amount of LEDs **/
	leds: function(){ return this.cols * this.rows; },

	/** Loop container **/
	loop: function(){
		matrix.color.process();
		matrix.effects[matrix.effect.name].loop();
		setTimeout(matrix.loop, matrix.speed);
	},

	/**
	 * Initializing function
	**/
	init: function(){
		for (var effect in matrix.effects)
		{
			var option = document.createElement('option');
			option.value = effect;
			option.text = (effect.charAt(0).toUpperCase()) + effect.substr(1);
			document.getElementById('matrix-effect').appendChild(option);
		}
		document.getElementById('matrix-effect').onchange = function(event){
			matrix.change(event.srcElement.children[event.srcElement.options.selectedIndex].value);
		};
		document.getElementById('matrix-color-mode').onchange = function(event){
			matrix.color.mode = parseInt(event.srcElement.children[event.srcElement.options.selectedIndex].value);
		};
		document.getElementById('matrix-color-saturation').onchange = function(event){
			matrix.color.sat = event.srcElement.value / 100;
		};
		document.getElementById('matrix-color-saturation').onmouseup = function(event){
			matrix.color.sat = event.srcElement.value / 100;
		};
		document.getElementById('matrix-color-saturation').onmousewheel = function(event){
			matrix.color.sat = event.srcElement.value / 100;
		};
		document.getElementById('matrix-speed').onchange = function(event){
			matrix.speed = event.srcElement.value;
		};
		document.getElementById('matrix-speed').onmouseup = function(event){
			matrix.speed = event.srcElement.value;
		};
		document.getElementById('matrix-speed').onmousewheel = function(event){
			matrix.speed = event.srcElement.value;
		};
		document.getElementById('matrix-color').onchange = function(event){
			matrix.color.pos = event.srcElement.value / 1000;
		};
		document.getElementById('matrix-color').onmousewheel = function(event){
			matrix.color.pos = event.srcElement.value / 1000;
		};
		document.getElementById('matrix-color').onmouseup = function(event){
			matrix.color.pos = event.srcElement.value / 1000;
		};
		matrix.change(document.getElementById('matrix-effect').children[0].value);
		matrix.loop();
	},

	/**
	 * Change effect
	**/
	change: function(name){
		matrix.fixpixels = false;
		if (matrix.effects[name]) {
			if (matrix.effects[name].init) {
				matrix.effects[name].init();
			}
			matrix.effect.pos = 0;
			matrix.effect.storage = [];
			matrix.effect.name = name;
		}
		else
		{
			console.log('effect [' + name + '] not found');
		}
	},

	/**
	 * Equalizer
	 */
	 equalizer: function(){
		var equalizer = [];
		for (var i = 0; i < 7; i++)
		{
			equalizer[i] = Math.floor(Math.random() * 1023);
		}
		return equalizer;
	 },

	/**
	 * Convert functions
	**/
	convert: {
		/**
		 * Convert HSL to RGB values
		**/
		hsltorgb: function(h, s, l){
			var r, g, b;
			if (s == 0)
			{
				r = g = b = l;
			}
			else
			{
				var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
				var p = 2 * l - q;
				r = matrix.convert.huetorgb(p, q, h + 1/3);
				g = matrix.convert.huetorgb(p, q, h);
				b = matrix.convert.huetorgb(p, q, h - 1/3);
			}
			return [r * 255, g * 255, b * 255];
		},
		/**
		 * Convert HUE to RGB values
		**/
		huetorgb: function(p, q, t){
			if (t < 0) t += 1;
			if (t > 1) t -= 1;
			if (t < 1/6) return p + (q - p) * 6 * t;
			if (t < 1/2) return q;
			if (t < 2/3) return p + (q - p) * (2/3 - t) * 6;
			return p;
		},

		/**
		 * Convert RGB to HUE
		**/
		rgbtohsl: function(r, g, b){
			r /= 255, g /= 255, b /= 255;
			var max = Math.max(r, g, b), min = Math.min(r, g, b);
			var h, s, l = (max + min) / 2;
			if(max == min)
			{
				h = s = 0;
			}
			else
			{
				var d = max - min;
				s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
				switch(max){
					case r: h = (g - b) / d + (g < b ? 6 : 0); break;
					case g: h = (b - r) / d + 2; break;
					case b: h = (r - g) / d + 4; break;
				}
				h /= 6;
			}
			return [h, s, l];
		}
	},

	/**
	 * Set LED color in HSL format
	**/
	fixpixels: false,
	led: function(i, h, s, l, rgbmode){
		if (rgbmode == true){
			var color = [h, s, l];
		} else {
			var color = matrix.convert.hsltorgb(h, s, l);
		}
		var row = Math.floor(i / matrix.cols)
		if (row % 2 == 1 && matrix.fixpixels == true){
			var num = row * matrix.cols;
			switch (i - num) {
				case 0: i = num + 3; break;
				case 1: i = num + 2; break;
				case 2: i = num + 1; break;
				case 3: i = num; break;
			}
		}
		if (document.getElementsByTagName('li')[i]) {
			document.getElementsByTagName('li')[i].style.backgroundColor = 'rgba(' + Math.floor(color[0]) + ', ' + Math.floor(color[1]) + ', ' + Math.floor(color[2]) + ', 1)';
		}
	},

	clear: function(){
		for(i = 0; i < matrix.leds(); i++) {
			matrix.led(i, 0, 0, 0);
		}
	},

	/**
	 * Effects storage
	**/
	effects: {
		row: {
			init: function(){
				matrix.effect.pos = 0;
				matrix.effect.dir = 1;
			},
			loop: function() {
				var i = 0;
				for (var r = 0; r < matrix.rows; r++) {
					for (var c = 0; c < matrix.cols; c++) {
						if (matrix.effect.pos == r) {
							matrix.led(i, matrix.color.value(), matrix.color.sat, 0.5);
						}
						else if (matrix.effect.dir == 1 && r < matrix.effect.pos) {
							matrix.led(i, matrix.color.value(), matrix.color.sat, (matrix.effect.pos - r <= 3 ? (50 / (matrix.effect.pos - r)) / 100 : 0));
						}
						else if (matrix.effect.dir == 0 && r > matrix.effect.pos) {
							matrix.led(i, matrix.color.value(), matrix.color.sat, (r - matrix.effect.pos <= 3 ? (50 / (r - matrix.effect.pos)) / 100 : 0));
						}
						else
						{
							matrix.led(i, matrix.color.value(), matrix.color.sat, 0);
						}
						i++;
					}
				}
				if (matrix.effect.dir == 1) {
					matrix.effect.pos++;
				} else {
					matrix.effect.pos--;
				}
				if (matrix.effect.pos >= matrix.rows) {
					matrix.effect.dir = 0;
					matrix.effect.pos--;
				}
				if (matrix.effect.pos == 0){
					matrix.effect.dir = 1;
					matrix.effect.pos = 0;
				}
			}
		},
		fade: {
			loop: function() {
				var j = 0;
				for (r = 0; r < matrix.rows; r++) {
					for (c = 0; c < matrix.cols; c++) {
						matrix.led(j, matrix.color.value(), matrix.color.sat, 0.5);
						j++;
					}
				}
				matrix.effect.pos++;
			}
		},
		strobe: {
			loop: function() {
				for (i = 0; i < matrix.leds(); i++) {
					matrix.led(i, matrix.color.value(), matrix.color.sat, matrix.effect.pos % 2 == 1 ? 0 : 0.5);
				}
				matrix.effect.pos++;
			}
		},
		snakie: {
			init: function() {
				matrix.clear();
			},
			loop: function(){
				for (i = 0; i < matrix.leds(); i++) {
					var trail = 10;
					if (i == matrix.effect.pos) {
						matrix.led(i, matrix.color.value(), matrix.color.sat, 0.5);
					} else if (matrix.effect.dir == 0 && (i >= (matrix.effect.pos - trail) && i < matrix.effect.pos)) {
						var col = (100 / (matrix.effect.pos - i)) / 100;
						matrix.led(i, matrix.color.value(), matrix.color.sat, (col / 2));
					} else if (matrix.effect.dir == 1 && (i <= (matrix.effect.pos + trail) && i > matrix.effect.pos)) {
						var col = (100 / (i - matrix.effect.pos)) / 100;
						matrix.led(i, matrix.color.value(), matrix.color.sat, (col / 2));
					} else {
						matrix.led(i, 0, 1, 0);
					}
					if (matrix.effect.dir == 0 && matrix.effect.pos == (matrix.leds() - 1))
					{
						matrix.effect.dir = 1;
					}
					else if (matrix.effect.dir == 1 && matrix.effect.pos == 0)
					{
						matrix.effect.dir = 0;
					}
				}
				matrix.effect.pos = (matrix.effect.dir == 1 ? matrix.effect.pos - 1 : matrix.effect.pos + 1);
			}
		},
		blinks: {
			init: function() {
				matrix.clear();
			},
			loop: function() {
				var randoms = [];
				for (var x = 0; x < Math.floor(Math.random() * 5); x++)
				{
					randoms[x] = Math.floor(Math.random() * matrix.leds());
				}
				for (var i = 0; i < matrix.leds(); i++)
				{
					if (matrix.effect.storage[i]) {
						var tmp = matrix.effect.storage[i];
						matrix.effect.storage[i] = [tmp[0], parseFloat(tmp[1]) - 0.05];
						matrix.led(i, tmp[0], matrix.color.sat, parseFloat(tmp[1]) - 0.05);
						if (parseFloat(tmp[1]) - 0.05 == 0.00){
							matrix.effect.storage[i] = null;
						}
					}
					for (y = 0; y < randoms.length; y++){
						if (randoms[y] == i) {
							matrix.effect.storage[i] = [matrix.color.value(), 0.5];
							matrix.led(i, matrix.color.value(), matrix.color.sat, 0.5);
						}
					}
				}
			}
		},
		equalizer: {
			init: function() {
				matrix.clear();
				matrix.fixpixels = true;
			},
			loop: function() {
				var equalizer = matrix.equalizer();
				var z = 0;
				for (var x = 0; x < matrix.rows; x++)
				{
					for (var y = 0; y < matrix.cols; y++)
					{
						var eqval = (x == 1 ? (equalizer[0] + equalizer[1]) / 2046 : (x > 1 ? equalizer[x - 1] / 1023 : equalizer[x] / 1023)) * matrix.cols;
						matrix.led(z, matrix.color.value(), matrix.color.sat, (y < eqval) ? (0.5 - ((y+1) / matrix.cols) * 0.5) : 0.0);
						z++;
					}
				}
			}
		},
		sparkle: {
			loop: function(){
				for (var i = 0; i < matrix.leds(); i++)
				{
					matrix.led(i, matrix.color.value(), matrix.color.sat, (i % Math.floor(Math.random() * 4) == 1 ? 0.5 : (Math.floor(Math.random() * 5) == 1 ? 0.4 : 0.45)));
				}
			}
		},
		plasma: {
			init: function(){
				matrix.clear();
				matrix.fixpixels = false;
				matrix.effect.pos = 0.0;
			},
			loop: function(){
				for (var c = 0; c < matrix.cols; c++) {
					var b = (1 + Math.sin(Math.PI * c * 0.75 / (matrix.cols - 1.0) + matrix.effect.pos)) * 0.3;
					for (var r = 0; r < matrix.rows; r++) {
						var a = (1 + Math.sin(Math.PI * r * 0.75 / (matrix.rows + 2 - 1.0) + matrix.effect.pos)) * 0.3;
						var color = matrix.convert.rgbtohsl(250.0 * ((a + b - 1.33) / 0.667), 0, 255 - (250.0 * ((a + b - 1.33) / 0.667)));
						if ((a + b) < 0.667) {
							color = matrix.convert.rgbtohsl(255 - (250.0 * ((a + b) / 0.667)), 250.0 * ((a + b) / 0.667), 0);
						}else if((a + b) < 1.333) {
							color = matrix.convert.rgbtohsl(0, 255 - (250.0 * ((a + b - 0.667) / 0.667)), 250.0 * ((a + b - 0.667) / 0.667));
						}
						matrix.led(r + (c * matrix.rows), color[0], matrix.color.sat, color[2]);
					}
				}
				matrix.effect.pos = matrix.effect.pos + (Math.PI / 50) * 0.75;
				if (matrix.effect.pos > (Math.PI * 2)) {
					matrix.effect.pos = 0;
				}
			}
		},
		police: {
			init: function(){
				matrix.effect.pos = 0;
			},
			loop: function(){
				var p = matrix.effect.pos;
				for (var i = 0; i < matrix.leds(); i++) {
					if (i < matrix.leds() / 2) {
						matrix.led(i, 0, matrix.color.sat, (p < 10 ? (p / 10) * 0.5 : (p >= 10 ? 0.5 - ((p - 10) / 10) * 0.5 : 0)));
					} else {
						matrix.led(i, 0.65, matrix.color.sat, (p < 10 ? 0.5 - (p / 10) * 0.5 : (p >= 10 ? ((p - 10) / 10) * 0.5 : 0.5)));
					}
				}
				if (matrix.effect.pos >= 20) {
					matrix.effect.pos = -1;
				}
				matrix.effect.pos++;
			}
		},
		snake: {
			init: function(){
				matrix.clear();
				matrix.fixpixels = true;
				matrix.effect.pos = 0;
				matrix.effect.dir = 0;
			},
			loop: function(){
				if (Math.random() * 10 > 8) {
					matrix.effect.dir = Math.ceil(Math.random() * 2) + (matrix.effect.dir < 2 ? 2 : -1);
				}
				matrix.led(0, 0, 0, 0);
				matrix.led(matrix.effect.storage[5], 0, 0, 0);
				for (var i = 4; i >= 0; i--) {
					if (matrix.effect.storage[i]) {
						matrix.led(matrix.effect.storage[i], matrix.color.value(), matrix.color.sat, 0.5 - ((i / 5) * 0.5));
						matrix.effect.storage[i + 1] = matrix.effect.storage[i];
					}
				}
				var _c = matrix.effect.pos % matrix.cols;
				var _r = Math.floor(matrix.effect.pos / matrix.cols);
				var _l = matrix.effect.pos - ((_r - 1) * matrix.cols);
				if (matrix.effect.dir == 0) {
					matrix.effect.pos = ((_r >= (matrix.rows - 1) ? -1 : _r) * matrix.cols) + _l;
				}
				else if (matrix.effect.dir == 1) {
					matrix.effect.pos = ((_r <= 0 ? (matrix.rows - 2) : _r - 2) * matrix.cols) + _l;
				}
				else if (matrix.effect.dir == 2) {
					matrix.effect.pos = ((_r - 1) * matrix.cols) + (_l == matrix.cols ? ((matrix.cols * 2) - 1) : _l - 1);
				}
				else if (matrix.effect.dir == 3) {
					matrix.effect.pos = ((_r - 1) * matrix.cols) + (_l == ((matrix.cols * 2) - 1) ? matrix.cols : _l + 1);
				}
				matrix.effect.storage[0] = matrix.effect.pos;
				matrix.led(matrix.effect.pos, matrix.color.value(), matrix.color.sat, 0.5);
			}
		},
		blocks: {
			init: function(){
				matrix.clear();
				matrix.fixpixels = true;
			},
			loop: function(){
				
			}
		}
	}
};

// Init the matrix
matrix.init();
