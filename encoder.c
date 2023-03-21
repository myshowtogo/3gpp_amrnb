/*
 * ===================================================================
 *  TS 26.104
 *  REL-5 V5.4.0 2004-03
 *  REL-6 V6.1.0 2004-03
 *  REL-15 V15.1.0 2018-07
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/*
 * encoder.c
 *
 *
 * Project:
 *    AMR Floating-Point Codec
 *
 * Contains:
 *    Speech encoder main program
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "typedef.h"
#include "interf_enc.h"

#include "board.h"
#include <rtthread.h>

#ifndef ETSI
#ifndef IF2
#define AMR_MAGIC_NUMBER "#!AMR\n"
#endif
#endif

#define PATH_STRING_MAX_LEN 256
#define MODE_MAX_LEN 10

static char file_speech_path[PATH_STRING_MAX_LEN];
static char file_encoded_path[PATH_STRING_MAX_LEN];
static char file_mode_path[MODE_MAX_LEN];

static const short modeConv[] = {475, 515, 59, 67, 74, 795, 102, 122};

static void EncoderUsage(void)
{
    rt_kprintf("Usage: encoder -dtx [mode] [speech_file] [bitstream_file]\n");
    rt_kprintf("       mode    : MR475, MR515, MR59, MR67, MR74, MR795, MR102, MR122\n");
    rt_kprintf("       example : encoder -dtx MR102 test.wav out.amr\n");
}
static void EncoderCopyright(void)
{
    rt_kprintf("Copyright:\n");
    rt_kprintf("========================================\n");
    rt_kprintf(" TS 26.104                              \n");
    rt_kprintf(" REL-16 V16.0.0 2018-09                 \n");
    rt_kprintf(" 3GPP AMR Floating-point Speech Encoder \n");
    rt_kprintf("========================================\n");
}
/*
 * main
 *
 *
 * Function:
 *    Speech encoder main program
 *
 *    Usage: encoder speech_file bitstream_file mode dtx mode_file
 *
 *    Format for speech_file:
 *       Speech is read from a binary file of 16 bits data.
 *
 *    Format for ETSI bitstream file:
 *       1 word (2-byte) for the TX frame type
 *       244 words (2-byte) containing 244 bits.
 *          Bit 0 = 0x0000 and Bit 1 = 0x0001
 *       1 word (2-byte) for the mode indication
 *       4 words for future use, currently written as zero
 *
 *    Format for 3GPP bitstream file:
 *       Holds mode information and bits packed to octets.
 *       Size is from 1 byte to 31 bytes.
 *
 *    ETSI bitstream file format is defined using ETSI as preprocessor
 *    definition
 *
 *    mode        : MR475, MR515, MR59, MR67, MR74, MR795, MR102, MR122
 *    mode_file   : reads mode information from a file
 * Returns:
 *    0
 */

