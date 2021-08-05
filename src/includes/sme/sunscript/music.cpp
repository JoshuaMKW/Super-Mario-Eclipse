#include "common.hxx"
#include "libs/sMusic.hxx"

using namespace SME::Class;

static AudioStreamer::AudioPacket sAudioPackets[AudioQueueSize] = {
    AudioStreamer::AudioPacket(-1), AudioStreamer::AudioPacket(-1),
    AudioStreamer::AudioPacket(-1), AudioStreamer::AudioPacket(-1)};
static s32 sAudioPacketIndex = 0;

void Spc::queueStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1,
             "Incorrect number of arguments passed to spc::queueStream (%lu "
             "args passed, 1 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  char *path = reinterpret_cast<char *>(Spc::Stack::popItem(interp).mValue);

  sAudioPackets[sAudioPacketIndex] = AudioStreamer::AudioPacket(path);
  streamer->queueAudio(sAudioPackets[sAudioPacketIndex]);

  sAudioPacketIndex = (sAudioPacketIndex + 1) % AudioQueueSize;
}

void Spc::playStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0,
             "Incorrect number of arguments passed to spc::playStream (%lu "
             "args passed, 0 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  streamer->play();
}

void Spc::pauseStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0,
             "Incorrect number of arguments passed to spc::pauseStream (%lu "
             "args passed, 0 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  streamer->pause();
}

void Spc::stopStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0,
             "Incorrect number of arguments passed to spc::stopStream (%lu "
             "args passed, 0 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  streamer->stop();
}

void Spc::seekStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 2,
             "Incorrect number of arguments passed to spc::seekStream (%lu "
             "args passed, 2 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  s32 where = Spc::Stack::popItem(interp).mValue;
  JSUStreamSeekFrom whence =
      static_cast<JSUStreamSeekFrom>(Spc::Stack::popItem(interp).mValue);

  streamer->seek(where, whence);
}

void Spc::nextStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1,
             "Incorrect number of arguments passed to spc::nextStream (%lu "
             "args passed, 1 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  f32 delay = static_cast<f32>(Spc::Stack::popItem(interp).mValue);
  streamer->next(delay);
}

void Spc::skipStream(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1,
             "Incorrect number of arguments passed to spc::skipStream (%lu "
             "args passed, 1 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  f32 delay = static_cast<f32>(Spc::Stack::popItem(interp).mValue);
  streamer->skip(delay);
}

void Spc::getStreamVolume(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0,
             "Incorrect number of arguments passed to spc::skipStream (%lu "
             "args passed, 0 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  Spc::Stack::pushItem(interp, streamer->getVolumeLR(), Spc::ValueType::INT);
}

void Spc::setStreamVolume(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1,
             "Incorrect number of arguments passed to spc::skipStream (%lu "
             "args passed, 1 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  u32 volLR = Spc::Stack::popItem(interp).mValue;
  streamer->setVolumeLR(static_cast<u8>(volLR >> 8), static_cast<u8>(volLR));
}

void Spc::getStreamLooping(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0,
             "Incorrect number of arguments passed to spc::skipStream (%lu "
             "args passed, 0 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  Spc::Stack::pushItem(interp, streamer->isLooping(), Spc::ValueType::INT);
}
void Spc::setStreamLooping(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1,
             "Incorrect number of arguments passed to spc::skipStream (%lu "
             "args passed, 1 needed)",
             argc);
  AudioStreamer *streamer = AudioStreamer::getInstance();

  bool looping = static_cast<bool>(Spc::Stack::popItem(interp).mValue);
  streamer->setLooping(looping);
}