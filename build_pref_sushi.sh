echo "making sushi preference matrix"
cd sushi
echo "downloading sushi3.tgz"
wget http://www.kamishima.net/asset/sushi3.tgz
echo "extracting sushi3.tgz"
tar xf sushi3.tgz
echo "creating prefmats/pref_sushi.txt"
python build_pref_sushi.py > ../prefmats/pref_sushi.txt
echo "done"
