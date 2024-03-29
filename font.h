/*
 * font.h
 *
 *  Created on: 12.11.2021
 *      Author: Administrator
 */

#ifndef FONT_H_
#define FONT_H_

//struct symbol {
//	union {
//		struct symbol_content {
//			unsigned int sym: 54;
//			unsigned int pad: 10;
//		} content;
//		unsigned int as_arr[4];
//		long long unsigned int a;
//	};
//};
//
//symbol font [10] = {
//		//0
//
//		// 011110	1
//		// 100001	2
//		// 100001	3
//		// 100011	4
//		// 100101	5
//		// 101001	6
//		// 110001	7
//		// 100001	8
//		// 011110	9
//
//		// 	0111 1010 0001 1000 	0110 0011 1001 0110 	1001 1100 0110 0001 	0111 10 00 0000 0000
//		{	0x7a18, 				0x6396, 				0x9a61, 				0x7800},
//
//		//1
//
//		// 000110	1
//		// 001010	2
//		// 000010	3
//		// 000010	4
//		// 000010	5
//		// 000010	6
//		// 000010	7
//		// 000010	8
//		// 111111	9
//
//		//	0001 1000 1010 0000 	1000 0010 0000 1000 	0010 0000 1000 0010 	1111 11 00 0000 0000
//		{	0x18a0, 				0x8208, 				0x2082, 				0xfc00},
//
//		// 2
//
//		// 001110	1
//		// 010001	2
//		// 010001	3
//		// 000010	4
//		// 000100	5
//		// 001000	6
//		// 010000	7
//		// 100000	8
//		// 111111	9
//
//		// 	0011 1001 0001 0100 	0100 0010 0001 0000 	1000 0100 0010 0000 	1111 11 00 0000 0000
//		{	0x3914, 				0x4210, 				0x8420, 				0xfc00},
//
//		// 3
//
//		// 011111	1
//		// 001001	2
//		// 010010	3
//		// 000100	4
//		// 001100	5
//		// 000011	6
//		// 000001	7
//		// 100110	8
//		// 111000	9
//
//
//		// 	0111 1100 1001 0100 	1000 0100 0011 0000 	0011 0000 0110 0110 	1110 00 00 0000 0000
//		{	0x7c94, 				0x8430, 				0x3066, 				0xe000},
//
//		// 4
//
//		// 010000	1
//		// 010000	2
//		// 100000	3
//		// 100010	4
//		// 100010	5
//		// 111111	6
//		// 000010	7
//		// 000010	8
//		// 000111	9
//
//		// 	0100 0001 0000 1000 	0010 0010 1000 1011 	1111 0000 1000 0010 	0001 11 00 0000 0000
//		{	0x4108, 				0x228b, 				0xf082, 				0x1c00},
//
//		// 5
//
//		// 011110	1
//		// 010000	2
//		// 100000	3
//		// 111000	4
//		// 100110	5
//		// 000011	6
//		// 000001	7
//		// 100011	8
//		// 011110	9
//
//		// 	0111 1001 0000 1000 	0011 1000 1001 1000 	0011 0000 0110 0011 	0111 10 00 0000 0000
//		{	0x7908, 				0x3898, 				0x3063, 				0x7800},
//
//		// 6
//
//		// 001110	1
//		// 011001	2
//		// 110000	3
//		// 100000	4
//		// 101110	5
//		// 110001	6
//		// 100001	7
//		// 010011	8
//		// 001110	9
//
//		// 	0011 1001 1001 1100 	0010 0000 1011 1011 	0001 1000 0101 0011 	0011 10 00 0000 0000
//		{	0x399a, 				0x20dd, 				0x1853, 				0x3800},
//
//		// 7
//
//		// 111111	1
//		// 010001	2
//		// 000001	3
//		// 000010	4
//		// 000010	5
//		// 000100	6
//		// 000100	7
//		// 001000	8
//		// 001000	9
//
//		// 	1111 1101 0001 0000 	0100 0010 0000 1000 	0100 0001 0000 1000 	0010 00 00 0000 0000
//		{	0xfd10, 				0x4208, 				0x4108, 				0x2000},
//
//		// 8
//
//		// 001100	1
//		// 010010	2
//		// 010010	3
//		// 001100	4
//		// 010010	5
//		// 100001	6
//		// 100001	7
//		// 100001	8
//		// 011110	9
//
//		// 	0011 0001 0010 0100 	1000 1100 0100 1010 	0001 1000 0110 0001 	0111 10 00 0000 0000
//		{	0x3124, 				0x8c4a, 				0x1861, 				0x7800},
//
//		// 9
//
//		// 001110	1
//		// 010001	2
//		// 100001	3
//		// 100001	4
//		// 011110	5
//		// 000010	6
//		// 000100	7
//		// 001000	8
//		// 110000	9
//
//		// 	0011 1001 0001 1000 	0110 0001 0111 1000 	0010 0001 0000 1000 	1100 00 00 0000 0000
//		{	0x3918, 				0x6178, 				0x2108, 				0xc000}
//};


