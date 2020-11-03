#include <vector>
#include "std_utils.h"
using namespace std;
// #include <locale>
// #include <codecvt>

const vector<stdstring> split(const stdstring& s, stdchar delim) {
    stdstringstream wss(s);
	stdstring item;
	vector<stdstring> elems;
	while (getline(wss, item, delim))
		elems.push_back(item);

	return elems;
}
stdstring join(const vector<stdstring>& arr, stdchar delim) {
    stdstring result;
    for (auto it = arr.begin(); it < arr.end(); it++) {
        if (!result.empty())
            result += delim;
        result.append(*it);
    }
    return result;
}

int findString(stdstring const& text, stdstring const& searchText) {
    auto it = search(text.begin(), text.end(),
        searchText.begin(), searchText.end(),
        [](auto ch1, auto ch2) { return toupper(ch1) == toupper(ch2); }
    );
	auto pos = static_cast<int>(it - text.begin());
	return pos < (int)text.length() ? pos : -1;
}

bool ends_with(stdstring const& value, stdstring const& ending)
{
    if (ending.size() > value.size())
        return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin(), [](stdchar a, stdchar b){ return tolower(a) == tolower(b); });
}

