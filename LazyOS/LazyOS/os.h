#pragma once
#define MB 1024*1024
using byte = unsigned char;
#include <fstream>

class LazyOS {
	struct inode;
	struct super_block;
	struct directory_file;
private:
	std::fstream file;
public:
	LazyOS();
	~LazyOS();
	int resize(int size, int size_claster);

	void set_bit_0(int bit_number);
	void set_bit_1(int bit_number);

	void set_bit(int bit_number, byte value = 1, int offset = 4096);
	int get_bit(int bit_number, int offset = 4096);

	int bios_read_sector(int sector_number, char buf[512]);
	int bios_write_sector(int sector_number, char buf[512]);

	void write_inode(int inode_number, inode& inode);
	inode read_inode(int inode_number);

	void write_super_block(super_block& inode);
	super_block read_super_block();

	uint32_t get_free_block();

	void load_root();

	struct super_block {
		uint32_t magic;            //является ли LazyOS
		uint32_t block_size;       //размер блока
		uint32_t root_inode;       //инод рута
		uint32_t inode_blocks;     //количество инодов
		uint32_t bitmap_size;      //размер bitmap в байтах
		uint32_t inodes_in_block;
	};

	super_block superblock;

	struct inode {
		                     
		/* 
		[
			|type|     тип файла. 4 бита
			|u|        suid 
			|g|        sgid 
			|s|        sticky
			|r|w|x|    владелец
			|r|w|x|    группа
			|r|w|x|    остальные
		]
		*/
		uint16_t mode;
		uint16_t nlinks;
		uint32_t uid;
		uint32_t gid;
		uint32_t blocks[15];
		uint64_t size;
		uint64_t date_creation;
		uint64_t date_modification;
	};

	struct directory_file {
		uint32_t n_inode;
		char filename[54];
		char extension[6];
	};

	directory_file root[128];      //128 файлов

};

/*
sb: [0;0]
bm: [1;256]
id: [257;4006]
/:  [4007, 4022]
*/