unsigned char _font [15][12] = {
		//0

		// 011110	1
		// 100001	2
		// 100001	3
		// 100011	4
		// 100101	5
		// 101001	6
		// 110001	7
		// 100001	8
		// 011110	9

		{	0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
			0xfe, 0x05, 0x09, 0x11, 0x21, 0xfe},

		//1

		// 000110	1
		// 001010	2
		// 000010	3
		// 000010	4
		// 000010	5
		// 000010	6
		// 000010	7
		// 000010	8
		// 111111	9

		{	0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
			0x01, 0x01, 0x81, 0x01, 0xff, 0x01},

		// 2

		// 001110	1
		// 010001	2
		// 010001	3
		// 000010	4
		// 000100	5
		// 001000	6
		// 010000	7
		// 100000	8
		// 111111	9

		{	0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
			0x03, 0xc5, 0x09, 0x11, 0x21, 0xc1},

		// 3

		// 011111	1
		// 001001	2
		// 000010	3
		// 000100	4
		// 001100	5
		// 000011	6
		// 000001	7
		// 100110	8
		// 111000	9

		{	0x00, 0x01, 0x01, 0x01, 0x01, 0x01,
			0x03, 0x01, 0x91, 0x32, 0x4a, 0x8c},

		// 4

		// 010000	1
		// 010000	2
		// 100000	3
		// 100010	4
		// 100010	5
		// 111111	6
		// 000010	7
		// 000010	8
		// 000111	9

		{	0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
			0x78, 0x88, 0x08, 0x09, 0x3f, 0x09},

		// 5

		// 011110	1
		// 010000	2
		// 100000	3
		// 111000	4
		// 100110	5
		// 000011	6
		// 000001	7
		// 100011	8
		// 011110	9

		{	0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
			0x72, 0xa1, 0x21, 0x11, 0x1b, 0x0e},

		// 6

		// 001110	1
		// 011001	2
		// 110000	3
		// 100000	4
		// 101110	5
		// 110001	6
		// 100001	7
		// 010011	8
		// 001110	9

		{	0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
			0x7c, 0xca, 0x91, 0x11, 0x13, 0x8e},

		// 7

		// 111111	1
		// 010001	2
		// 000001	3
		// 000010	4
		// 000010	5
		// 000100	6
		// 000100	7
		// 001000	8
		// 001000	9

		{	0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
			0x00, 0x80, 0x03, 0x0c, 0x30, 0xc0},

		// 8

		// 001100	1
		// 010010	2
		// 010010	3
		// 001100	4
		// 010010	5
		// 100001	6
		// 100001	7
		// 100001	8
		// 011110	9

		{	0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
			0x0e, 0xd1, 0x21, 0x21, 0xd1, 0x0e},

		// 9

		// 001110	1
		// 010001	2
		// 100001	3
		// 100001	4
		// 011110	5
		// 000010	6
		// 000100	7
		// 001000	8
		// 110000	9

		{	0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
			0x61, 0x91, 0x12, 0x14, 0x18, 0xe0},

		// +


		// 000000	1
		// 001000	2
		// 001000	3
		// 001000	4
		// 111110	5
		// 001000	6
		// 001000	7
		// 001000	8
		// 000000	9

		{	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x10, 0x10, 0xfe, 0x10, 0x10, 0x00},

		// -


		// 000000	1
		// 000000	2
		// 000000	3
		// 000000	4
		// 000000	5
		// 111111	6
		// 000000	7
		// 000000	8
		// 000000	9

		{	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x08, 0x08, 0x08, 0x08, 0x08, 0x08},

		// (space)


		// 000000	1
		// 000000	2
		// 000000	3
		// 000000	4
		// 000000	5
		// 000000	6
		// 000000	7
		// 000000	8
		// 000000	9

		{	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// (degree)


		// 001100	1
		// 010010	2
		// 010010	3
		// 001100	4
		// 000000	5
		// 000000	6
		// 000000	7
		// 000000	8
		// 000000	9

		{	0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
			0x00, 0xc0, 0x20, 0x20, 0xc0, 0x00},

		// . (dot)


		// 000000	1
		// 000000	2
		// 000000	3
		// 000000	4
		// 000000	5
		// 000000	6
		// 000000	7
		// 001100	8
		// 001100	9

		{	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x03, 0x03, 0x00, 0x00}
};

#endif /* FONT_H_ */
