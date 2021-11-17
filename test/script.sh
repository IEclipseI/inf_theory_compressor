
for filename in files/*; do
  echo $filename
  ./coder files/$(basename -- "$filename") tmp/$(basename -- "$filename").coded
  ./decoder tmp/$(basename -- "$filename").coded tmp/$(basename -- "$filename").decoded
  echo "$(diff files/$(basename -- "$filename") tmp/$(basename -- "$filename").decoded)"

#    for ((i=0; i<=3; i++)); do
#        ./MyProgram.exe "$filename" "Logs/$(basename "$filename" .txt)_Log$i.txt"
#    done
done