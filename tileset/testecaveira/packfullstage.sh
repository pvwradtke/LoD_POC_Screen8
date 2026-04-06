#Clean up
#rm fase_packed
#rm fase_[0-9]*

#png2sc8raw $1.png $1.raw NODISPLAY $2

cat diffusion_dither.raw diffusion_dither.raw pattern_dither.raw no_dither.raw pablo.raw curve.raw brightness.raw original.raw > fasecaveira.raw

split fasecaveira.raw fasecaveira_ -b 256 -d -a 4

count=`ls -1 fasecaveira_* | wc -l`

list=`ls fasecaveira_[0-9]*`
for i in $list;
do
    lz48 -i $i -o $i.lz48
done

./pack_stage fasecaveira_ $count

rm fasecaveira_[0-9]*

