#Clean up
rm fase_packed
rm fase_[0-9]*

png2sc8raw fase.png fase.raw NODISPLAY DIVISION
split fase.raw fase_ -b 256 -d -a 4

count=`ls -1 fase_* | wc -l`

list=`ls fase_*`
for i in $list;
do
    lz48 -i $i -o $i.lz48
done

./pack_stage fase_ $count

rm fase_[0-9]*

