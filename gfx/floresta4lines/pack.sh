split floresta.raw floresta_ -b 2K -d -a 3

list=`ls floresta_*`
for i in $list;
do
    lz48 -i $i -o $i.lz48
done
