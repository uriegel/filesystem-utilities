#include <vector>
#include "../drives.h"
using namespace std;

vector<Drive_item> get_drives() {
	vector<Drive_item> result;

	// auto erase_it = remove_if(drive_items.begin(), drive_items.end(), [](const Drive_item & val) {
	// 	return !val.is_mounted;
	// 	});
	// drive_items.erase(erase_it, drive_items.end());
	return result;
}
