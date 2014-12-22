# [Floppy Disk Ripper Converter v0.1 alpha](https://github.com/psbhlw/floppy-disk-ripper)
---

## Описание

Floppy Disk Ripper Converter предназначен для преобразования образов дискет, полученных при помощи Floppy Disk Ripper, в распространённые форматы, используемые эмуляторами.
Программа работает только с дискетами формата MFM. Внутренний формат представлен в виде массива интервалов между импульсами сигнала READ DATA в единицах периода частоты дискретизации.
В конвертере реализован анализатор треков, который выдаёт предупреждения при ошибках CRC или не найденных секторах.

## Использование программы:

*fdrc [options] input_file [input_file [...]]*

Опции:

 **-i fmt**       - Тип входных файлов (fdr, adf, raw, mfm). Если формат не указан, будет применено автораспознавание формата.

 **-o fmt**       - Тип выходного файла (udi, adf, img). Если формат не указан, выходной формат будет определен по расширению выходного файла.

 **-O filename**  - Имя выходного файла.

 **-T tracks**    - Обрабатывать только указанные треки. Можно указывать отдельные треки и/или диапазон, например: 0,40-79 означает обрабатывать трек 0 и треки с 40 по 79 включительно.

 **-S sides**     - Обрабатывать только указанную сторону: 0 - нижняя, 1 - верхняя или 2 - обе.

 **-h**           - Показать краткую подсказку по использованию программы.

 **-r n**         - Использовать захваченный оборот диска номер n. По-умолчанию используется первый оборот, 0 - все обороты (неприменимо для некоторых форматов).

 **-w n**         - Задать время битового интервала в секундах: 0.000002 (2 мкс) для дискет двойной плотности, 0.000001 (1 мкс) для дискет высокой плотности.

 **-f n**         - Принудительно задать частоту дискретизации сырого трека, Гц. По умолчанию используется частота 14МГц.

В качестве входных файлов можно задавать как файлы, так и папки. Папки не обрабатываются рекурсивно. Файлы загружаются в указанном порядке, это позволяет заменять отдельные треки в образах.

---

## Форматы файлов

### FDR (Floppy Disk Ripper)

Образ, снятый программой **Floppy Disk Ripper** для ZX Evolution с использованием соответствующей прошивки.

### [UDI](http://speccy.info/UDI) (Ultra Disk Image)

Формат был разработан Александром Макеевым, данные хранятся в виде побайтовых образов дорожек, включая пробелы, синхропробелы, синхроимпульсы и т.п.

### IMG

Формат содержит только полезные данные всех секторов без служебной информации.

### ADF (UAE-1ADF)

Образ MFM-диска, снятый компьютером Amiga. Содержит данные диска в виде потока MFM-бит (данные и синхробиты).

---

## Copyright and license

Copyright (C) 2014, psb^hlw, ts-labs.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
