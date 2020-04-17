#include "PNGImage.h"
#include <fstream>
#include <iostream>


inline UINT getSmallEdian(UINT i) {
	return (i >> 24) | (i >> 8 & 0xFF00) | (i << 8 & 0xFF0000) | (i << 24 & 0xFF000000);
}

PNGImage::PNGImage(UINT width, UINT height, PNG_COLOR_SPACE colorType) :
	width(width), height(height), ColorType(colorType) {
	data = new BYTE[width * height * 3];
}


int PNGImage::get_height() {
	return height;
}

int PNGImage::get_width() {
	return width;
}

Color PNGImage::get(int x, int y) {
	int base = (x + y * width) * 3;
	if (base < width * height * 3)
		return Color(data[base + 0], data[base + 1], data[base + 2], data[base + 3]);
	else
		return Color();
}

bool PNGImage::set(int x, int y, Color color) {
	int base = (x + y * width) * 3;
	if (base < width * height * 3) {
		data[base] = color.r;
		data[base + 1] = color.g;
		data[base + 2] = color.b;
		return true;
	}
	return false;
}

void PNGImage::writeWithCRC(std::ostream& out, BYTE b, UINT& c) {
	out << b;
	calcuCRC(c, b);
}

void PNGImage::writeWithCRC(std::ostream& out, const BYTE* b, UINT length, UINT& c) {
	for (int i = 0; i != length; i++) {
		calcuCRC(c,*(b + i));
	}
	out.write((char*)b,length);
}

void PNGImage::calcuCRC(UINT& c,BYTE b) {
	c ^= b;
	c = (c >> 4) ^ CRC_TABLE[c & 15];
	c = (c >> 4) ^ CRC_TABLE[c & 15];
}

UINT PNGImage::writeHead(std::ostream& out, CHUNK_HEADER header) {
	UINT c = 0xFFFFFFFF;
	out.write((char*)& header, 4);
	writeWithCRC(out, (BYTE*)& header + 4, 4, c);
	return c;
}

void PNGImage::save(const char* filename) {
	std::ofstream out;
	out.open(filename, std::ofstream::binary);
	if (!out.is_open()) {
		std::cerr << "file" << filename << "can not be opend";
		return;
	}
	out.write(PNG_HEAD, 8);
	IHDR_CHUNK ihdr = { 0 };
	ihdr.ChunkHeader.ChunkLength = getSmallEdian(13);
	ihdr.ChunkHeader.ChunkType = IHDR;
	ihdr.ColorType = (BYTE)ColorType;
	ihdr.Width = getSmallEdian(width);
	ihdr.Height = getSmallEdian(height);
	ihdr.Depth = 8;

	UINT c = writeHead(out, ihdr.ChunkHeader);
	writeWithCRC(out, (BYTE*)& ihdr + 8, 13, c);
	c = getSmallEdian(~c);
	out.write((char*)& c, 4);
	c = 0xFFFFFFFF;
	UINT a = 1, b = 0, linelen = 1 + width * 3,
		length = 2 + height * (linelen + 5) + 4;
	BYTE* temp = new BYTE[length], * p = temp;
	CHUNK_HEADER idat;
	idat.ChunkType = IDAT;
	idat.ChunkLength = getSmallEdian(length);
	c = writeHead(out, idat);
	*(p++) = 0x78, *(p++) = 0x01;
	for (UINT y = 0; y != height; y++) {
		*(p++) = (y == (height - 1));
		*(p++) = linelen & 0xFF;
		*(p++) = (linelen >> 8) & 0xFF;
		*(p++) = (~linelen) & 0xFF;
		*(p++) = ((~linelen) >> 8) & 0xFF;
		*(p++) = 0;
		a = a % 65521;
		b = (b + a) % 65521;
		for (UINT x = 0; x != width * 3; x++) {
			*(p++) = this->data[x + 3 * width * y];
			a = a % 65521;
			b = (b + a) % 65521;
		}
	}
	a = getSmallEdian((b << 16) | a);
	memcpy(p, &a, sizeof(UINT));

	writeWithCRC(out, temp, length, c);
	c = getSmallEdian(~c);
	out.write((char*)& c, 4);

	c = 0xFFFFFFFF;
	UINT zero = getSmallEdian(0);
	writeWithCRC(out, (const BYTE*)& IEND, 4, c);
	writeWithCRC(out, (const BYTE*)& zero, 4, c);
	c = getSmallEdian(~c);
	out.write((char*)& c, 4);

	out.close();
	delete[] temp;
}

void PNGImage::flip_vertically() {
	UINT halfy = height / 2;
	for (UINT x = 0; x != width; x++) {
		for (UINT y = 0; y != halfy; y++) {
			int v1 = (x + y * width) * 3;
			int v2 = (x + (height - y - 1) * width) * 3;
			std::swap(data[v1], data[v2]);
			std::swap(data[v1 + 1], data[v2 + 1]);
			std::swap(data[v1 + 2], data[v2 + 2]);
		}
	}
}
