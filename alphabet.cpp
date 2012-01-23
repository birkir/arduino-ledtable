int letternum = 51;
int font[letternum][4][5] = {
	{{1,1,1,2,2},{1,0,1,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // 0
	{{1,1,0,2,2},{0,1,0,2,2},{0,1,0,2,2},{1,1,1,2,2}}, // 1
	{{1,1,1,2,2},{0,0,1,2,2},{1,1,0,2,2},{1,1,1,2,2}}, // 2
	{{1,1,1,2,2},{0,0,1,2,2},{0,1,1,2,2},{1,1,1,2,2}}, // 3
	{{1,0,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{0,0,1,2,2}}, // 4
	{{1,1,1,2,2},{1,1,0,2,2},{0,0,1,2,2},{1,1,1,2,2}}, // 5
	{{1,0,0,2,2},{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // 6
	{{1,1,1,2,2},{1,0,1,2,2},{0,0,1,2,2},{0,0,1,2,2}}, // 7
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{1,1,1,2,2}}, // 8
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{0,0,1,2,2}}, // 9
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{1,0,1,2,2}}, // a
	{{1,1,0,2,2},{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // b
	{{1,1,0,2,2},{1,0,0,2,2},{1,0,0,2,2},{1,1,1,2,2}}, // c
	{{1,1,0,2,2},{1,0,1,2,2},{1,0,1,2,2},{1,1,0,2,2}}, // d
	{{1,1,0,2,2},{1,0,1,2,2},{1,0,1,2,2},{1,1,0,2,2}}, // e
	{{1,1,1,2,2},{1,0,0,2,2},{1,1,0,2,2},{1,0,0,2,2}}, // f
	{{1,1,1,2,2},{1,0,0,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // g
	{{1,0,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{1,0,1,2,2}}, // h
	{{1,1,1,2,2},{0,1,0,2,2},{0,1,0,2,2},{1,1,1,2,2}}, // i
	{{1,1,1,2,2},{0,0,1,2,2},{1,0,1,2,2},{0,1,0,2,2}}, // j
	{{1,0,1,2,2},{1,0,1,2,2},{1,1,0,2,2},{1,0,1,2,2}}, // k
	{{1,0,0,2,2},{1,0,0,2,2},{1,0,0,2,2},{1,1,1,2,2}}, // l
	{{1,0,0,0,1},{1,1,0,1,1},{1,0,1,0,1},{1,0,0,0,1}}, // m
	{{1,0,0,1,2},{1,1,0,1,2},{1,0,1,1,2},{1,0,0,1,2}}, // n
	{{1,1,1,2,2},{1,0,0,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // o
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{1,0,0,2,2}}, // p
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,1,2,2},{0,1,0,2,2}}, // q
	{{1,1,1,2,2},{1,0,1,2,2},{1,1,0,2,2},{1,0,1,2,2}}, // r
	{{1,1,1,2,2},{1,0,0,2,2},{0,1,1,2,2},{1,1,1,2,2}}, // s
	{{1,1,1,2,2},{0,1,0,2,2},{0,1,0,2,2},{0,1,0,2,2}}, // t
	{{1,0,1,2,2},{1,0,1,2,2},{1,0,1,2,2},{1,1,1,2,2}}, // u
	{{1,0,1,2,2},{1,0,1,2,2},{1,0,1,2,2},{0,1,0,2,2}}, // v
	{{1,0,1,0,1},{1,0,1,0,1},{1,0,1,0,1},{0,1,0,1,0}}, // w
	{{1,0,1,2,2},{0,1,0,2,2},{0,1,0,2,2},{1,0,1,2,2}}, // x
	{{1,0,1,2,2},{1,0,1,2,2},{0,1,0,2,2},{0,1,0,2,2}}, // y
	{{1,1,1,2,2},{0,0,1,2,2},{1,1,0,2,2},{1,1,1,2,2}}, // z
	{{0,0,0,2,2},{0,0,0,2,2},{0,1,0,2,2},{0,0,0,2,2}}, // .
	{{0,0,0,2,2},{0,0,0,2,2},{0,1,0,2,2},{1,0,0,2,2}}, // ,
	{{1,0,1,2,2},{1,0,1,2,2},{0,0,0,2,2},{0,0,0,2,2}}, // "
	{{0,1,0,2,2},{0,0,0,2,2},{0,1,0,2,2},{0,0,0,2,2}}, // :
	{{0,1,0,2,2},{0,0,0,2,2},{0,1,0,2,2},{1,0,0,2,2}}, // ;
	{{1,1,2,2,2},{1,0,2,2,2},{1,0,2,2,2},{1,1,2,2,2}}, // [
	{{1,1,2,2,2},{0,1,2,2,2},{0,1,2,2,2},{1,1,2,2,2}}, // ]
	{{0,1,2,2,2},{1,0,2,2,2},{1,0,2,2,2},{0,1,2,2,2}}, // (
	{{1,0,2,2,2},{0,1,2,2,2},{0,1,2,2,2},{1,0,2,2,2}}, // )
	{{0,0,1,2,2},{0,1,0,2,2},{0,1,0,2,2},{1,0,0,2,2}}, // /
	{{1,0,0,2,2},{0,1,0,2,2},{0,1,0,2,2},{0,0,1,2,2}}, // \
	{{1,1,1,2,2},{0,0,0,2,2},{1,1,1,2,2},{0,0,0,2,2}}, // =
	{{0,0,0,2,2},{1,1,1,2,2},{0,0,0,2,2},{0,0,0,2,2}}, // -
	{{0,1,0,2,2},{1,1,1,2,2},{0,1,0,2,2},{0,0,0,2,2}}, // +
	{{1,0,1,2,2},{0,1,0,2,2},{1,0,1,2,2},{0,0,0,2,2}}  // *
};
