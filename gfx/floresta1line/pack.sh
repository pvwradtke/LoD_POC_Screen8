split floresta.raw floresta_ -b 256 -d -a 4

list=`ls floresta_*`
for i in $list;
do
    lz48 -i $i -o $i.lz48
done
