#include <utility.hpp>
#include <wave.hpp>
#include <formatters.hpp>

#include <fstream>
#include <cassert>

//-----------------------------------------------------------------------------
// Public Interface
//-----------------------------------------------------------------------------

void wave::File::init(const std::string& file_path) {
  this->load_from_file(file_path);
}

void wave::File::load_from_file(const std::string& file_path) {
  std::ifstream fs(file_path, std::ios::binary | std::ios::in);
  if (!fs.is_open()) {
    log(std::cerr, "Can't open file", file_path);
    return;
  }

  this->load(fs);
  fs.close();
}

void wave::File::save_to_file(const std::string& file_path) {
  std::ofstream fs(file_path, std::ios::binary | std::ios::out);
  if (!fs.is_open()) {
    log(std::cerr, "Can't open file", file_path);
  }

  this->save(fs);
  fs.close();
}


//-----------------------------------------------------------------------------
// Private Functions
//-----------------------------------------------------------------------------

void wave::File::validate_riff() const {
  auto& chunk = this->riff;
  assert(field_to_str(chunk.ck_id) == "RIFF");
  assert(field_to_str(chunk.wave_id) == "WAVE");
}

void wave::File::validate_fmt() const {
  auto& chunk = this->fmt;
  assert(field_to_str(chunk.ck_id) == "fmt ");
  assert( (chunk.ck_size==16) | (chunk.ck_size==18) | (chunk.ck_size==40) );
}

void wave::File::validate_fact() const {
  auto& chunk = this->fact;
  assert(field_to_str(chunk.ck_id) == "fact");
  assert(chunk.ck_size == 4);
  assert(chunk.sample_per_channel == this->num_samples);
}

void wave::File::validate_data() const {
  auto& chunk = this->data;
  assert(field_to_str(chunk.ck_id) == "data");
}

void wave::File::load(std::ifstream& fs) {
  fs.seekg(0, fs.beg);
  u64 start = fs.tellg();
  fs.seekg(0, fs.end);
  u64 stream_size = static_cast<u64>(fs.tellg()) - start;

  if (stream_size < wave::MIN_SIZE) {
    log(std::cerr, "File is: ", stream_size, " bytes. Way too small to hold data.");
    return;
  }
  fs.seekg(0, fs.beg);

	//bit_cast would result in a copy, which we dont want
  fs.read(reinterpret_cast<char*>(&this->riff.ck_id), sizeof(this->riff.ck_id));
  fs.read(reinterpret_cast<char*>(&this->riff.ck_size), sizeof(this->riff.ck_size));
  fs.read(reinterpret_cast<char*>(&this->riff.wave_id), sizeof(this->riff.wave_id));
  validate_riff();

  fs.read(reinterpret_cast<char*>(&this->fmt.ck_id), sizeof(this->fmt.ck_id));
  fs.read(reinterpret_cast<char*>(&this->fmt.ck_size), sizeof(this->fmt.ck_size));
  fs.read(reinterpret_cast<char*>(&this->fmt.fmt_code), sizeof(this->fmt.fmt_code));
  fs.read(reinterpret_cast<char*>(&this->fmt.num_channels), sizeof(this->fmt.num_channels));
  fs.read(reinterpret_cast<char*>(&this->fmt.sample_rate), sizeof(this->fmt.sample_rate));
  fs.read(reinterpret_cast<char*>(&this->fmt.data_rate), sizeof(this->fmt.data_rate));
  fs.read(reinterpret_cast<char*>(&this->fmt.sample_block_size), sizeof(this->fmt.sample_block_size));
  fs.read(reinterpret_cast<char*>(&this->fmt.bit_per_sample), sizeof(this->fmt.bit_per_sample));
  this->per_chanel_sample_size = this->fmt.sample_block_size/this->fmt.num_channels;
  validate_fmt();

  // Read fmt extension if present
  switch (this->fmt.fmt_code) {
    case wave::WAVE_FORMAT_PCM:
      break;
    case wave::WAVE_FORMAT_IEEE_FLOAT:
    case wave::WAVE_FORMAT_ALAW:
    case wave::WAVE_FORMAT_MULAW:
      fs.read(reinterpret_cast<char*>(&this->fmt.extension_size), sizeof(this->fmt.extension_size));
      break;
    case wave::WAVE_FORMAT_EXTENSIBLE:
      fs.read(reinterpret_cast<char*>(&this->fmt.extension_size), sizeof(this->fmt.extension_size));
      fs.read(reinterpret_cast<char*>(&this->fmt.valid_bits), sizeof(this->fmt.valid_bits));
      fs.read(reinterpret_cast<char*>(&this->fmt.speaker_pos), sizeof(this->fmt.speaker_pos));
      fs.read(reinterpret_cast<char*>(&this->fmt.GUID), sizeof(this->fmt.GUID));
      break;
  }

  // Read Fact (if present) and data header
  switch(this->fmt.fmt_code) {
    case wave::WAVE_FORMAT_PCM:
			fs.read(reinterpret_cast<char*>(&this->data.ck_id), sizeof(this->data.ck_id));
			fs.read(reinterpret_cast<char*>(&this->data.ck_size), sizeof(this->data.ck_size));
			this->num_samples = (this->data.ck_size/(this->per_chanel_sample_size*this->fmt.num_channels));
			break;
    case wave::WAVE_FORMAT_IEEE_FLOAT:
    case wave::WAVE_FORMAT_ALAW:
    case wave::WAVE_FORMAT_MULAW:
    case wave::WAVE_FORMAT_EXTENSIBLE:
      fs.read(reinterpret_cast<char*>(&this->fact.ck_id), sizeof(this->fact.ck_id));
      fs.read(reinterpret_cast<char*>(&this->fact.ck_size), sizeof(this->fact.ck_size));
      fs.read(reinterpret_cast<char*>(&this->fact.sample_per_channel), sizeof(this->fact.sample_per_channel));
			fs.read(reinterpret_cast<char*>(&this->data.ck_id), sizeof(this->data.ck_id));
			fs.read(reinterpret_cast<char*>(&this->data.ck_size), sizeof(this->data.ck_size));
			this->num_samples = (this->data.ck_size/(this->per_chanel_sample_size*this->fmt.num_channels));
			validate_fact();
  }

	validate_data();
  this->data.samples.reserve(this->data.ck_size);
	fs.read(reinterpret_cast<char*>(this->data.samples.data()), static_cast<std::streamsize>(this->data.ck_size));
}


