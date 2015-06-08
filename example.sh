./compile
#./build/main -r 10 -t 100000 -i prefmats/pref_sushi.txt -o test.txt
./build/main -r 10 -t 100000 -i prefmats/pref_arithmetic.txt -o test.txt
rm regret.pdf
python simpleplot.py out/test.txt 
