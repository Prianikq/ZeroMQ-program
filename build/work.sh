#!/bin/bash
echo "Запуск БСКИ..."
gnome-terminal -- /bin/sh -c './syncsub 10000; exec bash' 
echo "Запуск НО..."
gnome-terminal -- /bin/sh -c './syncpub НО 5564 5562 40000; exec bash'
echo "Запуск РВ..."
gnome-terminal -- /bin/sh -c './syncpub РВ 5563 5561 200000; exec bash'





