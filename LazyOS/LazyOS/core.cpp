#include "core.h"
#include <chrono>
#include "utils.h"

/*
	0xD  - dir
	0x5D - system dir
	0xF  - file
	0x5F - system file
	*/

std::string file_to_filename(LazyOS::directory_file& file) {
	std::string tmp;
	tmp.append(file.filename);
	if (strlen(file.extension) != 0) {
		tmp.append(".");
		tmp.append(file.extension);
	}
	return tmp;
}

int create_file(int inode_number, LazyOS::inode& inode, LazyOS::inode& new_inode, std::vector<std::string>& dirs, int k) {
	char buf[512];
	//insert file
	if (k == dirs.size() - 2) {
		auto now = std::chrono::system_clock::now();
		uint32_t free_inode = GV::os.get_free_inode();

		LazyOS::directory_file files[8];
		LazyOS::directory_file file(free_inode, dirs[dirs.size() - 2]);
		GV::os.read_block_indirect(inode, inode.size / 512, buf);

		memcpy(files, buf, 512);
		for (int i = 0; i < 8; i++)
			if (file_to_filename(files[i]) == dirs[dirs.size() - 2]) {
				return 0;
			}
		files[inode.size / 64] = file;
		memcpy(buf, files, 512);

		inode.size += 64;

		GV::os.write_block_indirect(inode, inode.size / 512, buf);
		GV::os.write_inode(inode_number, inode);


		GV::os.write_inode(free_inode, new_inode);
		GV::os.set_bit(free_inode + 256, 0x1);

		return 1;
	}
	//search directory to insert
	for (int i = 0; i < inode.size / 64; i++) {
		if (i % 8 == 0)
			GV::os.read_block_indirect(inode, i/8, buf);
		LazyOS::directory_file file;
		memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		if (dirs[k] == file_to_filename(file)) {
			LazyOS::inode temp = GV::os.read_inode(file.n_inode);
			return create_file(file.n_inode, temp, new_inode, dirs, k + 1);
		}
		continue;
	}
	return 0;
}
int core::fcreate(std::string filename)
{
	auto now = std::chrono::system_clock::now();
	auto dirs = util::split(filename, '/');
	if (dirs[0] == "" && dirs[1] == "" && dirs.size() == 2) { //root
		LazyOS::inode inode = {0};//20000
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
		LazyOS::inode n_inode = { 0 };
		n_inode.mode = util::write_first_4_bits(n_inode.mode, 0xF);
		n_inode.mode = util::write_rwxrwxrwx(n_inode.mode, 0000);
		n_inode.date_creation = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		n_inode.date_modification = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		LazyOS::inode root = GV::os.read_inode(GV::os.superblock.root_inode);
		dirs.push_back("");
		return create_file(0, root, n_inode, dirs, 1);
	}
	else if (dirs[dirs.size() - 1] == "" && dirs.size() > 2) { //dir

		LazyOS::inode n_inode = { 0 };
		n_inode.mode = util::write_first_4_bits(n_inode.mode, 0xD);
		n_inode.mode = util::write_rwxrwxrwx(n_inode.mode, 0000);
		n_inode.date_creation = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		n_inode.date_modification = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

		LazyOS::inode root = GV::os.read_inode(GV::os.superblock.root_inode);

		return create_file(0, root, n_inode, dirs, 1);
	}

	return 0;
}

int open_file(int inode_number, LazyOS::inode& inode, std::vector<std::string>& dirs, int k) {
	char buf[512];

	for (int i = 0; i < inode.size / 64; i++) {
		if (i % 8 == 0)
			GV::os.read_block_indirect(inode, i/8, buf);
		LazyOS::directory_file file;
		memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		if (dirs[k] == file_to_filename(file)) {
			if (k == dirs.size() - 2) {
				return file.n_inode;
			}
			LazyOS::inode temp = GV::os.read_inode(file.n_inode);
			return open_file(file.n_inode, temp, dirs, k + 1);
		}
		continue;
	}
	return inode_number;
}
int core::fopen(std::string filename)
{
	auto now = std::chrono::system_clock::now();
	auto dirs = util::split(filename, '/');

	LazyOS::inode root = GV::os.read_inode(GV::os.superblock.root_inode);

	if (dirs[dirs.size() - 1] != "" && dirs.size() > 1) {  //file
		dirs.push_back("");
		return open_file(0, root, dirs, 1);
	}
	else if (dirs[dirs.size() - 1] == "" && dirs.size() > 2) { //dir
		return open_file(0, root, dirs, 1);
	}
	return 0;
}

