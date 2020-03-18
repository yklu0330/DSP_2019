#include "hmm.h"
#include <math.h>

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int obsToNum(char obs)
{	
	return int(obs - 'A');
}

double Viterbi(HMM *hmm, string line)
{
	double v[6][50] = {0};
	for (int i = 0; i < hmm->state_num; i++) // Initialization
		v[i][0] = hmm->initial[i] * hmm->observation[obsToNum(line[0])][i];
	for (int t = 0; t < (line.size() - 1); t++) // Recursion
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			double max = 0;
			for (int i = 0; i < hmm->state_num; i++)
			{
				double curV = v[i][t] * hmm->transition[i][j] * hmm->observation[obsToNum(line[t])][j];
				if (curV > max)
					max = curV;
			}
			v[j][t+1] = max;
		}
	}
	double max = 0;
	for (int i = 0; i < hmm->state_num; i++)
		if (v[i][49] > max)
			max = v[i][49];
	return max;
}


void testModel(HMM *hmm, string input, string output)
{
	ifstream iptFile(input);
	ofstream optFile(output);
	string seq;

	while (getline(iptFile, seq))
	{
		double max = 0;
		int modelIdx = 0;
		for (int i = 0; i < 5; i++)
		{
			double temp = Viterbi(hmm + i, seq);
			if (temp > max)
			{
				max = temp;
				modelIdx = i + 1;
			}
		}
		optFile << "model_0" << modelIdx << ".txt " << max << endl;
	}
}

void calAcc(string input1, string input2)
{
	ifstream iptFile1(input1);
	ifstream iptFile2(input2);
	// ofstream opt("results.txt", ios::app);
	string seq1, seq2;
	double sampleNum = 0;
	double right = 0;

	while (getline(iptFile1, seq1))
	{
		sampleNum++;
		getline(iptFile2, seq2);
		if (seq1[7] == seq2[7])
			right++;
	}
	double acc = right / sampleNum;
	cout << "accuracy: " << acc << endl;
	// opt << "accuracy: " << acc << endl;
}

int main(int argc, char* argv[])
{
	HMM hmms[5];
	load_models(argv[1], hmms, 5);
	testModel(hmms, argv[2], argv[3]);
	// calAcc(argv[3], "./data/test_lbl.txt");
	dump_models( hmms, 5);

	
	return 0;
}
