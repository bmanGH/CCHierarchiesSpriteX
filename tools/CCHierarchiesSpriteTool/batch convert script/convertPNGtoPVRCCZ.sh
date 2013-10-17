#!/bin/sh

echo "==== convert png to pvr begin ===="
find . -name '*.png' -exec ./PVRTexToolCL -fOGL8888 -yflip 0 -l -i {} \;
echo "==== convert png to pvr end ===="

echo "==== compression pvr to pvr ccz begin ===="
find . -name '*.pvr' -exec ./ccz {} \;
echo "==== compression pvr to pvr ccz end ===="

echo "==== rename image name in .hsheet file begin ===="
find . -name '*.hsheet' -exec sed -i .bak -e "s/.png/.pvr.ccz/" {} \;
echo "==== rename image name in .hsheet file end ===="

echo "==== remove unused files begin ===="
find . -name '*.png' -exec rm {} \;
find . -name '*.pvr' -exec rm {} \;
find . -name '*.bak' -exec rm {} \;
echo "==== remove unused files end ===="