int recursive_clear(int inode_number, LazyOS::inode& inode) {
	
	if (util::read_first_4_bits(inode.mode) == 0xF
		|| util::read_first_4_bits(inode.mode) == 0x5F) {
		GV::os.unlock_inode(inode_number);
		GV::os.free_inode(inode_number);
		return 0;
	}
	char buf[512];
	for (int i = 0; i < inode.size / 64; i++) {
		if (i % 8 == 0) {
			GV::os.read_block_indirect(inode, i / 8, buf);
		}
		LazyOS::directory_file file;
		memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		if (file.n_inode != 0) {
			LazyOS::inode temp = GV::os.read_inode(file.n_inode);
			recursive_clear(file.n_inode, temp);
		}
	}
	GV::os.free_inode(inode_number);
	GV::os.unlock_inode(inode_number);
	return 1;
}
int delete_file(int inode_number, LazyOS::inode& inode, std::vector<std::string>& dirs, int k) {
	char buf[512];
	//insert file
	if (k == dirs.size() - 2) {
		if (inode.size == 0)
			return 0;
		auto now = std::chrono::system_clock::now();

		//take last file
		GV::os.read_block_indirect(inode, inode.size/512, buf);
		LazyOS::directory_file files[8];
		memcpy(files, buf, 512);
		LazyOS::directory_file last_file;
		last_file = files[inode.size / 64 - 1];

		if (file_to_filename(last_file) == dirs[dirs.size() - 2]) {//is last file
			LazyOS::inode a = GV::os.read_inode(files[inode.size / 64 - 1].n_inode);
			recursive_clear(files[inode.size / 64 - 1].n_inode, a);
			files[inode.size / 64 - 1] = LazyOS::directory_file();
			memcpy(buf, files, 512);
			inode.size -= 64;
			
			GV::os.write_block_indirect(inode, inode.size / 512, buf);
			GV::os.write_inode(inode_number, inode);
			return 1;
		}
		else {//search file
			for (int i = 0; i < inode.size / 64; i++) {
				if (i % 8 == 0)
					GV::os.read_block_indirect(inode, i/8, buf);
				LazyOS::directory_file file;
				memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
				if (dirs[dirs.size() - 2] == file_to_filename(file)) { //swap files
					files[inode.size / 64 - 1] = LazyOS::directory_file();
					memcpy(buf, files, 512);
					GV::os.write_block_indirect(inode, inode.size / 512, buf);

					GV::os.read_block_indirect(inode, i / 8, buf);
					memcpy(files, buf, 512);
					
					LazyOS::inode a = GV::os.read_inode(files[i%8].n_inode);
					recursive_clear(files[i % 8].n_inode, a);

					files[i % 8] = last_file;
					memcpy(buf, files, 512);
					GV::os.write_block_indirect(inode, i / 8, buf);

					inode.size -= 64;
					GV::os.write_inode(inode_number, inode);
					return 1;
				}
				continue;
			}
		}

		return 0;
	}
	//search directory
	for (int i = 0; i < inode.size / 64; i++) {
		if (i % 8 == 0)
			GV::os.read_block_indirect(inode, i/8, buf);
		LazyOS::directory_file file;
		memcpy(&file, buf + i * sizeof(LazyOS::directory_file), sizeof(LazyOS::directory_file));
		if (dirs[k] == file_to_filename(file)) {
			LazyOS::inode temp = GV::os.read_inode(file.n_inode);
			return delete_file(file.n_inode, temp, dirs, k + 1);
		}
		continue;
	}
	return 0;
}
int core::fdelete(std::string filename)
{
	auto now = std::chrono::system_clock::now();
	auto dirs = util::split(filename, '/');

	LazyOS::inode root = GV::os.read_inode(GV::os.superblock.root_inode);

	if (dirs[dirs.size() - 1] != "" && dirs.size() > 1) {  //file
		dirs.push_back("");
		return delete_file(0, root, dirs, 1);
	}
	else if (dirs[dirs.size() - 1] == "" && dirs.size() > 2) { //dir
		return delete_file(0, root, dirs, 1);
	}
	return 0;
}

int core::fread()
{
	return 0;
}

int core::fwrite()
{
	return 0;
}

int core::fappend()
{
	return 0;
}

int core::fseek()
{
	return 0;
}

int core::fget_attributes()
{
	return 0;
}

int core::fset_attributes()
{
	return 0;
}

int core::frename()
{
	return 0;
}


