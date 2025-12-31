#include <algorithm>
#include <array>
#include <cassert>
#include <constants.hpp>
#include <cstring>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <wave.hpp>
#include <formatters.hpp>

void wave::File::init(const std::string& file_path){
  std::ifstream fs(file_path, std::ios::binary | std::ios::in);
  if (!fs.is_open()){
    log(std::cerr, "Cant open file", file_path);
    return;
  }
  this->load(fs);
}

void wave::File::load(std::ifstream& fs){
  auto strm = fs.rdbuf();
  std::streamsize strm_size = strm->pubseekoff(0, fs.end);
  strm->pubseekoff(0, fs.beg);
  if (static_cast<u64>(strm_size) < wave::MIN_SIZE){
    log(std::cerr, "File is: ", strm_size, " bytes. Way too small to hold data.");
    return;
  }
  
	//bit_cast would result in a copy, which we dont want
  strm->sgetn(reinterpret_cast<char*>(this->riff.ck_id),         sizeof(this->riff.ck_id));

  strm->sgetn(reinterpret_cast<char*>(this->riff.ck_size),       sizeof(this->riff.ck_size));
  strm->sgetn(reinterpret_cast<char*>(this->riff.wave_id),       sizeof(this->riff.wave_id));
  strm->sgetn(reinterpret_cast<char*>(this->fmt.ck_id),          sizeof(this->fmt.ck_id));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.ck_size)),            sizeof(this->fmt.ck_size));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.fmt_code)),           sizeof(this->fmt.fmt_code));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.num_channels)),       sizeof(this->fmt.num_channels));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.sample_rate)),        sizeof(this->fmt.sample_rate));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.data_rate)),          sizeof(this->fmt.data_rate));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.sample_block_size)),	sizeof(this->fmt.sample_block_size));
  strm->sgetn(reinterpret_cast<char*>(&(this->fmt.bit_per_sample)),	    sizeof(this->fmt.bit_per_sample));

  switch (this->fmt.fmt_code){
    case wave::WAVE_FORMAT_PCM:
      break;

    case wave::WAVE_FORMAT_IEEE_FLOAT:
    case wave::WAVE_FORMAT_ALAW:
    case wave::WAVE_FORMAT_MULAW:

      break;
    case wave::WAVE_FORMAT_EXTENSIBLE:
      break;
  }


}
