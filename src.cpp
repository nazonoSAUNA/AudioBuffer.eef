#include <windows.h>
#include <exedit.hpp>

#define TRACK_N 1

inline static char name[] = "オーディオバッファ";
inline static char* track_name[TRACK_N] = { const_cast<char*>("元の音量") };
inline static int track_default[TRACK_N] = { 0 };
inline static int track_s[TRACK_N] = { 0 };
inline static int track_e[TRACK_N] = { 1000 };
inline static int track_scale[TRACK_N] = { 10 };

BOOL func_proc(ExEdit::Filter* efp, ExEdit::FilterProcInfo* efpip) {
    memcpy(efpip->audio_data, efpip->audio_p, efpip->audio_n * efpip->audio_ch * sizeof(short));

    if (efp->track[0] <= 0) {
        memset(efpip->audio_p, 0, efpip->audio_n * efpip->audio_ch * sizeof(short));
    } else if (efp->track[0] < 1000) {
        int volume = (efp->track[0] << 9) / 125; // * 4096 / 1000
        short* ptr = efpip->audio_p;
        for (int i = efpip->audio_n * efpip->audio_ch; 0 < i; i--) {
            *ptr = *ptr * volume >> 12;
            ptr++;
        }
    }
    return TRUE;
}


ExEdit::Filter ef = {
    .flag = ExEdit::Filter::Flag::Audio | ExEdit::Filter::Flag::Input,
    .name = name,
    .track_n = TRACK_N,
    .track_name = track_name,
    .track_default = track_default,
    .track_s = track_s,
    .track_e = track_e,
    .check_n = 0,
    .func_proc = &func_proc,
    .track_scale = track_scale,
    .track_drag_min = track_s,
    .track_drag_max = track_e,
};

ExEdit::Filter* filter_list[] = {
    &ef,
    NULL
};
EXTERN_C __declspec(dllexport)ExEdit::Filter** __stdcall GetFilterTableList() {
    return filter_list;
}
