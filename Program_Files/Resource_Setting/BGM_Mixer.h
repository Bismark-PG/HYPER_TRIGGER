/*==============================================================================

	Manage BGM [BGM_Mixer.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef BGM_MIXER_H
#define BGM_MIXER_H

void Mixer_Init();
void Mixer_First_Game_Start(double elapsed_time);
void Mixer_Control_Layer(const char* LayerName, bool IsTurnOn);

#endif // BGM_MIXER_H