static void encoder_thread(void *parameter)
{
    /* file strucrures */
    FILE *file_speech = NULL;
    FILE *file_encoded = NULL;
    FILE *file_mode = NULL;

    /* input speech vector */
    short speech[160];

    /* counters */
    int byte_counter, frames = 0, bytes = 0;

    /* pointer to encoder state structure */
    int *enstate;

    /* requested mode */
    enum Mode req_mode = MR122;
    int dtx = 0;

    /* temporary variables */
    char mode_string[9];
    long mode_tmp;

    /* bitstream filetype */
#ifndef ETSI
    unsigned char serial_data[32];
#else
    short serial_data[250] =
        {0};
#endif

    /* Process command line options */
    rt_tick_t tick = rt_tick_get_millisecond();

    // open encoded file
    file_encoded = fopen(file_encoded_path, "wb");
    if (file_encoded == NULL)
    {
        rt_kprintf("open encoded file error !\n");
        return;
    }

    // open speech file
    file_speech = fopen(file_speech_path, "rb");
    if (file_speech == NULL)
    {
        rt_kprintf("open speech file error !\n");
        fclose(file_encoded);
        return;
    }

    // set code rate
    mode_tmp = strtol(&file_mode_path[2], NULL, 0);
    for (req_mode = 0; req_mode < 8; req_mode++)
    {
        if (mode_tmp == modeConv[req_mode])
            break;
    }
    if (req_mode == 8)
    {
        rt_kprintf("mode error !\n");
        fclose(file_speech);
        fclose(file_encoded);
        if (file_mode != NULL)
            fclose(file_mode);
        return;
    }
    dtx = 1;

    enstate = Encoder_Interface_init(dtx);
    EncoderCopyright();

#ifndef VAD2
    rt_kprintf("%s\n", "Code compiled with VAD option: VAD1");
#else
    rt_kprintf("%s\n", "Code compiled with VAD option: VAD2");
#endif

#ifndef ETSI
#ifndef IF2
    /* write magic number to indicate single channel AMR file storage format */
    bytes = fwrite(AMR_MAGIC_NUMBER, sizeof(char), strlen(AMR_MAGIC_NUMBER), file_encoded);
#endif
#endif

    /* read file */
    while (fread(speech, sizeof(Word16), 160, file_speech) > 0)
    {
        /* read mode */
        if (file_mode != NULL)
        {
            req_mode = 8;
            if (fscanf(file_mode, "%9s\n", mode_string) != EOF)
            {
                mode_tmp = strtol(&mode_string[2], NULL, 0);
                for (req_mode = 0; req_mode < 8; req_mode++)
                {
                    if (mode_tmp == modeConv[req_mode])
                    {
                        break;
                    }
                }
            }
            if (req_mode == 8)
            {
                break;
            }
        }

        frames++;

        /* call encoder */
        byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, serial_data, 0);
        if (frames % 64 == 0)
            rt_kprintf(".");

        bytes += byte_counter;
        fwrite(serial_data, sizeof(UWord8), byte_counter, file_encoded);
        fflush(file_encoded);
    }
    Encoder_Interface_exit(enstate);
    rt_kprintf("\n");

#ifndef ETSI
#ifdef IF2
    rt_kprintf("%s%i%s%i%s\n", "Frame structure AMR IF2: ", frames, " frames, ", bytes, " bytes.");
#else
    rt_kprintf("%s%i%s%i%s\n", "Frame structure AMR MIME file storage format: ", frames, " frames, ", bytes,
               " bytes.");
#endif
#else
    rt_kprintf("%s%i%s\n", "Frame structure AMR ETSI: ", frames, " frames. ");
#endif

    fclose(file_speech);
    fclose(file_encoded);
    if (file_mode != NULL)
        fclose(file_mode);
    rt_kprintf("encoder used %d.%03d s\n", (rt_tick_get_millisecond() - tick) / 1000, (rt_tick_get_millisecond() - tick) % 1000);
}

static void encoder(int argc, char *argv[])
{
    if (argc != 5)
    {
        EncoderUsage();
        return;
    }

    // set file_mode_path
    memset(file_mode_path, 0, MODE_MAX_LEN);
    memcpy(file_mode_path, argv[2], strlen(argv[2]));
    // set file_speech_path
    memset(file_speech_path, 0, PATH_STRING_MAX_LEN);
    memcpy(file_speech_path, argv[3], strlen(argv[3]));
    // set file_encoded_path
    memset(file_encoded_path, 0, PATH_STRING_MAX_LEN);
    memcpy(file_encoded_path, argv[4], strlen(argv[4]));

    // create thread
    rt_thread_t thread = rt_thread_create("encoder", encoder_thread, RT_NULL, 20 * 1024, 21, 10);
    if (thread != RT_NULL)
        rt_thread_startup(thread);
    return;
}

#ifdef AMRNB_USING_CMD
MSH_CMD_EXPORT_ALIAS(encoder, amr_encoder, AMR - NB encoder cmd);
#endif
