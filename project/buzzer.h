#ifndef buzzer_included
#define buzzer_included

#define NOTE_D6 852  // D6 = 1174.66 Hz
#define NOTE_E6 762  // E6 = 1318.51 Hz
#define NOTE_G6 650  // G6 = 1567.98 Hz
#define NOTE_REST 0


void buzzer_init();
void buzzer_set_period(short cycles);
void buzzer_off();
void play_song();


#endif // included
