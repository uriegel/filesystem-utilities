#include "exif_reader.h"
#include <vector>
#include <fstream>
#include <map>
#include <chrono>
#include <iomanip>
#include "cancellation.h"
using namespace std;

enum class Exif_tag {
	// IFD0 items
	ImageWidth = 0x100,
	ImageLength = 0x101,
	BitsPerSample = 0x102,
	Compression = 0x103,
	PhotometricInterpretation = 0x106,
	ImageDescription = 0x10E,
	Make = 0x10F,
	Model = 0x110,
	StripOffsets = 0x111,
	Orientation = 0x112,
	SamplesPerPixel = 0x115,
	RowsPerStrip = 0x116,
	StripByteCounts = 0x117,
	XResolution = 0x11A,
	YResolution = 0x11B,
	PlanarConfiguration = 0x11C,
	ResolutionUnit = 0x128,
	TransferFunction = 0x12D,
	Software = 0x131,
	DateTime = 0x132,
	Artist = 0x13B,
	WhitePoint = 0x13E,
	PrimaryChromaticities = 0x13F,
	JPEGInterchangeFormat = 0x201,
	JPEGInterchangeFormatLength = 0x202,
	YCbCrCoefficients = 0x211,
	YCbCrSubSampling = 0x212,
	YCbCrPositioning = 0x213,
	ReferenceBlackWhite = 0x214,
	Copyright = 0x8298,

	// SubIFD items
	ExposureTime = 0x829A,
	FNumber = 0x829D,
	SubIFD = 0x8769,
	ExposureProgram = 0x8822,
	SpectralSensitivity = 0x8824,
	GpsIFD = 0x8825,
	ISOSpeedRatings = 0x8827,
	OECF = 0x8828,
	ExifVersion = 0x9000,
	DateTimeOriginal = 0x9003,
	DateTimeDigitized = 0x9004,
	ComponentsConfiguration = 0x9101,
	CompressedBitsPerPixel = 0x9102,
	ShutterSpeedValue = 0x9201,
	ApertureValue = 0x9202,
	BrightnessValue = 0x9203,
	ExposureBiasValue = 0x9204,
	MaxApertureValue = 0x9205,
	SubjectDistance = 0x9206,
	MeteringMode = 0x9207,
	LightSource = 0x9208,
	Flash = 0x9209,
	FocalLength = 0x920A,
	SubjectArea = 0x9214,
	MakerNote = 0x927C,
	UserComment = 0x9286,
	SubsecTime = 0x9290,
	SubsecTimeOriginal = 0x9291,
	SubsecTimeDigitized = 0x9292,
	FlashpixVersion = 0xA000,
	ColorSpace = 0xA001,
	PixelXDimension = 0xA002,
	PixelYDimension = 0xA003,
	RelatedSoundFile = 0xA004,
	FlashEnergy = 0xA20B,
	SpatialFrequencyResponse = 0xA20C,
	FocalPlaneXResolution = 0xA20E,
	FocalPlaneYResolution = 0xA20F,
	FocalPlaneResolutionUnit = 0xA210,
	SubjectLocation = 0xA214,
	ExposureIndex = 0xA215,
	SensingMethod = 0xA217,
	FileSource = 0xA300,
	SceneType = 0xA301,
	CFAPattern = 0xA302,
	CustomRendered = 0xA401,
	ExposureMode = 0xA402,
	WhiteBalance = 0xA403,
	DigitalZoomRatio = 0xA404,
	FocalLengthIn35mmFilm = 0xA405,
	SceneCaptureType = 0xA406,
	GainControl = 0xA407,
	Contrast = 0xA408,
	Saturation = 0xA409,
	Sharpness = 0xA40A,
	DeviceSettingDescription = 0xA40B,
	SubjectDistanceRange = 0xA40C,
	ImageUniqueID = 0xA420,

	// GPS subifd items
	GPSVersionID = 0x0,
	GPSLatitudeRef = 0x1,
	GPSLatitude = 0x2,
	GPSLongitudeRef = 0x3,
	GPSLongitude = 0x4,
	GPSAltitudeRef = 0x5,
	GPSAltitude = 0x6,
	GPSTimeStamp = 0x7,
	GPSSatellites = 0x8,
	GPSStatus = 0x9,
	GPSMeasureMode = 0xA,
	GPSDOP = 0xB,
	GPSSpeedRef = 0xC,
	GPSSpeed = 0xD,
	GPSTrackRef = 0xE,
	GPSTrack = 0xF,
	GPSImgDirectionRef = 0x10,
	GPSImgDirection = 0x11,
	GPSMapDatum = 0x12,
	GPSDestLatitudeRef = 0x13,
	GPSDestLatitude = 0x14,
	GPSDestLongitudeRef = 0x15,
	GPSDestLongitude = 0x16,
	GPSDestBearingRef = 0x17,
	GPSDestBearing = 0x18,
	GPSDestDistanceRef = 0x19,
	GPSDestDistance = 0x1A,
	GPSProcessingMethod = 0x1B,
	GPSAreaInformation = 0x1C,
	GPSDateStamp = 0x1D,
	GPSDifferential = 0x1E
};

