cd asm
make >/dev/null
echo -e "Asm:"
cd ..
{ time asm/test.sh >/dev/null; } |& grep "sys"
echo -e "versus\nC:"
cd c
cmake --build ./build/ >/dev/null
cd ..
{ time c/test.sh >/dev/null; } |& grep "sys"
