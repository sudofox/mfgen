#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void play_tone(double frequency1, double frequency2, int duration) {
  int sample_rate = 44100;
  int samples = duration * sample_rate / 1000;
  short* wave = (short*)malloc(samples * sizeof(short));

  for (int i = 0; i < samples; i++) {
    double t = (double)i / (double)sample_rate;
    wave[i] = (short)(32760.0 * (sin(2.0 * M_PI * frequency1 * t) / 2.0 +
                                 sin(2.0 * M_PI * frequency2 * t) / 2.0));
  }

  snd_pcm_t* handle;
  snd_pcm_hw_params_t* params;
  snd_pcm_uframes_t frames;

  // Open the PCM device in playback mode
  if (snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) {
    printf("Error: Cannot open PCM device.\n");
    return;
  }

  // Allocate hardware parameters object
  snd_pcm_hw_params_alloca(&params);

  // Fill the hardware parameters with default values
  snd_pcm_hw_params_any(handle, params);

  // Set the desired hardware parameters
  if (snd_pcm_hw_params_set_access(
          handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    printf("Error: Cannot set access type.\n");
    return;
  }
  if (snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE) < 0) {
    printf("Error: Cannot set sample format.\n");
    return;
  }
  if (snd_pcm_hw_params_set_rate_near(
          handle, params, (unsigned int*)&sample_rate, 0) < 0) {
    fprintf(stderr, "Error setting sample rate.\n");
    exit(EXIT_FAILURE);
  }
  if (snd_pcm_hw_params_set_channels(handle, params, 1) < 0) {
    printf("Error: Cannot set channel count.\n");
    return;
  }

  // Set hardware parameters to the playback handle
  if (snd_pcm_hw_params(handle, params) < 0) {
    printf("Error: Cannot set hardware parameters.\n");
    return;
  }

  // Write the waveform data to the PCM device
  frames = samples;
  if (snd_pcm_writei(handle, wave, frames) < 0) {
    printf("Error: Cannot write PCM data.\n");
    return;
  }

  // Wait until all playback is finished
  snd_pcm_drain(handle);

  // Release the PCM handle
  snd_pcm_close(handle);

  free(wave);
}

void generate_dtmf(char digit, double duration) {
  double freq1, freq2;

  switch (digit) {
    case '1':
      freq1 = 697;
      freq2 = 1209;
      break;
    case '2':
      freq1 = 697;
      freq2 = 1336;
      break;
    case '3':
      freq1 = 697;
      freq2 = 1477;
      break;
    case '4':
      freq1 = 770;
      freq2 = 1209;
      break;
    case '5':
      freq1 = 770;
      freq2 = 1336;
      break;
    case '6':
      freq1 = 770;
      freq2 = 1477;
      break;
    case '7':
      freq1 = 852;
      freq2 = 1209;
      break;
    case '8':
      freq1 = 852;
      freq2 = 1336;
      break;
    case '9':
      freq1 = 852;
      freq2 = 1477;
      break;
    case '*':
      freq1 = 941;
      freq2 = 1209;
      break;
    case '0':
      freq1 = 941;
      freq2 = 1336;
      break;
    case '#':
      freq1 = 941;
      freq2 = 1477;
      break;
    default:
      return;
  }

  play_tone(freq1, freq2, duration);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage: %s <delay between digits> <phone number>\n", argv[0]);
    return 1;
  }

  int delay = atoi(argv[1]);
  char* number = argv[2];

  for (size_t i = 0; i < strlen(number); i++) {
    generate_dtmf(number[i], 100);
    usleep(delay * 1000);
  }

  return 0;
}
