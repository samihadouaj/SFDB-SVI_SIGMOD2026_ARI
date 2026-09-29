# download UCI datasets
wget -P data -nc -i uci_datasets.txt
for filename in ./data/*.gz
do
   if [ ! -f "$(echo ${filename} | sed -e 's/\.[^.]*$//')" ]
      then
         gunzip -c -v "${filename}" > $(echo ${filename} | sed -e 's/\.[^.]*$//')  # x.txt.gz >x.txt
   fi
done
# yes n | gunzip -k -q ./data/*.txt.gz


