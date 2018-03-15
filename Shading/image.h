// -*-C++-*-

#pragma once

#include <string>
#include <vector>
#include <map>

// this class acts as a manager also

class Image {
public:

	void print() const;

	const std::string &getName() const;
	int getWidth() const;
	int getHeight() const;
	size_t getSize() const;
	const unsigned char *getData() const;

	friend class ImageManager;

private:
	Image(const std::string &fname);
	~Image();
	Image(const Image &);
	Image & operator =(const Image &);
	void load_jpg();

	std::string m_fileName; // The image path
	int m_width;      // (must be power of 2)
	int m_height;     // (must be power of 2)
	size_t m_size;
	std::vector<unsigned char> m_data;
};
