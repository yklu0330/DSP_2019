## File description

# data/train_seq_0X.txt
- training data (10K observation sequences)
# data/test_lbl.txt
- testing labels
# data/test_seq.txt
- Testing data (2.5K observation sequences)
# model_init.txt
- initial model parameters
# modellist.txt
- paths to model files
# Makefile
# inc/hmm.h
- load and dump HMM models
# src/train.c
- train model
# src/test.c
- test model

## Compiling
make

## Training
./train $iter model_init.txt data/train_seq_01.txt model01.txt
./train $iter model_init.txt data/train_seq_02.txt model02.txt
./train $iter model_init.txt data/train_seq_03.txt model03.txt
./train $iter model_init.txt data/train_seq_04.txt model04.txt
./train $iter model_init.txt data/train_seq_05.txt model05.txt

## Testing
./test modellist.txt data/test_seq.txt result.txt
