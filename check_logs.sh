first_file="server_logs/server_nu1.log"
for i in "server_logs"; do
    diff -q "$i" $first_file;
done


