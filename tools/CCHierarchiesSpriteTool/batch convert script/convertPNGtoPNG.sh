#!/bin/sh

export PATH=$PATH:ImageOptim-CLI/bin

# echo "==== alpha dilate begin ===="
# find . -name '*.png' -exec ./AlphaUtilityCL -i {} -o {} \;
# echo "==== alpha dilate end ===="

echo "==== png optimisation begin ===="
imageOptim --directory . -a -q
# find . -name '*.png' -exec ./pngquant 256 {} \;
# find . -name '*.png' -exec ./pngout {} \;
echo "==== png optimisation end ===="
