file=$1
docker run -v .:/src/ -it z88dk/z88dk z80asm -b "/src/$file"