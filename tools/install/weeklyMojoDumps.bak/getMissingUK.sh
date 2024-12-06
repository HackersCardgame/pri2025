ua="User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_1) AppleWebKit/537.36 (K HTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36"

year=`date +%Y`

for i in $(seq 01 11)
do i=$(printf "%02d" $i)
curl -A '${ua}' -H '${ua}' "https://www.boxofficemojo.com/intl/uk/?yr=$year&wk=$i&p=.htm" -o $year$i.ukmojo
sleep 5
done
