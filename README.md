# OpenCV integrado com Aplicações Web

## Objetivo

Desenvolver soluções de visão computacional integrada à dispositivos móveis e aplicações web. Durante este minicurso, iremos desenvolver uma aplicação de filtros, que será transformada em uma aplicação mobile.

## Ferramentas que iremos utilizar

* OpenCV (biblioteca open-source de visão computacional em C++)
* Boost (conjunto de bibliotecas em C++ que provê diversas funcionalidades, dentre elas a possibilidade de criar um servidor TCP/IP)
* NodeJS (plataforma que permite criar servidores HTTP em JavaScript)
* AngularJS (framework que facilita o desenvolvimento de aplicações web do lado do cliente)
* Ionic (framework que permite exportar aplicações web do lado do cliente em aplicações mobile nativas) 

### Instalando o OpenCV

```shell
sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
git clone https://github.com/Itseez/opencv.git
mkdir opencv/release
cd opencv/releasecmake ..
make –j8 && sudo make install
```
### Instalando o Boost

Acesse www.boost.org, baixe a versão mais recente, extraia, entre na pasta, e digite apenas dois comandos:

```shell
./bootstrap.sh
sudo ./b2 install
```
## O básico do básico

* Vamos criar uma aplicação simples só pra testar se tudo está OK!
* Crie uma pasta chamada “SmartFilter”.
* Dentro desta pasta, crie um arquivo chamado “main.cpp” dentro de uma subpasta chamada “src”.
* “src” será onde iremos salvar nossos códigos-fonte.
* Crie o seguinte arquivo: 

src/main.cpp
```c++
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv)  {
    if (argc != 2) {
        cerr << “Usage: <PATH_TO_IMAGE>” << endl;
        exit(-1);
    }
        Mat img = imread(argv[1]);
    if (!img.data) {
        cerr << “Could not read image: “ << argv[1] << endl;
        exit(-1);
    }
    imshow(“image”, img);
    waitKey(0);
    return 0;
}
```

* Agora vamos criar um Makefile simples:

Makefile:

```makefile
CPPFLAGS=-std=c++11
OPENCV_LIBS=-lopencv_core –lopencv_highgui –lopencv_imgcodecs –lopencv_imgproc
LIBS=$(OPENCV_LIBS)
LDFLAGS=$(LIBS)
srcs=$(wildcard src/*.cpp)
objs=$(patsubst src/%.cpp,release/%.o,$(srcs))

release/%.o: src/%.cpp
    mkdir –p release
    g++ $(CPPFLAGS) –c $^ -o $@
    
default: $(objs)
    g++ $(CPPFLAGS) –o SmartFilter $(objs) $(LDFLAGS)
    
clean:
    rm –rf release/* SmartFilter
```
Compile e baixe esta imagem de teste abaixo:

```shell
wget https://upload.wikimedia.org/wikipedia/en/2/24/Lenna.png
```