void wave::File::save(std::ofstream& fs) {
  fs.seekp(0, fs.beg);
  
  fs.write(reinterpret_cast<char*>(&this->riff.ck_id), sizeof(this->riff.ck_id));
  fs.write(reinterpret_cast<char*>(&this->riff.ck_size), sizeof(this->riff.ck_size));
  fs.write(reinterpret_cast<char*>(&this->riff.wave_id), sizeof(this->riff.wave_id));

  fs.write(reinterpret_cast<char*>(&this->fmt.ck_id), sizeof(this->fmt.ck_id));
  fs.write(reinterpret_cast<char*>(&this->fmt.ck_size), sizeof(this->fmt.ck_size));
  fs.write(reinterpret_cast<char*>(&this->fmt.fmt_code), sizeof(this->fmt.fmt_code));

  fs.write(reinterpret_cast<char*>(&this->fmt.num_channels), sizeof(this->fmt.num_channels));
  fs.write(reinterpret_cast<char*>(&this->fmt.sample_rate), sizeof(this->fmt.sample_rate));
  fs.write(reinterpret_cast<char*>(&this->fmt.data_rate), sizeof(this->fmt.data_rate));
  fs.write(reinterpret_cast<char*>(&this->fmt.sample_block_size), sizeof(this->fmt.sample_block_size));
  fs.write(reinterpret_cast<char*>(&this->fmt.bit_per_sample), sizeof(this->fmt.bit_per_sample));
    
  // Read fmt extension if present
  switch (this->fmt.fmt_code) {
    case wave::WAVE_FORMAT_PCM:
      break;
    case wave::WAVE_FORMAT_IEEE_FLOAT:
    case wave::WAVE_FORMAT_ALAW:
    case wave::WAVE_FORMAT_MULAW:
      fs.write(reinterpret_cast<char*>(&this->fmt.extension_size), sizeof(this->fmt.extension_size));
      break;
    case wave::WAVE_FORMAT_EXTENSIBLE:
      fs.write(reinterpret_cast<char*>(&this->fmt.extension_size), sizeof(this->fmt.extension_size));
      fs.write(reinterpret_cast<char*>(&this->fmt.valid_bits), sizeof(this->fmt.valid_bits));
      fs.write(reinterpret_cast<char*>(&this->fmt.speaker_pos), sizeof(this->fmt.speaker_pos));
      fs.write(reinterpret_cast<char*>(&this->fmt.GUID), sizeof(this->fmt.GUID));
      break;
  }

  switch(this->fmt.fmt_code) {
    case wave::WAVE_FORMAT_PCM:
			fs.write(reinterpret_cast<char*>(&this->data.ck_id), sizeof(this->data.ck_id));
			fs.write(reinterpret_cast<char*>(&this->data.ck_size), sizeof(this->data.ck_size));
			break;
    case wave::WAVE_FORMAT_IEEE_FLOAT:
    case wave::WAVE_FORMAT_ALAW:
    case wave::WAVE_FORMAT_MULAW:
    case wave::WAVE_FORMAT_EXTENSIBLE:
      fs.write(reinterpret_cast<char*>(&this->fact.ck_id), sizeof(this->fact.ck_id));
      fs.write(reinterpret_cast<char*>(&this->fact.ck_size), sizeof(this->fact.ck_size));
      fs.write(reinterpret_cast<char*>(&this->fact.sample_per_channel), sizeof(this->fact.sample_per_channel));
			fs.write(reinterpret_cast<char*>(&this->data.ck_id), sizeof(this->data.ck_id));
			fs.write(reinterpret_cast<char*>(&this->data.ck_size), sizeof(this->data.ck_size));
  }

	fs.write(reinterpret_cast<char*>(this->data.samples.data()),	this->data.ck_id);
  if (this->data.ck_size%2 == 1) {
    fs.write(0, sizeof(char));
  }
}
