# Especto Project [RU]

Данный репозиторий содержит исходный код целевого проекта мастерской №3 "Классовый C или 22 упавших теста", ЛШЮП 2019.

## Ученики
* Маневич Герман - human-pack.cpp, human-unpack.cpp
* Подоскин Константин - main.cpp
* Юриков Никита - human-pack.cpp, human-unpack.cpp

## Краткое описание функционала
* Чтение ESP/ESM формата файлов от игры [The Elder Scrolls III: Morrowind](https://ru.wikipedia.org/wiki/The_Elder_Scrolls_III:_Morrowind) во внутреннее представление
* Поиск подстрок в данных подзаписей, которые содержат строковые данные
* Редактирование данных в подзаписях
* Распаковка внутреннего представления в иерархическую структуру файлов и папок без потери данных
* Чтение иерархической структуры файлов и папок во внутреннее представление
* Распаковка внутреннего представления в иерархическую структуру файлов и папок с дополнительным преобразованием в человекочитаемый формат
* Чтение иерархической структуры файлов и папок в человекочитаемом формате во внутреннее представление (не было тщательно оттестировано)
* Запись внутреннего представления в ESP/ESM файл

## Сборка
CMake 3.12+, компилятор с поддержкой C++ 17 и стандартной библиотекой с Filesystem Library не в статусе экспериментального (libstd++ 9 и выше, libc++ 9 и выше). Особенностей или дополнительных шагов нет.

# Especto Project [EN]

This repository contains the code and data of the final project of the 3rd workshop, SSYP (LSHUP) 2019.

## Build
CMake 3.12+, C++ 17 conformant compiler with standard library containing Filesystem Library in non-experimental state (libstd++ 9 or greater, libc++ 9 or greater). No workarounds or additional build stages are required.

## Terse functionality description
* ESP/ESM [The Elder Scrolls III: Morrowind](https://en.wikipedia.org/wiki/The_Elder_Scrolls_III:_Morrowind) game data file format reader into Internal Representation (IR)
* Substring search within subrecords data of type string
* Subrecord data modification
* Lossless IR extraction to file tree
* File tree reading into IR
* IR extraction to file tree with additional conversion to human-readable format
* File tree reading of data in human-readable format into IR (this functionality wasn't properly tested)
* ESP/ESM file writer from IR

# Используемый сторонний код | Third-Party Code
* [fmem](https://github.com/Snaipe/fmem) - [MIT](https://github.com/Snaipe/fmem/blob/master/LICENSE)
* [BMP Generation Code (Stack Overflow)](https://stackoverflow.com/a/47785639) - [CC-BY-SA 3.0](https://creativecommons.org/licenses/by-sa/3.0/) ([Minhas Kamal](https://stackoverflow.com/users/4684058/minhas-kamal))
