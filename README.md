## Img2Fourier GUI application v1.0 ##
Based on [Savannah](https://github.com/Oscillograph/Savannah) application framework.

## Build: ##
```
cmake -S . -B build
cd build
make
make install
```

The program and (probably) all necessary dependencies will reside in the "out" directory in the source root. If the program crashes at startup, it's probably due to lack of "data" directory inside the "out" one. Copy "data" directory from the source root to "out" directory -- and the program should start properly.

## Features: ##
+ Load images of different formats: BMP, PNG, JPEG, GIF
+ Calculate 2D Fourier Transform of the loaded image
+ Display 2D Fourier Transform in different colormaps: Jet, Jet with Black & White, Black and White, White and Black, HSV, Greenish, Matrix, Inferno, Hot, Seismic
+ Save 2D Fourier Transform as an image in PNG format

## Known bugs ##
- диалоговое окно выбора файла не обновляет содержимое папки автоматически, если вручную вводить путь, содержащий кириллицу;
- загрузка некоторых изображений в формате PNG/JPEG может приводить к вылету программы с ошибкой;

## Screenshot ##
<img src="https://raw.githubusercontent.com/Oscillograph/Img2Fourier/main/data/screenshot.png" alt="Img2FourierGUI" width="400"/>
<img src="https://raw.githubusercontent.com/Oscillograph/Img2Fourier/main/data/screenshot-2.png" alt="Img2FourierGUI" width="400"/>
