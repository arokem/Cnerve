#include "Ks_Gillespie.h"
#include <math.h>

int main(){
	kinParam Kins;
	Kins.a_s_A = 0.3;
	Kins.a_s_B = -12.5;
	Kins.a_s_C = 23.6;
	Kins.b_s_A = (pow(1.7,3*0)*0.000739);
	Kins.b_s_B = -80.1;
	Kins.b_s_C = 21.8;

	double E_rest = -168;
	double V_m = 0;
	int doInit = 1;
	int N_slowK = 150;
	double it = 0.001;
	double si = 0;
	int open_Chans = Ks_Gillespie(N_slowK,chan_states,V_m+E_rest,si,it,Kins);
	stim = np.linspace(0,40-E_rest,150);
	static int  s0, s1;
	s0 = np.append(s0,chan_states[0]/N_slowK)
	s1 = np.append(s1,chan_states[1]/N_slowK)
	for V_m in stim:
	    s = Ks_Gillespie(N_slowK,chan_states,V_m+E_rest,si,it,kinParams)
	    s0 = np.append(s0,chan_states[0]/N_slowK)
	    s1 = np.append(s1,chan_states[1]/N_slowK)

	return 0;
}
