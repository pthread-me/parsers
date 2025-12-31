#include <constants.hpp>
#include <type_traits>
#include <vector>

#ifndef WAVE
#define WAVE

namespace wave{

template <typename T>
	concept Field = std::is_arithmetic_v<T> || std::is_array_v<T>;

enum FormatCode: u16{
	WAVE_FORMAT_PCM         = u16{0x0001},
	WAVE_FORMAT_IEEE_FLOAT  = u16{0x0003},
	WAVE_FORMAT_ALAW        = u16{0x0006},
	WAVE_FORMAT_MULAW       = u16{0x0007},
	WAVE_FORMAT_EXTENSIBLE  = u16{0xFFFE},
};

const static u64 MIN_SIZE = 44; 

struct RiffChunk {
  u32	ck_id{};
  u32 ck_size{};
  u32 wave_id{};
};

struct FmtChunk {
  u32								ck_id{}; 
	u32               ck_size{};
  FormatCode        fmt_code{};
  u16               num_channels{};
  u32               sample_rate{};
  u32               data_rate{};
  u16               sample_block_size{};
  u16               bit_per_sample{};		// redundant: sample_block_size/num_channels
  u16               extension_size{};
  u16               valid_bits{};
  u32               speaker_pos{};
  std::array<u8, 16>GUID{};
};

struct FactChunk {
  u32 ck_id{};
  u32 ck_size{};
  u32 sample_per_channel{};
};

struct DataChunk {
  u32             ck_id{};
  u32             ck_size{};
  std::vector<u8> samples{};
  bool            padding{}; // single byte if true
};



class File{
	public:
		File(const std::string& file_path): riff{}, fmt{}, fact{}, data{} {
			init(file_path);
		}
	
  private:
		RiffChunk riff;
    FmtChunk  fmt;
    FactChunk fact;
    DataChunk data;
		 

    void init(const std::string& file_path);
    void load(std::ifstream& src_file);
    void save(std::ostream& dst_file);
		
		
		template <Field T>
		bool validate_field(T, std::string);
};

}
#endif
