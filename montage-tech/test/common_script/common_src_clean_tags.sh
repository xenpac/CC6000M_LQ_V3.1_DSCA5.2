sed -i 's/\\\\/\//g' all_src_files.txt

for  i  in  `cat all_src_files.txt`
do
		sed -i '/Montage.*Technology/d' $i
		sed -i '/Montage.*Proprietary/d' $i
		sed -i '/Copyright.*Montage Tech/d' $i		
done