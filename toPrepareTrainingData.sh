echo Prepare training data :
echo Step 1. Tokenize training data
#./prepareData/scripts/tokenizer.perl -l fr < prepareData/testCorpus/testCorpus.fr > prepareData/trainingCorpus/Test.tok.fr
#./prepareData/scripts/tokenizer.perl -l en < prepareData/testCorpus/testCorpus.en > prepareData/trainingCorpus/Test.tok.en
./prepareData/scripts/tokenizer.perl -l fr < prepareData/testCorpus/t1.fr > prepareData/trainingCorpus/Test.tok.fr
./prepareData/scripts/tokenizer.perl -l en < prepareData/testCorpus/t1.en > prepareData/trainingCorpus/Test.tok.en

echo Step 2. Filter out long sentence
./prepareData/scripts/clean-corpus-n.perl prepareData/trainingCorpus/Test.tok fr en prepareData/trainingCorpus/Test.clean 1 40

echo Step 3. Lowercase training data
./prepareData/scripts/lowercase.perl < prepareData/trainingCorpus/Test.clean.fr > prepareData/trainingCorpus/Test.lowercase.fr
./prepareData/scripts/lowercase.perl < prepareData/trainingCorpus/Test.clean.en > prepareData/trainingCorpus/Test.lowercase.en
