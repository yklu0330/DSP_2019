## Setup SRILM
To compile the code successfully, install SRILM toolkit before compiling. It is a toolkit for building and applying various statistical language models.

## Workflow
![](https://i.imgur.com/V7V1ffq.png)


## File description

#### Big5-ZhuYin.map
- a mapping from Chinese character to ZhuYin
#### corpus.txt
- corpus data
#### Makefile
#### separator_big5.pl
- script used to separate Chinese words
#### setup.sh
- script used to activate SRILM environment
#### test_data
- directory for test data

## Setup
`$ source setup.sh`

## Character Segmentation
`$ perl separator_big5.pl corpus.txt > seg.txt`

## Language Model
`$ ngram-count ‒text corpus.txt ‒write count.txt ‒order 2`
`$ ngram-count ‒read count.txt ‒lm lm.txt ‒order 2 -unk`

## Compiling
`$ make`

## Mapping Program Compiling
`$ make map Big5-ZhuYin.map ZhuYin-Big5.map`

## Running MyDisambig
`$ ./mydisambig seg.txt ZhuYin-Big5.map lm.txt output.txt`
