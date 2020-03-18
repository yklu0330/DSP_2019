#include "hmm.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int obsToNum(char obs)
{	
	return int(obs - 'A');
}

void calAlpha(HMM *hmm, string line, double alpha[][50]) // Forward algo.
{
	for (int i = 0; i < hmm->state_num; i++) // Initialization
		alpha[i][0] = hmm->observation[obsToNum(line[0])][i] * hmm->initial[i];
	for (int i = 0; i < line.size()-1; i++) // Induction
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			double sum = 0;
			for (int k = 0; k < hmm->state_num; k++)
			{
				sum += alpha[k][i] * hmm->transition[k][j];
			}
			alpha[j][i+1] = sum * hmm->observation[obsToNum(line[i+1])][j];
		}
	}
}

void calBeta(HMM *hmm, string line, double beta[][50]) // Backward algo.
{
	for (int i = 0; i < hmm->state_num; i++) // Initialization
		beta[i][49] = 1;
	for (int i = (line.size() - 1); i > 0; i--) // Induction
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			double sum = 0;
			for (int k = 0; k < hmm->state_num; k++)
			{
				sum += hmm->transition[j][k] * hmm->observation[obsToNum(line[i])][k] * beta[k][i];
			}
			beta[j][i-1] = sum;
			
		}	
	}
}

void calGamma(HMM *hmm, string line, double alpha[][50], double beta[][50], double gamma[][50])
{
	double sum = 0;
	for (int i = 0; i < hmm->state_num; i++)
	{
		sum += alpha[i][49];
	}
	for (int i = 0; i < line.size(); i++)
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			gamma[j][i] = alpha[j][i] * beta[j][i] / sum;
		}
	}
}

void calEpsilon(HMM *hmm, string line, double alpha[][50], double beta[][50], double epsilon[][49][6])
{
	double sum = 0;
	for (int i = 0; i < hmm->state_num; i++)
	{
		sum += alpha[i][49];
	}
	for (int i = 0; i < line.size() - 1; i++)
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			for (int k = 0; k < hmm->state_num; k++)
			{
				epsilon[j][i][k] = (alpha[j][i] * hmm->transition[j][k] * hmm->observation[obsToNum(line[i+1])][k] * beta[k][i+1]) / sum;
			}
		}

	}
	
}

void trainModel(HMM *hmm, string input)
{
	string seq;
	ifstream iptFile(input);
	int sampleNum = 0;

	double alpha[6][50] = {0};
	double beta[6][50] = {0};
	double gamma[6][50] = {0};
	double epsilon[6][49][6] = {0};

	double accGamma[6][50] = {0};
	double accEpsilon[6][49][6] = {0};
	double obsNum[50][6] = {0};
	
	while (getline(iptFile, seq))
	{
		sampleNum++;
		calAlpha(hmm, seq, alpha);
		calBeta(hmm, seq, beta);
		calGamma(hmm, seq, alpha, beta, gamma);
		calEpsilon(hmm, seq, alpha, beta, epsilon);		

		for (int i = 0; i < hmm->state_num; i++)
		{
			for (int j = 0; j < 50; j++)
			{
				accGamma[i][j] += gamma[i][j];
			}
		}

		for (int i = 0; i < hmm->state_num; i++)
		{
			for (int j = 0; j < 49; j++)
			{
				for (int k = 0; k < hmm->state_num; k++)
				{
					accEpsilon[i][j][k] += epsilon[i][j][k];
				}
			}
		}

		for (int i = 0; i < hmm->state_num; i++)
	    {
	     	for (int j = 0; j < seq.size(); j++)
	    	{
	     		obsNum[obsToNum(seq[j])][i] += gamma[i][j];
	     	}
	    }
	}
	// Re-estimate Model Parameters
	double pi[6] = {0};
	double a[6][6] = {0};
	double b[6][6] = {0};
	for (int i = 0; i < hmm->state_num; i++)
		pi[i] = accGamma[i][0] / sampleNum;
	for (int i = 0; i < hmm->state_num; i++)
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			double transNum = 0;
			for (int k = 0; k < 49; k++)
			{
				transNum += accEpsilon[i][k][j];
			}
			double visitNum = 0;
			for (int k = 0; k < 49; k++)
			{
				visitNum += accGamma[i][k];
			}
			a[i][j] = transNum / visitNum;
		}
	}
	for (int i = 0; i < hmm->state_num; i++)
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			double visitNum = 0;
			for (int k = 0; k < 50; k++)
			{
				visitNum += accGamma[i][k];
			}
			b[j][i] = obsNum[j][i] / visitNum;
		}
	}
	for (int i = 0; i < hmm->state_num; i++)
		hmm->initial[i] = pi[i];
	for (int i = 0; i < hmm->state_num; i++)
	{
		for (int j = 0; j < hmm->state_num; j++)
		{
			hmm->transition[i][j] = a[i][j];
			hmm->observation[i][j] = b[i][j];
		}
	}
}

int main(int argc, char* argv[])
{

	// HMM hmms[5];
	// load_models( "modellist.txt", hmms, 5);
	// dump_models( hmms, 5);

	string iterStr = argv[1];
	int iter = stoi(iterStr);

	for (int i = 0; i < iter; i++)
	{
		FILE *file = open_or_die(argv[4],"w");
		HMM hmm;

		if (i == 0)
			loadHMM(&hmm, argv[2]);
		else
			loadHMM(&hmm, argv[4]);

		trainModel(&hmm, argv[3]);
		dumpHMM(stderr, &hmm);
		dumpHMM(file, &hmm);
		fclose(file);
	}
	
	return 0;
}
