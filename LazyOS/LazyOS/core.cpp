#include "core.h"
#include <chrono>
#include "utils.h"
void readall(LazyOS::inode& inode, std::vector<std::string>& dirs, int i);
int core::fcreate(std::string filename)
{


	/*
	0xD  - dir
	0x5D - system dir
	0xF  - file
	0x5F - system file
	*/

	auto now = std::chrono::system_clock::now();
	auto dirs = util::split(filename, '/');
	if (dirs[0] == "" && dirs[1] == "" && dirs.size() == 2) { //root
		LazyOS::inode inode;//20000
		inode.mode = 0x5D;
		inode.date_creation = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		inode.date_modification = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		inode.size = 0;//8192
		for (int i = 0; i < 12; i++) {
			inode.blocks[i] = 4007 + i;
		}
		inode.blocks[12] = GV::os.get_free_block();
		GV::os.set_bit(inode.blocks[12], 0x1);
		inode.blocks[13] = 0;
		inode.blocks[14] = 0;
		uint32_t buf[144] = {0};
		buf[0] = 4019;
		buf[1] = 4020;
		buf[2] = 4021;
		buf[3] = 4022;
		GV::os.bios_write_sector(inode.blocks[12], (char*)buf);
		GV::os.write_inode(0, inode);
	}
	else if (dirs[dirs.size() - 1] != "" && dirs.size() > 1) {  //file
		auto root = GV::os.read_inode(GV::os.superblock.root_inode);
		LazyOS::directory_file files[128];

		char buf[512];
		int t = 0;
		for (int i = 0; i < 16; i++) {
			GV::os.read_block_indirect(root, i, buf);
			for(int j=0;j<8;j++)
				memcpy(&files[t++], buf + j* sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		}
		LazyOS::inode inode = {0};

		inode.mode = util::write_first_4_bits(inode.mode, 0xD);
		inode.mode = util::write_rwxrwxrwx(inode.mode, 0000);
		inode.date_creation = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		inode.date_modification = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		uint32_t free_inode = GV::os.get_free_inode();
		GV::os.write_inode(free_inode, inode);
		GV::os.set_bit(free_inode+256, 0x1);
	}
	else if (dirs[dirs.size() - 1] == "" && dirs.size() > 2) { //dir
		auto root = GV::os.read_inode(GV::os.superblock.root_inode);
		uint32_t free_inode = GV::os.get_free_inode();

		LazyOS::directory_file files[128];
		if (dirs.size() == 3) {
			char buf[512];
			int t = 0;
			for (int i = 0; i < 16; i++) {
				GV::os.read_block_indirect(root, i, buf);
				for (int j = 0; j < 8; j++) {
					memcpy(&files[t++], buf + j * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
					std::string tmp;
					tmp.append(files[t - 1].filename);
					if (strlen(files[t - 1].extension) != 0) {
						tmp.append(".");
						tmp.append(files[t - 1].extension);
					}
					if (tmp == dirs[dirs.size() - 2])
						return -1;
				}
			}

			LazyOS::directory_file file(free_inode, dirs[dirs.size() - 2]);
			files[root.size / 64] = file;
			root.size += sizeof(LazyOS::directory_file);
			GV::os.write_inode(0, root);

			t = 0;
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 8; j++)
					memcpy(buf + j * sizeof(LazyOS::directory_file), &files[t++], sizeof(LazyOS::directory_file));
				GV::os.write_block_indirect(root, i, buf);
			}
		}
		else {
			readall(root, dirs, 1);
			for (int i = 0; i < dirs.size() - 2; i++) {//пройти по каждой директории
				

				

			
			}
		}
		LazyOS::inode inode = {0};
		inode.mode = util::write_first_4_bits(inode.mode, 0xD);
		inode.mode = util::write_rwxrwxrwx(inode.mode, 0000);
		inode.date_creation = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		inode.date_modification = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		GV::os.write_inode(free_inode, inode);
		GV::os.set_bit(free_inode + 256, 0x1);
	}
	return 0;
}
void readall(LazyOS::inode& inode, std::vector<std::string>& dirs, int k) {
	char buf[512];
	LazyOS::directory_file file;
	
	if (k == dirs.size() - 2) {
		//запись
		return;
	}

	for (int i = 0; i < inode.size/64; i++) {
		if(i%8==0)
			GV::os.read_block_indirect(inode, i, buf);
		memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		if (strcmp(dirs[k].c_str(), file.filename) == 0) {
			inode = GV::os.read_inode(file.n_inode);
			readall(inode, dirs, k + 1);
		}
		continue;
	}
}