echo "Generating documentation"
set -x
doxygen Doxyfile
cd docs
git add .
git commit -m "regen docs"
git push -u origin master
cd ..
git add .
git commit -m "regen docs"
git push -u origin master
set +x
echo "Finished generating documentation"
