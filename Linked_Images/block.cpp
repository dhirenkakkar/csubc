#include "block.h"

void Block::build(PNG & im, int column, int width) {
	data.resize(width);
	for (int i = 0; i < width; i++) {
		unsigned int c = column + i;
		vector <HSLAPixel> v(im.height());
		for (unsigned int j = 0; j < im.height(); j++) {
			v[j] = *im.getPixel(c,j);
		}
		data[i] = v;
	}
}

void Block::render(PNG & im, int column) const {
	int w = width();
	int h = height();
	for (int i = 0; i < w; i++) {
		int c = column + i;
		for (int j = 0; j < h; j++) {
			*im.getPixel(c,j) = data[i][j];
		}
	}
}

void Block::greyscale() {
	int w = width();
	int h = height();
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			data[i][j].s = 0;
		}
	}
}

int Block::width() const {
	return data.size();
}

int Block::height() const {
	return data[0].size();
}