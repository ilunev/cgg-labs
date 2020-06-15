# Лабораторная работа 4

Программа производит преобразование цветовых пространств в PNM изображениях. Поддерживаемые пространства: RGB, HSV, HSL, YCbCr.601, YCbCr.709, YCoCg, CMY.

## Использование

```shell
./lab4.exe -f <from_colorspace> -t <to_colorspace> -i <count> <input_files> -o <count> <output_files>
```

## Сборка через Meson + Ninja

```shell
meson builddir
cd builddir
ninja
```
