#include "image_editor.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avif/avif.h"
#include "avifjpeg.h"
#include "avifutil.h"
#include "y4m.h"

#if defined(_WIN32)
#include <locale.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define DEFAULT_JPEG_QUALITY 90

int avif2jpeg(const char* inputfile) {
  const char* source = inputfile;
  char output[1024] = "";
  avifAppFileFormat inputFormat = avifGuessFileFormat(source);
  if (inputFormat != AVIF_APP_FILE_FORMAT_AVIF) {
    fprintf(stderr, "input file %s not a avif file\n", inputfile);
    goto cleanup;
  }

  char* dot = strstr(inputfile, ".avif");
  if (dot) {
    int size = (int)(dot - source);
    memcpy(output, source, size);
  } else {
    memcpy(output, source, strlen(inputfile));
  }
  strcat(output, ".jpeg");

  int returnCode = 1;
  avifBool rawColor = AVIF_FALSE;
  avifChromaUpsampling chromaUpsampling = AVIF_CHROMA_UPSAMPLING_AUTOMATIC;
  avifDecoder* decoder = avifDecoderCreate();
  if (!decoder) {
    fprintf(stderr, "Memory allocation failure\n");
    goto cleanup;
  }
  decoder->maxThreads = 1;
  decoder->codecChoice = AVIF_CODEC_CHOICE_AUTO;
  decoder->imageSizeLimit = AVIF_DEFAULT_IMAGE_SIZE_LIMIT;
  decoder->imageDimensionLimit = AVIF_DEFAULT_IMAGE_DIMENSION_LIMIT;
  decoder->strictFlags = AVIF_STRICT_ENABLED;
  decoder->allowProgressive = AVIF_FALSE;

  avifResult result = avifDecoderSetIOFile(decoder, source);
  if (result != AVIF_RESULT_OK) {
    fprintf(stderr, "Cannot open file for read: %s\n", source);
    goto cleanup;
  }

  result = avifDecoderParse(decoder);
  if (result != AVIF_RESULT_OK) {
    fprintf(stderr, "ERROR: Failed to parse image: %s\n",
            avifResultToString(result));
    goto cleanup;
  }

  result = avifDecoderNthImage(decoder, 0);
  if (result != AVIF_RESULT_OK) {
    fprintf(stderr, "ERROR: Failed to decode image: %s\n",
            avifResultToString(result));
    goto cleanup;
  }

  printf("Image decoded: %s\n", source);
  printf("Image details:\n");
  avifBool gainMapPresent = AVIF_FALSE;
#if defined(AVIF_ENABLE_EXPERIMENTAL_GAIN_MAP)
  gainMapPresent = decoder->gainMapPresent;
#endif
  avifImageDump(decoder->image, 0, 0, gainMapPresent,
                decoder->progressiveState);

  avifAppFileFormat outputFormat = avifGuessFileFormat(output);
  if (outputFormat == AVIF_APP_FILE_FORMAT_UNKNOWN) {
    fprintf(stderr, "Cannot determine output file extension: %s\n", output);
    goto cleanup;
  } else if (outputFormat == AVIF_APP_FILE_FORMAT_JPEG) {
    // Bypass alpha multiply step during conversion
    if (rawColor) {
      decoder->image->alphaPremultiplied = AVIF_TRUE;
    }
    if (!avifJPEGWrite(output, decoder->image, DEFAULT_JPEG_QUALITY,
                       chromaUpsampling)) {
      goto cleanup;
    }
  } else {
    fprintf(stderr, "Unsupported output file extension: %s\n", output);
    goto cleanup;
  }

  returnCode = 0;

cleanup:
  if (decoder != NULL) {
    if (returnCode != 0) {
      avifDumpDiagnostics(&decoder->diag);
    }
    avifDecoderDestroy(decoder);
  }

  return returnCode;
}