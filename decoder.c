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

#include <stdlib.h>
#include <stdio.h>
#include "interf_dec.h"
#include "sp_dec.h"
#include "typedef.h"

#include "board.h"

#ifndef ETSI
#ifndef IF2
#include <string.h>
#define AMR_MAGIC_NUMBER "#!AMR\n"
#endif
#endif

#define PATH_STRING_MAX_LEN 256

char file_speech_path[PATH_STRING_MAX_LEN];
char file_analysis_path[PATH_STRING_MAX_LEN];

static void DecoderUsage(void)
{
    rt_kprintf("Usage: decoder [analysis_file] [speech_file]\n");
    rt_kprintf("       example : decoder out.amr out.wav\n");
}
void DeocoderCopyright(void)
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
 *    Speech decoder main program
 *
 *    Usage: decoder bitstream_file synthesis_file
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
 *    Format for synthesis_file:
 *       Speech is written to a 16 bit 8kHz file.
 *
 *    ETSI bitstream file format is defined using ETSI as preprocessor
 *    definition
 * Returns:
 *    0
 */

static void decoder_thread(void *parameter)
{
    FILE *file_speech, *file_analysis;

    short synth[160];
    int frames = 0;
    int *destate;
    int read_size;
#ifndef ETSI
    unsigned char analysis[32];
    enum Mode dec_mode;
#ifdef IF2
    short block_size[16] =
        {12, 13, 15, 17, 18, 20, 25, 30, 5, 0, 0, 0, 0, 0, 0, 0};
#else
    char magic[8];
    short block_size[16] = {12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0};
#endif
#else
    short analysis[250];
#endif

    /* Process command line options */
    rt_tick_t tick = rt_tick_get_millisecond();

    // open speech file
    file_speech = fopen(file_speech_path, "wb");
    if (file_speech == NULL)
    {
        rt_kprintf("open speech file '%s' error !\n", file_speech_path);
        return;
    }

    // open analysis file
    file_analysis = fopen(file_analysis_path, "rb");
    if (file_analysis == NULL)
    {
        rt_kprintf("open analysis file '%s' error !\n", file_analysis_path);
        fclose(file_speech);
        return;
    }

    /* init decoder */
    destate = Decoder_Interface_init();
    DeocoderCopyright();

#ifndef ETSI
#ifndef IF2
    /* read and verify magic number */
    fread(magic, sizeof(char), strlen(AMR_MAGIC_NUMBER), file_analysis);
    if (strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
    {
        rt_kprintf("%s%s\n", "Invalid magic number: ", magic);
        fclose(file_speech);
        fclose(file_analysis);
        return;
    }
#endif
#endif

#ifndef ETSI
    /* find mode, read file */
    while (fread(analysis, sizeof(unsigned char), 1, file_analysis) > 0)
    {
#ifdef IF2
        dec_mode = analysis[0] & 0x000F;
#else
        dec_mode = (analysis[0] >> 3) & 0x000F;
#endif
        read_size = block_size[dec_mode];

        fread(&analysis[1], sizeof(char), read_size, file_analysis);
#else

    read_size = 250;
    /* read file */
    while (fread(analysis, sizeof(short), read_size, file_analysis) > 0)
    {
#endif

        frames++;

        /* call decoder */
        Decoder_Interface_Decode(destate, analysis, synth, 0);

        fwrite(synth, sizeof(short), 160, file_speech);
    }

    Decoder_Interface_exit(destate);

    fclose(file_speech);
    fclose(file_analysis);
    rt_kprintf("%s%i%s\n", "Decoded ", frames, " frames.");
    rt_kprintf("decoder used %d.%03d s\n", (rt_tick_get_millisecond() - tick) / 1000, (rt_tick_get_millisecond() - tick) % 1000);

    return;
}

static void decoder(int argc, char *argv[])
{
    if (argc != 3)
    {
        DecoderUsage();
        return;
    }

    // set file_analysis_path
    memset(file_analysis_path, 0, PATH_STRING_MAX_LEN);
    memcpy(file_analysis_path, argv[1], strlen(argv[1]));
    // set file_speech_path
    memset(file_speech_path, 0, PATH_STRING_MAX_LEN);
    memcpy(file_speech_path, argv[2], strlen(argv[2]));

    // create thread
    rt_thread_t thread = rt_thread_create("decoder", decoder_thread, RT_NULL, 4 * 1024, 21, 10);
    if (thread != RT_NULL)
        rt_thread_startup(thread);
    return;
}

#ifdef AMRNB_USING_CMD
MSH_CMD_EXPORT_ALIAS(decoder, amr_decoder, AMR - NB decoder cmd);
#endif
