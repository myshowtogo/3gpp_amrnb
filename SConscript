
# RT-Thread building script for bridge

import rtconfig
from building import *

cwd     = GetCurrentDir()
CPPPATH = [cwd, str(Dir('#'))]
src     = Split("""
                interf_enc.c
                sp_enc.c
                sp_dec.c
                interf_dec.c
                interf_rom.c
                encoder.c
                decoder.c
                """)

group = DefineGroup('3gpp-amrnb', src, depend = ['PKG_USING_3GPP_AMRNB'], CPPPATH = CPPPATH)

Return('group')
