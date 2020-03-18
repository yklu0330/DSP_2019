#include "Ngram.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h> 
#include <string.h> 
#include <float.h>

using namespace std;

int main(int argc, char *argv[])
{
	Vocab voc;
	//read lm file
	int ngramOrder = 2;
	Ngram *lm = new Ngram(voc, ngramOrder);
	File lmFile(argv[3], "r");
	lm->read(lmFile);
	lmFile.close();

	//read map file
	ifstream mapFile(argv[2]);
	map<string, vector<string>> zhuMap;
	string line;
	while (getline(mapFile, line))
	{
		char temp[line.size()+1];
		strcpy(temp, line.c_str());
		
		char *temp2 = strtok(temp, "\t");
		string zhu = string(temp2);
		char *big5 = strtok(NULL, " ");
		while (big5 != NULL)
		{
			zhuMap[zhu].push_back(string(big5));
			big5 = strtok(NULL, " ");
		}
	}
	zhuMap["<s>"].push_back("<s>");
	mapFile.close();

	//read text file
	ifstream testFile(argv[1]);
	ofstream optFile(argv[4]);
	
	string testLine;
	while (getline(testFile, testLine))
	{
		char temp3[testLine.size()+1];
		strcpy(temp3, testLine.c_str());

		char *temp4 = strtok(temp3, " ");
		string firstWord = string(temp4);
		vector<string> probWords = zhuMap[firstWord];
		map<string, double> wordProb;
		vector<map<string, string>> wordTrack;

		for (int i = 0; i < probWords.size(); i++)
		{
			VocabIndex wid1 = voc.getIndex("<s>");
			VocabIndex wid2 = voc.getIndex(probWords[i].c_str());

			if (wid1 == Vocab_None)
				wid1 = voc.getIndex(Vocab_Unknown);
			if (wid2 == Vocab_None)
				wid2 = voc.getIndex(Vocab_Unknown);
			VocabIndex context[] = {wid1, Vocab_None};
			wordProb[probWords[i]] = lm->wordProb(wid2, context);
		}

		char *nextWord = strtok(NULL, " ");
		while (nextWord != NULL)
		{
			probWords = zhuMap[string(nextWord)];
			// Viterbi forward
			wordTrack.push_back(map<string, string>());
			map<string, double>::iterator iter;
			map<string, double> curProb;
			for (int j = 0; j < probWords.size(); j++)
			{
				double max = -DBL_MAX;
				string resultWord = "";
				for (iter = wordProb.begin(); iter != wordProb.end(); iter++)
				{
					double prevProb = iter->second;

					VocabIndex wid1 = voc.getIndex(iter->first.c_str());
					VocabIndex wid2 = voc.getIndex(probWords[j].c_str());

					if (wid1 == Vocab_None)
						wid1 = voc.getIndex(Vocab_Unknown);
					if (wid2 == Vocab_None)
						wid2 = voc.getIndex(Vocab_Unknown);
					VocabIndex context[] = {wid1, Vocab_None};
					double nextProb = lm->wordProb(wid2, context);

					double bigramProb = prevProb + nextProb;
					if (bigramProb > max)
					{
						max = bigramProb;
						resultWord = iter->first;
					}
				}
				curProb[probWords[j]] = max;
				wordTrack.back()[probWords[j]] = resultWord;
			}
			wordProb = curProb;

			nextWord = strtok(NULL, " ");
		}

		//Viterbi backward
		vector<string> ansSeq;
		double max = -DBL_MAX;
		string bestword = "";
		map<string, double>::iterator iter;
		for (iter = wordProb.begin(); iter != wordProb.end(); iter++)
		{
			if ((iter->second) > max)
			{
				bestword = iter->first;
				max = iter->second;
			}
		}
		ansSeq.push_back(bestword);

		for (int i = wordTrack.size()-1; i >= 0; i--)
		{
			ansSeq.push_back(wordTrack[i][bestword]);
			bestword = wordTrack[i][bestword];
		}

		optFile << "<s> ";
		for (int i = ansSeq.size()-1; i >= 0; i--)
			optFile << ansSeq[i] << " ";
		optFile << "</s>" << endl;


	}
	optFile.close();
	return 0;
}