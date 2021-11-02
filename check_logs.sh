first_file="server_logs/server_n1.log"
for i in server_logs/server_n*.log; do
    diff -q "$i" $first_file;
done
