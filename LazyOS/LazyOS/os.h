#pragma once
#define MB 1024*1024
using byte = unsigned char;
#include <fstream>

class LazyOS {
private:
	std::fstream file;
	int bitmap_size;
public:
	LazyOS();
	~LazyOS();
	int resize(int size, int size_claster);

	void set_bit_0(int bit_number);
	void set_bit_1(int bit_number);

	void set_bit(int bit_number, byte value = 1);
	int get_bit(int bit_number);

	int bios_read_sector(int sector_number, char* buf);
	int bios_write_sector(int sector_number, char* buf);
	//1 блок под суперблок
	struct inode {
		int id_node;
		int owner_id;
		int group_id;
		int rwx;
		int size;
		int date_creation;
		int date_modification;
		int flags;
	};
};