class Exif_reader
{
public:
	Exif_reader(const string& file) : exif_stream(ifstream(file, ios_base::binary)) { }
#if WINDOWS
	Exif_reader(const wstring& file) : exif_stream(ifstream(file, ios_base::binary)) { }
#endif	
	bool initialize();
	tuple<bool, int> get_tag_int(Exif_tag tag);
	tuple<bool, double> get_tag_rationale(Exif_tag tag);
	tuple<bool, string> get_tag_string(Exif_tag tag) {
		return get_tag_bytes(tag);
	}
private:
	bool read_to_exif_start();
	void catalogue_ifd();

	uint8_t read_byte();
	uint16_t read_ushort();
	uint32_t read_uint();
	uint32_t to_int(string data);
	uint16_t to_ushort(string data);
	string read_string(int length);
	string read_string(uint16_t tiff_offset, int length);
	tuple<bool, string> get_tag_bytes(Exif_tag tag);

	ifstream exif_stream;
	map<Exif_tag, int64_t> catalogue;
	int tiff_header_start{ 0 };
	bool is_little_endian{ false };
};

ExifInfo get_exif_info(const stdstring& file, int idx) {
	Exif_reader er(file);
	auto res = er.initialize();
	if (!res)
       	return ExifInfo {
			idx,
			0,
			0,
			0, 
		};

	bool success;
	string result;

	tie(success, result) = er.get_tag_string(Exif_tag::DateTimeOriginal);
	if (!success) 
		tie(success, result) = er.get_tag_string(Exif_tag::DateTime);
	uint64_t unix_time = 0;
	if (success)
	{
		tm tm = {};
		stringstream ss(result.c_str());
		ss >> get_time(&tm, "%Y:%m:%d %H:%M:%S");
		tm.tm_isdst = -1;
		auto time = mktime(&tm);

		auto secs = static_cast<chrono::seconds>(time).count();
		unix_time = static_cast<uint64_t>(secs)  * 1000;
	}
	double double_result;
	tie(success, double_result) = er.get_tag_rationale(Exif_tag::GPSLatitude);
	double latitude = 0;
	if (success)
		latitude = double_result;
	tie(success, double_result) = er.get_tag_rationale(Exif_tag::GPSLongitude);
	double longitude = 0;
	if (success)
		longitude = double_result;
	return ExifInfo {
		idx,
		unix_time,
		latitude,
		longitude, 
	};
}

uint64_t get_exif_date(const stdstring& file) {
	Exif_reader er(file);
	auto res = er.initialize();
	if (!res)
        return 0;
	bool success;
	string result;
	tie(success, result) = er.get_tag_string(Exif_tag::DateTimeOriginal);
	if (!success) 
		tie(success, result) = er.get_tag_string(Exif_tag::DateTime);
	if (!success) 
		return 0;
	
	tm tm = {};
	stringstream ss(result.c_str());
	ss >> get_time(&tm, "%Y:%m:%d %H:%M:%S");
	auto time = mktime(&tm);

	auto secs = static_cast<chrono::seconds>(time).count();
	auto unix_time = static_cast<uint64_t>(secs)  * 1000;
    return unix_time;
}

char get_tiff_field_length(uint16_t tiff_data_type);

static uint16_t swap16(uint16_t x) {
	return (uint16_t)((((x) >> 8) & 0xff) | (((x) & 0xff) << 8));
}

