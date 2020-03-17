#!/usr/bin/bash

../builddir/lab2.exe bg.pnm example.pnm 0 5 0 10 1000 10
../builddir/lab2.exe example.pnm example.pnm 127 5 0 20 1000 20
../builddir/lab2.exe example.pnm example.pnm 255 5 0 30 1000 30

../builddir/lab2.exe example.pnm example.pnm 0 0.5 50 50 250 250
../builddir/lab2.exe example.pnm example.pnm 0 1 50 60 250 260
../builddir/lab2.exe example.pnm example.pnm 0 3 50 70 250 270
../builddir/lab2.exe example.pnm example.pnm 0 6 50 80 250 280

../builddir/lab2.exe example.pnm example.pnm 255 50 600 100 730 600

../builddir/lab2.exe example.pnm example.pnm 0 5 8 520 248 520
../builddir/lab2.exe example.pnm example.pnm 0 5 8 520 8 760
../builddir/lab2.exe example.pnm example.pnm 0 5 248 760 8 760
../builddir/lab2.exe example.pnm example.pnm 0 5 248 760 248 520

../builddir/lab2.exe example.pnm example.pnm 176 7 768 768 -10 320
../builddir/lab2.exe example.pnm example.pnm 47 7 260 780 600 256
