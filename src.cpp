#include <windows.h>
#include <exedit.hpp>
#include <algorithm>

constexpr int track_n = 1;

inline static char name[] = "オーディオバッファ";
inline static char* track_name[track_n] = { const_cast<char*>("元の音量") };
inline static int track_default[track_n] = { 0 };
inline static int track_s[track_n] = { 0 };
inline static int track_e[track_n] = { 1000 };
inline static int track_scale[track_n] = { 10 };

BOOL func_proc(ExEdit::Filter* efp, ExEdit::FilterProcInfo* efpip) {
    memcpy(efpip->audio_data, efpip->audio_p, efpip->audio_n * efpip->audio_ch * sizeof(short));

    int next_track_values[track_n];
    int* next_track = next_track_values;

    int tl_nextframe, tl_nextsubframe;
    if (efpip->audio_speed == 0) {
        tl_nextframe = min(efpip->frame_num + 1, efp->frame_end_chain);
        tl_nextsubframe = 0;
    } else {
        int tl_nextmilli = min(efpip->audio_milliframe + efpip->audio_speed / 1000, efp->frame_end_chain * 1000);
        tl_nextframe = tl_nextmilli / 1000;
        tl_nextsubframe = (tl_nextmilli % 1000) / 10;
    }
    efp->exfunc->calc_trackbar(efp->processing, tl_nextframe, tl_nextsubframe, next_track_values, nullptr);

    if (efp->track[0] <= 0 && next_track[0] <= 0) {
        memset(efpip->audio_p, 0, efpip->audio_n * efpip->audio_ch * sizeof(short));
    } else if (efp->track[0] < 1000 || next_track[0] < 1000) {
        int efp_track0 = std::clamp(efp->track[0], 0, 1000);
        next_track[0] = std::clamp(next_track[0], 0, 1000);
        int volume = (int)round((double)efp_track0 * (4096.0 * 0.001 * 65536.0));
        short* volume_hi = (short*)&volume + 1;

        int step = (int)round((double)(next_track[0] - efp_track0) * (4096.0 * 0.001 * 65536.0)) / efpip->audio_n;

        short* audiop = efpip->audio_p;
        for (int i = efpip->audio_n; 0 < i; i--) {
            for (int ch = efpip->audio_ch; 0 < ch; ch--) {
                *audiop = (int)*audiop * *volume_hi >> 12;
                audiop++;
            }
            volume += step;
        }
    }

    return TRUE;
}


ExEdit::Filter ef = {
    .flag = ExEdit::Filter::Flag::Audio | ExEdit::Filter::Flag::Input,
    .name = name,
    .track_n = track_n,
    .track_name = track_name,
    .track_default = track_default,
    .track_s = track_s,
    .track_e = track_e,
    .check_n = 0,
    .func_proc = &func_proc,
    .track_scale = track_scale,
};

ExEdit::Filter* filter_list[] = {
    &ef,
    NULL
};
EXTERN_C __declspec(dllexport)ExEdit::Filter** __stdcall GetFilterTableList() {
    return filter_list;
}
