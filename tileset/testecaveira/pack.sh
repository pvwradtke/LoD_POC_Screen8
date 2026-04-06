#Clean up
#rm fase_packed
#rm fase_[0-9]*

png2sc8raw $1.png $1.raw NODISPLAY $2
split $1.raw $1_ -b 256 -d -a 4

count=`ls -1 $1_* | wc -l`

list=`ls $1_[0-9]*`
for i in $list;
do
    lz48 -i $i -o $i.lz48
done

./pack_stage $1_ $count

rm $1_[0-9]*

