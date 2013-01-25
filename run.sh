sh compile.sh

echo "Adjusting camera settings."
sudo uvcdynctrl -d /dev/video1 -s 'White Balance Temperature, Auto' 0

echo "Running python controller."
python core.py
