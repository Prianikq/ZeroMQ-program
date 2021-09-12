#!/bin/bash
echo "Запуск НО..."
gnome-terminal -- /bin/sh -c './syncpub НО 5561 40000; exec bash'
echo "Запуск РВ..."
gnome-terminal -- /bin/sh -c './syncpub РВ 5562 200000; exec bash'
