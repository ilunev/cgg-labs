# Лабораторная работа 7

Программа применяет алгоритм повышения резкости Contrast Adaptive Sharpening к изображениям PNM P5 и P6.

## Использование

```shell
./lab7.exe <input> <output> <sharpen>
```

input - имя входного файла
output - имя выходного файла
sharpen - вещественное значение от 0 до 1

## Сборка через Meson + Ninja

```shell
meson builddir
cd builddir
ninja
```