static uint32_t swap32(uint32_t x) {
	return (((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) | (((x) & 0x0000ff00u) << 8) | (((x) & 0x000000ffu) << 24);
}

bool Exif_reader::initialize() {
	if (!exif_stream)
		return false;
	if (read_ushort() != 0xFFD8)
		//throw new Exception("File is not a valid JPEG");
		return false;
	if (!read_to_exif_start())
		return false;

	read_ushort(); // auto sizeOfExifData = 

	auto exif = read_string(4);
	if (exif != "Exif")
		return false;

	if (read_ushort() != 0)
		//throw new Exception("Malformed Exif data");
		return false;

	tiff_header_start = static_cast<int>(exif_stream.tellg());

	// II for Intel, MM for Motorola
	auto ii = read_string(2);
	is_little_endian = ii == "II";

	if (read_ushort() != 0x002A)
		//throw new Exception("Error in TIFF data");
		return false;
	
	// offset image file directory
	auto ifd_offset = read_uint();

	exif_stream.seekg(ifd_offset + tiff_header_start);
	
	catalogue_ifd();

	bool success;
	int offset;
	tie(success, offset) = get_tag_int(Exif_tag::SubIFD);
	if (!success)
		// throw new Exception("Unable to locate Exif data");
		return false;

	exif_stream.seekg(offset + tiff_header_start);
	catalogue_ifd();

	tie(success, offset) = get_tag_int(Exif_tag::GpsIFD);
	if (success) {
		exif_stream.seekg(offset + tiff_header_start);
		catalogue_ifd();
	}
	
	return true;
}

bool Exif_reader::read_to_exif_start() {
	uint8_t marker_start;
	uint8_t marker_number{ 0 };

	while (((marker_start = read_byte()) == 0xFF) && (marker_number = read_byte()) != 0xE1)	{
		auto data_length = read_ushort();
		exif_stream.seekg(data_length - 2, istream::cur);
	}

	if (marker_start != 0xFF || marker_number != 0xE1)
		//throw new Exception("Could not find Exif data block");
		return false;
	return true;
}

void Exif_reader::catalogue_ifd() {
	auto count = read_ushort();

	for (auto current_entry = 0; current_entry < count; current_entry++) {
		auto current_tag_number = static_cast<Exif_tag>(read_ushort());

		catalogue[current_tag_number] = static_cast<int>(exif_stream.tellg()) - 2;
		exif_stream.seekg(10, ios_base::cur);
	}
}

uint8_t Exif_reader::read_byte() {
	char ret{ 0 };
	exif_stream.read(&ret, 1);
	return static_cast<uint8_t>(ret);
}

uint16_t Exif_reader::read_ushort() {
	uint16_t ret{ 0 };
	exif_stream.read(reinterpret_cast<char*>(&ret), 2);
	return is_little_endian ? ret : swap16(ret);
}

uint32_t Exif_reader::read_uint() {
	uint32_t  ret{ 0 };
	exif_stream.read(reinterpret_cast<char*>(&ret), 4);
	return is_little_endian ? ret : swap32(ret);
}

string Exif_reader::read_string(int length) {
	vector<char> bytes(length);
	exif_stream.read(bytes.data(), bytes.size());
	return string(bytes.data(), bytes.size());
}

tuple<bool, int> Exif_reader::get_tag_int(Exif_tag tag) {
	bool success;
	string result;
	std::tie(success, result) = get_tag_bytes(tag);
	if (!success)
		return make_tuple(false, 0);
	return make_tuple(true, to_int(result));
}

tuple<bool, double> Exif_reader::get_tag_rationale(Exif_tag tag){
	bool success;
	string result;
	tie(success, result) = get_tag_bytes(tag);
	if (!success)
		return make_tuple(false, 0);

    struct Rational {
        uint32_t num;
        uint32_t den;
    };

    Rational parts[3];
    memcpy(parts, result.data(), 24);

    auto to_double = [](const Rational& r) {
        return r.den ? static_cast<double>(r.num) / r.den : 0.0;
    };

    double degrees = to_double(parts[0]);
    double minutes = to_double(parts[1]);
    double seconds = to_double(parts[2]);

    // Convert to decimal degrees
    auto res = degrees + minutes / 60.0 + seconds / 3600.0;

	return make_tuple(true, res);
}

tuple<bool, string> Exif_reader::get_tag_bytes(Exif_tag tag) {
	if (catalogue.find(tag) == catalogue.end())
		return make_tuple(false, "");

	int offset = static_cast<int>(catalogue[tag]);
	exif_stream.seekg(offset);

	auto current_tag_id = static_cast<Exif_tag>(read_ushort());
	if (current_tag_id != tag)
		throw "Tag number not at expected offset";

	auto tiff_data_type = read_ushort();
	auto number_of_components = read_uint();
	auto tag_data = read_string(4);

	auto data_size = (int)(number_of_components * get_tiff_field_length(tiff_data_type));
	if (data_size > 4) {
		auto offset_address = to_ushort(tag_data);
		return make_tuple(true, read_string(offset_address, data_size));
	}
	return make_tuple(true, move(tag_data));
}

string Exif_reader::read_string(uint16_t tiff_offset, int length) {
	auto originalOffset = static_cast<int>(exif_stream.tellg());

	exif_stream.seekg(tiff_offset + tiff_header_start);
	auto result = read_string(length);

	exif_stream.seekg(originalOffset);

	return result;
}


uint32_t Exif_reader::to_int(string data) {
	if (!is_little_endian)
		reverse(data.begin(), data.end());

	return static_cast<uint32_t>(*(reinterpret_cast<const uint32_t*>(data.c_str())));
}

uint16_t Exif_reader::to_ushort(string data) {
	if (!is_little_endian)
		reverse(data.begin(), data.end());

	return static_cast<uint16_t>(*(reinterpret_cast<const uint16_t*>(data.c_str())));
}

char get_tiff_field_length(uint16_t tiff_data_type)
{
	switch (tiff_data_type)
	{
	case 1:
	case 2:
	case 6:
		return 1;
	case 3:
	case 8:
		return 2;
	case 4:
	case 7:
	case 9:
	case 11:
		return 4;
	case 5:
	case 10:
	case 12:
		return 8;
	default:
		throw "Unknown TIFF datatype";
	}
}

vector<ExifInfo> get_exif_infos(vector<ExifInfosInput>& input, stdstring cancellation) {
	vector<ExifInfo> output;

	register_cancellable(cancellation);

	for (auto& eii: input) {
		if (is_cancelled(cancellation))
			return vector<ExifInfo>();

		auto ret = get_exif_info(eii.path, eii.idx);
		if (ret.date != 0 || ret.latitude != 0 || ret.longitude != 0)
			output.push_back(ret);
	}

	unregister_cancellable(cancellation);

	return output;
}
