#include "os.h"
#include "utils.h"
#include <fstream>

using std::string;


LazyOS::LazyOS()
{
	file.open("lazy.dat", std::ios::in | std::ios::out | std::ios::binary);
	superblock = read_super_block();
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

	super_block sb;
	sb.magic = 0xED0DDCBA;//0xBADC0DED;
	sb.block_size = 512;
	sb.bitmap_size = size / size_claster/8;
	sb.inodes_in_block = 5;
	sb.inode_blocks = 20000;
	sb.root_inode = 0;

	write_super_block(sb);

	int i = 0;
	for (i = 0; i < 256 +1; i++) { //+1 ���� ����
		set_bit(i, 0x1);
	}

	return size / size_claster/8;
	return -1;
}



void LazyOS::set_bit(int n, byte value, int offset)
{
	if (value == 0)
		set_bit_0(n+offset);
	else 
		set_bit_1(n+offset);
}

void LazyOS::set_bit_1(int bit_number)
{
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	buf[(bit_number & 4095) >> 3] |= 1 << (bit_number & 7);
	bios_write_sector(bit_number >> 12, buf);
}

void LazyOS::set_bit_0(int bit_number)
{
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	buf[(bit_number & 4095) >> 3] &= ~(1 << (bit_number & 7));
	bios_write_sector(bit_number >> 12, buf);
}

int LazyOS::get_bit(int bit_number, int offset)
{
	bit_number += offset;
	static char buf[512];
	bios_read_sector(bit_number >> 12, buf);
	return (buf[(bit_number & 4095) >> 3] >> (bit_number & 7)) & 1;
}

int LazyOS::bios_read_sector(int sector_number, char buf[512])
{
	file.seekg(sector_number << 9);
	file.read(buf, 512);
	return 0;
}

int LazyOS::bios_write_sector(int sector_number, char buf[512])
{
	file.seekp(sector_number << 9);
	file.write(buf, 512);
	return 0;
}

//max = 20000
void LazyOS::write_inode(int inode_number, inode & input)
{
	file.seekp(256 * 512 + inode_number * sizeof(inode));
	file.write((char*)&input, sizeof(inode));
}

LazyOS::inode LazyOS::read_inode(int inode_number)
{
	inode result;
	file.seekg(256 * 512 + inode_number * sizeof(inode));
	file.read((char*)&result, sizeof(inode));
	return result;
}

void LazyOS::write_super_block(super_block & block)
{
	char buf[512];
	bios_read_sector(0, buf);
	memcpy(buf, &block, sizeof(super_block));
	bios_write_sector(0, buf);
}

LazyOS::super_block LazyOS::read_super_block()
{
	super_block result;
	char buf[512];
	bios_read_sector(0, buf);
	memcpy(&result, buf, sizeof(super_block));
	return result;
}

uint32_t LazyOS::get_free_block()
{
	for (int i = 4023; i < superblock.bitmap_size*8; i++) {
		if (get_bit(i) != 1) {
			return i;
		}
	}
	return 0;
}

void LazyOS::load_root()
{
	int t = 0;
	char buf[512];
	for (int clast = 4007; clast <= 4022; clast++) {
		bios_read_sector(clast, buf);
		for (int i = 0; i < 8; i++) {
			memcpy(&root[t++], buf + i * sizeof(directory_file), sizeof(directory_file));
		}
	}
}


