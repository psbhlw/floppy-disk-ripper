# [Floppy Disk Ripper Firmware for ZX Evolution](https://github.com/psbhlw/floppy-disk-ripper)
---

## Описание

Floppy Disk Ripper Firmware - специализированная прошивка для компьютера [ZX Evolution](http://www.nedopc.com/zxevo/zxevo.php), которая содержит модуль захвата сигнала READ DATA с дисковода.

---

## Прошивка ZX Evolution

FDR основана на конфигурации TS-Conf, поэтому требуется прошить файл `ts-bios.rom` в ПЗУ по адресам 0x0000-0xFFFF (первые 4 страницы). Сделать это можно при помощи программы Test'n'Service или из ERS ('Update Custom ROM').
   
Для прошивки FDR Firmware необходимо поместить файл `zxevo_fw.bin` в корневой каталог SD-карты. Удерживая в нажатом состоянии кнопку Soft reset (либо закоротив соответствующий пин, см. инструкцию ZX Evolution), нажать и отпустить Hard reset. Прошивка длится до 10 секунд, после чего ZX Evolution запустится в рабочем режиме.
Владельцам платы IDE-VideoDAC нужно использовать файл прошивки `zxevo_fw_vdac.bin`, переименовав его в `zxevo_fw.bin`.

Файлы прошивок содержат по две конфигурации: собственно FDR и BaseConf. Переключение между ними происходит по нажатию 'LAlt'+'LCtrl'+'F12'. Из TS-Conf, на основе которой сделана FDR, был удален модуль TSU (в связи с нехваткой места в чипе FPGA), поэтому использовать эту версию для полноценной работы, к сожалению, невозможно.  

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
