#!/bin/bash
echo "Запуск НО..."
gnome-terminal -- /bin/sh -c './syncpub НО 5561 40000; exec bash'
echo "Запуск РВ..."
gnome-terminal -- /bin/sh -c './syncpub РВ 5562 200000; exec bash'
echo "Запуск РЛС..."
gnome-terminal -- /bin/sh -c './syncpub РЛС 5563 66000; exec bash'
echo "Запуск ИУП №1..."
gnome-terminal -- /bin/sh -c './syncpub ИУП 5567 0; exec bash'
echo "Запуск ИУП №2..."
gnome-terminal -- /bin/sh -c './syncpub ИУП 5568 0; exec bash'
echo "Запуск ИУП №3..."
gnome-terminal -- /bin/sh -c './syncpub ИУП 5569 0; exec bash'
