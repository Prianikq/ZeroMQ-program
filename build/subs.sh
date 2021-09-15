#!/bin/bash
echo "Запуск БСКИ №1..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ1 localhost 100000 5567; exec bash' 
echo "Запуск БСКИ №2..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ2 localhost 100000 5568; exec bash'
echo "Запуск БСКИ №3..."
gnome-terminal -- /bin/sh -c './syncsub БСКИ3 localhost 100000 5569; exec bash'
