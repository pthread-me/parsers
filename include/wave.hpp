#include <utility.hpp>


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
	u32 ck_size{}; // includes size of wave_id and remaining chunks
  u32 wave_id{};
};

struct FmtChunk {
  u32								ck_id{}; 
	u32               ck_size{}; // includes size of all fields bellow it and remaining chunks
  FormatCode        fmt_code{};
  u16               num_channels{};
  u32               sample_rate{};
  u32               data_rate{};
  u16               sample_block_size{}; // sum size of Sample_x for all channels
  u16               bit_per_sample{};		// redundant: sample_block_size/num_channels
  u16               extension_size{};
  u16               valid_bits{};
  u32               speaker_pos{};
  std::array<u8,16>	GUID{};
};

struct FactChunk {
  u32 ck_id{};
  u32 ck_size{}; // is always 4
  u32 sample_per_channel{};
};

struct DataChunk {
  u32             ck_id{};
  u32             ck_size{};
  std::vector<u8> samples{};
};


class File{
	public:
		File(const std::string& file_path): riff{}, fmt{}, fact{}, data{} {
			init(file_path);
		}
	
		void load_from_file(const std::string& file_path);
		void save_to_file(const std::string& file_path);

  //private:
		RiffChunk riff;
    FmtChunk  fmt;
    FactChunk fact;
    DataChunk data;

		u64 num_samples; // deduced from struct values
		u64 per_chanel_sample_size;

    void init(const std::string& file_path);
    void load(std::ifstream& src_file);
    void save(std::ofstream& dst_file);
		

		void validate_riff() const;
		void validate_fmt() const;
		void validate_fact() const;
		void validate_data() const;
		
};
}




//-----------------------------------------------------------------------------
// CLASS SPECIALIZATIONS
//-----------------------------------------------------------------------------

template<>
struct std::formatter<wave::FormatCode>: std::formatter<std::string>{
  auto format(wave::FormatCode var, format_context& ctx) const{
    std::string name; 
    switch (var) {
      case wave::WAVE_FORMAT_PCM:
        name = "WAVE_FORMAT_PCM";
        break;
      case wave::WAVE_FORMAT_IEEE_FLOAT:
        name = "WAVE_FORMAT_IEEE_FLOAT";
        break;
      case wave::WAVE_FORMAT_ALAW:
        name = "WAVE_FORMAT_ALAW";
        break;
      case wave::WAVE_FORMAT_MULAW:
        name = "WAVE_FORMAT_MULAW";
        break;
      case wave::WAVE_FORMAT_EXTENSIBLE:
        name = "WAVE_FORMAT_EXTENSIBLE";
        break;
      default:
        throw std::runtime_error("Undefined wave format");
    }
    
    return std::formatter<std::string>::format(name, ctx);
  }
};



#endif
