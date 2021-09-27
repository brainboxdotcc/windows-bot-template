#!/bin/bash
echo "Fetching latest DPP"
mkdir temp
cd temp
echo "Download assets from CI..."
gh run list -w "D++ CI" -R "brainboxdotcc/DPP" | grep $'\t'master$'\t' | grep ^completed | head -n1
gh run download -R "brainboxdotcc/DPP" `gh run list -w "D++ CI" -R "brainboxdotcc/DPP" | grep $'\t'master$'\t' | grep ^completed | head -n1 | awk '{ printf $(NF-2) }'`

cd "./libdpp - Windows x64-Release" && unzip *.zip
# header files from first zip
cp -rv */include ../../MyBot/dependencies/

# dll files
cp -rv */bin/*.dll ../../MyBot/dependencies/64/release/bin/
# lib files
cp -rv */lib ../../MyBot/dependencies/64/release/
cd ..

cd "./libdpp - Windows x64-Debug" && unzip *.zip
# dll files
cp -rv */bin/*.dll ../../MyBot/dependencies/64/debug/bin/
# lib files
cp -rv */lib ../../MyBot/dependencies/64/debug/
cd ..

cd "./libdpp - Windows x86-Release" && unzip *.zip
# dll files
cp -rv */bin/*.dll ../../MyBot/dependencies/32/release/bin/
# lib files
cp -rv */lib ../../MyBot/dependencies/32/release/
cd ..

cd "./libdpp - Windows x86-Debug" && unzip *.zip
# dll files
cp -rv */bin/*.dll ../../MyBot/dependencies/32/debug/bin/
# lib files
cp -rv */lib ../../MyBot/dependencies/32/debug/
cd ..

# unix2dos
cd ..
cd MyBot/dependencies/include/dpp-9.0/dpp
find . -exec dos2unix {} \;
cd ../../../../..
find . -name '*.cmake' -exec dos2unix {} \;

echo "Cleaning up..."
rm -rf temp

echo "Committing..."
git add MyBot/dependencies
git commit -m "auto update to latest DPP master branch"

