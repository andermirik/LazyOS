#include "os.h"
#include "utils.h"
#include <fstream>

using std::string;


LazyOS::LazyOS()
{
	file.open("lazy.dat", std::ios::in | std::ios::out | std::ios::binary);
	bitmap_size = 512 << 20 / 512;
}

LazyOS::~LazyOS()
{
	file.close();
}

int LazyOS::resize(int size, int size_claster)
{
	file.close();
	std::remove("lazy.dat");
	file.open("lazy.dat", std::ios::out | std::ios::binary);
	file.seekp(size - 1);
	file.write("", 1);
	file.close();
	file.open("lazy.dat", std::ios::in | std::ios::out | std::ios::binary);

	this->bitmap_size = size/size_claster;

	return size / size_claster;
	return -1;
}



void LazyOS::set_bit(int n, byte value)
{
	if (value == 0)
		set_bit_0(n);
	else 
		set_bit_1(n);
}

void LazyOS::set_bit_1(int bit_number)
{
	/*
	char buf[1];
	file.seekg(n >> 3);
	file.read(buf, 1);
	buf[0] |= 1 << (n % 8);
	file.seekp(n >> 3);
	file.write(buf, 1);
	*/
	static char buf[512];
	bios_read_sector(bit_number >> 9, buf);
	buf[bit_number & 511] |= 1 << (bit_number & 7);
	bios_write_sector(bit_number >> 9, buf);
}

void LazyOS::set_bit_0(int bit_number)
{
	/*char buf[1];
	file.seekg(n >> 3);
	file.read(buf, 1);
	buf[0] &= ~(1<<(n%8));
	file.seekp(n >> 3);
	file.write(buf, 1);
	*/
	static char buf[512];
	bios_read_sector(bit_number >> 9, buf);
	buf[bit_number & 511] &= ~(1 << (bit_number & 7));
	bios_write_sector(bit_number >> 9, buf);
}

int LazyOS::get_bit(int bit_number)
{
	static char buf[512];
	bios_read_sector(bit_number >> 9, buf);
	return (buf[bit_number & 511] >> (bit_number & 7)) & 1;
	/*
	char buf[1];
	file.seekg(n>>3);
	file.read(buf, 1);
	return (buf[0] >> (n & 7)) & 1;
	*/
}

int LazyOS::bios_read_sector(int sector_number, char * buf)
{
	file.seekg(sector_number << 9);
	file.read(buf, 512);
	return 0;
}

int LazyOS::bios_write_sector(int sector_number, char * buf)
{
	file.seekp(sector_number << 9);
	file.write(buf, 512);
	return 0;
}
