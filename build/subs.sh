#!/bin/bash
echo "Запуск БСКИ №1..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ1 100000; exec bash' 
echo "Запуск БСКИ №2..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ2 100000; exec bash'
echo "Запуск БСКИ №3..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ3 100000; exec bash'
