#pragma once

#ifndef __INSTR86_H__
#define __INSTR86_H__

/****************************************************************************
*             real mode i286 emulator v1.4 by Fabrice Frances               *
*               (initial work based on David Hedley's pcemu)                *
****************************************************************************/

// file will be included in all cpu variants
// put non i86 instructions in own files (i286, i386, nec)
// function renaming will be added when necessary
// timing value should move to separate array

static void PREFIX86(_add_br8)(i8086_state *cpustate);
static void PREFIX86(_add_wr16)(i8086_state *cpustate);
static void PREFIX86(_add_r8b)(i8086_state *cpustate);
static void PREFIX86(_add_r16w)(i8086_state *cpustate);
static void PREFIX86(_add_ald8)(i8086_state *cpustate);
static void PREFIX86(_add_axd16)(i8086_state *cpustate);
static void PREFIX86(_push_es)(i8086_state *cpustate);
static void PREFIX86(_pop_es)(i8086_state *cpustate);
static void PREFIX86(_or_br8)(i8086_state *cpustate);
static void PREFIX86(_or_r8b)(i8086_state *cpustate);
static void PREFIX86(_or_wr16)(i8086_state *cpustate);
static void PREFIX86(_or_r16w)(i8086_state *cpustate);
static void PREFIX86(_or_ald8)(i8086_state *cpustate);
static void PREFIX86(_or_axd16)(i8086_state *cpustate);
static void PREFIX86(_push_cs)(i8086_state *cpustate);
#ifndef I80286
static void PREFIX86(_pop_cs)(i8086_state *cpustate);
#endif
static void PREFIX86(_adc_br8)(i8086_state *cpustate);
static void PREFIX86(_adc_wr16)(i8086_state *cpustate);
static void PREFIX86(_adc_r8b)(i8086_state *cpustate);
static void PREFIX86(_adc_r16w)(i8086_state *cpustate);
static void PREFIX86(_adc_ald8)(i8086_state *cpustate);
static void PREFIX86(_adc_axd16)(i8086_state *cpustate);
static void PREFIX86(_push_ss)(i8086_state *cpustate);
static void PREFIX86(_pop_ss)(i8086_state *cpustate);
static void PREFIX86(_sbb_br8)(i8086_state *cpustate);
static void PREFIX86(_sbb_wr16)(i8086_state *cpustate);
static void PREFIX86(_sbb_r8b)(i8086_state *cpustate);
static void PREFIX86(_sbb_r16w)(i8086_state *cpustate);
static void PREFIX86(_sbb_ald8)(i8086_state *cpustate);
static void PREFIX86(_sbb_axd16)(i8086_state *cpustate);
static void PREFIX86(_push_ds)(i8086_state *cpustate);
static void PREFIX86(_pop_ds)(i8086_state *cpustate);
static void PREFIX86(_and_br8)(i8086_state *cpustate);
static void PREFIX86(_and_r8b)(i8086_state *cpustate);
static void PREFIX86(_and_wr16)(i8086_state *cpustate);
static void PREFIX86(_and_r16w)(i8086_state *cpustate);
static void PREFIX86(_and_ald8)(i8086_state *cpustate);
static void PREFIX86(_and_axd16)(i8086_state *cpustate);
static void PREFIX86(_es)(i8086_state *cpustate);
static void PREFIX86(_daa)(i8086_state *cpustate);
static void PREFIX86(_sub_br8)(i8086_state *cpustate);
static void PREFIX86(_sub_wr16)(i8086_state *cpustate);
static void PREFIX86(_sub_r8b)(i8086_state *cpustate);
static void PREFIX86(_sub_r16w)(i8086_state *cpustate);
static void PREFIX86(_sub_ald8)(i8086_state *cpustate);
static void PREFIX86(_sub_axd16)(i8086_state *cpustate);
static void PREFIX86(_cs)(i8086_state *cpustate);
static void PREFIX86(_das)(i8086_state *cpustate);
static void PREFIX86(_xor_br8)(i8086_state *cpustate);
static void PREFIX86(_xor_r8b)(i8086_state *cpustate);
static void PREFIX86(_xor_wr16)(i8086_state *cpustate);
static void PREFIX86(_xor_r16w)(i8086_state *cpustate);
static void PREFIX86(_xor_ald8)(i8086_state *cpustate);
static void PREFIX86(_xor_axd16)(i8086_state *cpustate);
static void PREFIX86(_ss)(i8086_state *cpustate);
static void PREFIX86(_aaa)(i8086_state *cpustate);
static void PREFIX86(_cmp_br8)(i8086_state *cpustate);
static void PREFIX86(_cmp_wr16)(i8086_state *cpustate);
static void PREFIX86(_cmp_r8b)(i8086_state *cpustate);
static void PREFIX86(_cmp_r16w)(i8086_state *cpustate);
static void PREFIX86(_cmp_ald8)(i8086_state *cpustate);
static void PREFIX86(_cmp_axd16)(i8086_state *cpustate);
static void PREFIX86(_ds)(i8086_state *cpustate);
static void PREFIX86(_aas)(i8086_state *cpustate);
static void PREFIX86(_inc_ax)(i8086_state *cpustate);
static void PREFIX86(_inc_cx)(i8086_state *cpustate);
static void PREFIX86(_inc_dx)(i8086_state *cpustate);
static void PREFIX86(_inc_bx)(i8086_state *cpustate);
static void PREFIX86(_inc_sp)(i8086_state *cpustate);
static void PREFIX86(_inc_bp)(i8086_state *cpustate);
static void PREFIX86(_inc_si)(i8086_state *cpustate);
static void PREFIX86(_inc_di)(i8086_state *cpustate);
static void PREFIX86(_dec_ax)(i8086_state *cpustate);
static void PREFIX86(_dec_cx)(i8086_state *cpustate);
static void PREFIX86(_dec_dx)(i8086_state *cpustate);
static void PREFIX86(_dec_bx)(i8086_state *cpustate);
static void PREFIX86(_dec_sp)(i8086_state *cpustate);
static void PREFIX86(_dec_bp)(i8086_state *cpustate);
static void PREFIX86(_dec_si)(i8086_state *cpustate);
static void PREFIX86(_dec_di)(i8086_state *cpustate);
static void PREFIX86(_push_ax)(i8086_state *cpustate);
static void PREFIX86(_push_cx)(i8086_state *cpustate);
static void PREFIX86(_push_dx)(i8086_state *cpustate);
static void PREFIX86(_push_bx)(i8086_state *cpustate);
static void PREFIX86(_push_sp)(i8086_state *cpustate);
static void PREFIX86(_push_bp)(i8086_state *cpustate);
static void PREFIX86(_push_si)(i8086_state *cpustate);
static void PREFIX86(_push_di)(i8086_state *cpustate);
static void PREFIX86(_pop_ax)(i8086_state *cpustate);
static void PREFIX86(_pop_cx)(i8086_state *cpustate);
static void PREFIX86(_pop_dx)(i8086_state *cpustate);
static void PREFIX86(_pop_bx)(i8086_state *cpustate);
static void PREFIX86(_pop_sp)(i8086_state *cpustate);
static void PREFIX86(_pop_bp)(i8086_state *cpustate);
static void PREFIX86(_pop_si)(i8086_state *cpustate);
static void PREFIX86(_pop_di)(i8086_state *cpustate);
static void PREFIX86(_jo)(i8086_state *cpustate);
static void PREFIX86(_jno)(i8086_state *cpustate);
static void PREFIX86(_jb)(i8086_state *cpustate);
static void PREFIX86(_jnb)(i8086_state *cpustate);
static void PREFIX86(_jz)(i8086_state *cpustate);
static void PREFIX86(_jnz)(i8086_state *cpustate);
static void PREFIX86(_jbe)(i8086_state *cpustate);
static void PREFIX86(_jnbe)(i8086_state *cpustate);
static void PREFIX86(_js)(i8086_state *cpustate);
static void PREFIX86(_jns)(i8086_state *cpustate);
static void PREFIX86(_jp)(i8086_state *cpustate);
static void PREFIX86(_jnp)(i8086_state *cpustate);
static void PREFIX86(_jl)(i8086_state *cpustate);
static void PREFIX86(_jnl)(i8086_state *cpustate);
static void PREFIX86(_jle)(i8086_state *cpustate);
static void PREFIX86(_jnle)(i8086_state *cpustate);
static void PREFIX86(_80pre)(i8086_state *cpustate);
static void PREFIX86(_82pre)(i8086_state *cpustate);
static void PREFIX86(_81pre)(i8086_state *cpustate);
static void PREFIX86(_83pre)(i8086_state *cpustate);
static void PREFIX86(_test_br8)(i8086_state *cpustate);
static void PREFIX86(_test_wr16)(i8086_state *cpustate);
static void PREFIX86(_xchg_br8)(i8086_state *cpustate);
static void PREFIX86(_xchg_wr16)(i8086_state *cpustate);
static void PREFIX86(_mov_br8)(i8086_state *cpustate);
static void PREFIX86(_mov_r8b)(i8086_state *cpustate);
static void PREFIX86(_mov_wr16)(i8086_state *cpustate);
static void PREFIX86(_mov_r16w)(i8086_state *cpustate);
static void PREFIX86(_mov_wsreg)(i8086_state *cpustate);
static void PREFIX86(_lea)(i8086_state *cpustate);
static void PREFIX86(_mov_sregw)(i8086_state *cpustate);
static void PREFIX86(_invalid)(i8086_state *cpustate);
#ifndef I80286
static void PREFIX86(_invalid_2b)(i8086_state *cpustate);
#endif
static void PREFIX86(_popw)(i8086_state *cpustate);
static void PREFIX86(_nop)(i8086_state *cpustate);
static void PREFIX86(_xchg_axcx)(i8086_state *cpustate);
static void PREFIX86(_xchg_axdx)(i8086_state *cpustate);
static void PREFIX86(_xchg_axbx)(i8086_state *cpustate);
static void PREFIX86(_xchg_axsp)(i8086_state *cpustate);
static void PREFIX86(_xchg_axbp)(i8086_state *cpustate);
static void PREFIX86(_xchg_axsi)(i8086_state *cpustate);
static void PREFIX86(_xchg_axdi)(i8086_state *cpustate);
static void PREFIX86(_cbw)(i8086_state *cpustate);
static void PREFIX86(_cwd)(i8086_state *cpustate);
static void PREFIX86(_call_far)(i8086_state *cpustate);
static void PREFIX86(_pushf)(i8086_state *cpustate);
static void PREFIX86(_popf)(i8086_state *cpustate);
static void PREFIX86(_sahf)(i8086_state *cpustate);
static void PREFIX86(_lahf)(i8086_state *cpustate);
static void PREFIX86(_mov_aldisp)(i8086_state *cpustate);
static void PREFIX86(_mov_axdisp)(i8086_state *cpustate);
static void PREFIX86(_mov_dispal)(i8086_state *cpustate);
static void PREFIX86(_mov_dispax)(i8086_state *cpustate);
static void PREFIX86(_movsb)(i8086_state *cpustate);
static void PREFIX86(_movsw)(i8086_state *cpustate);
static void PREFIX86(_cmpsb)(i8086_state *cpustate);
static void PREFIX86(_cmpsw)(i8086_state *cpustate);
static void PREFIX86(_test_ald8)(i8086_state *cpustate);
static void PREFIX86(_test_axd16)(i8086_state *cpustate);
static void PREFIX86(_stosb)(i8086_state *cpustate);
static void PREFIX86(_stosw)(i8086_state *cpustate);
static void PREFIX86(_lodsb)(i8086_state *cpustate);
static void PREFIX86(_lodsw)(i8086_state *cpustate);
static void PREFIX86(_scasb)(i8086_state *cpustate);
static void PREFIX86(_scasw)(i8086_state *cpustate);
static void PREFIX86(_mov_ald8)(i8086_state *cpustate);
static void PREFIX86(_mov_cld8)(i8086_state *cpustate);
static void PREFIX86(_mov_dld8)(i8086_state *cpustate);
static void PREFIX86(_mov_bld8)(i8086_state *cpustate);
static void PREFIX86(_mov_ahd8)(i8086_state *cpustate);
static void PREFIX86(_mov_chd8)(i8086_state *cpustate);
static void PREFIX86(_mov_dhd8)(i8086_state *cpustate);
static void PREFIX86(_mov_bhd8)(i8086_state *cpustate);
static void PREFIX86(_mov_axd16)(i8086_state *cpustate);
static void PREFIX86(_mov_cxd16)(i8086_state *cpustate);
static void PREFIX86(_mov_dxd16)(i8086_state *cpustate);
static void PREFIX86(_mov_bxd16)(i8086_state *cpustate);
static void PREFIX86(_mov_spd16)(i8086_state *cpustate);
static void PREFIX86(_mov_bpd16)(i8086_state *cpustate);
static void PREFIX86(_mov_sid16)(i8086_state *cpustate);
static void PREFIX86(_mov_did16)(i8086_state *cpustate);
static void PREFIX86(_ret_d16)(i8086_state *cpustate);
static void PREFIX86(_ret)(i8086_state *cpustate);
static void PREFIX86(_les_dw)(i8086_state *cpustate);
static void PREFIX86(_lds_dw)(i8086_state *cpustate);
static void PREFIX86(_mov_bd8)(i8086_state *cpustate);
static void PREFIX86(_mov_wd16)(i8086_state *cpustate);
static void PREFIX86(_retf_d16)(i8086_state *cpustate);
static void PREFIX86(_retf)(i8086_state *cpustate);
static void PREFIX86(_int3)(i8086_state *cpustate);
static void PREFIX86(_int)(i8086_state *cpustate);
static void PREFIX86(_into)(i8086_state *cpustate);
static void PREFIX86(_iret)(i8086_state *cpustate);
static void PREFIX86(_rotshft_b)(i8086_state *cpustate);
static void PREFIX86(_rotshft_w)(i8086_state *cpustate);
static void PREFIX86(_rotshft_bcl)(i8086_state *cpustate);
static void PREFIX86(_rotshft_wcl)(i8086_state *cpustate);
static void PREFIX86(_aam)(i8086_state *cpustate);
static void PREFIX86(_aad)(i8086_state *cpustate);
static void PREFIX86(_xlat)(i8086_state *cpustate);
static void PREFIX86(_escape)(i8086_state *cpustate);
static void PREFIX86(_loopne)(i8086_state *cpustate);
static void PREFIX86(_loope)(i8086_state *cpustate);
static void PREFIX86(_loop)(i8086_state *cpustate);
static void PREFIX86(_jcxz)(i8086_state *cpustate);
static void PREFIX86(_inal)(i8086_state *cpustate);
static void PREFIX86(_inax)(i8086_state *cpustate);
static void PREFIX86(_outal)(i8086_state *cpustate);
static void PREFIX86(_outax)(i8086_state *cpustate);
static void PREFIX86(_call_d16)(i8086_state *cpustate);
static void PREFIX86(_jmp_d16)(i8086_state *cpustate);
static void PREFIX86(_jmp_far)(i8086_state *cpustate);
static void PREFIX86(_jmp_d8)(i8086_state *cpustate);
static void PREFIX86(_inaldx)(i8086_state *cpustate);
static void PREFIX86(_inaxdx)(i8086_state *cpustate);
static void PREFIX86(_outdxal)(i8086_state *cpustate);
static void PREFIX86(_outdxax)(i8086_state *cpustate);
static void PREFIX86(_lock)(i8086_state *cpustate);
static void PREFIX86(_repne)(i8086_state *cpustate);
static void PREFIX86(_repe)(i8086_state *cpustate);
static void PREFIX86(_hlt)(i8086_state *cpustate);
static void PREFIX86(_cmc)(i8086_state *cpustate);
static void PREFIX86(_f6pre)(i8086_state *cpustate);
static void PREFIX86(_f7pre)(i8086_state *cpustate);
static void PREFIX86(_clc)(i8086_state *cpustate);
static void PREFIX86(_stc)(i8086_state *cpustate);
static void PREFIX86(_cli)(i8086_state *cpustate);
static void PREFIX86(_sti)(i8086_state *cpustate);
static void PREFIX86(_cld)(i8086_state *cpustate);
static void PREFIX86(_std)(i8086_state *cpustate);
static void PREFIX86(_fepre)(i8086_state *cpustate);
static void PREFIX86(_ffpre)(i8086_state *cpustate);
static void PREFIX86(_wait)(i8086_state *cpustate);

#endif /* __INSTR86_H__ */
