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

Boost é a biblioteca que irá fazer a comunicação entre a aplicação c++ mais o Node JS:

```shell
sudo apt-get install libboost-all-dev
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

int main(int argc, char **argv)  
{
    if (argc != 2) 
    {
        cerr << “Usage: <PATH_TO_IMAGE>” << endl;
        exit(-1);
    }
    
    Mat img = imread(argv[1]);
    
    if (!img.data) 
    {
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
Abra o terminal do Ubuntu, compile e baixe esta imagem de teste abaixo, tudo isso dentro da pasta onde contém o Makefile:
```shell
make
wget https://upload.wikimedia.org/wikipedia/en/2/24/Lenna.png
```
E teste:
```shell
./SmartFilter ./Lenna.png
```
Vamos agora criar uma classe abstrata para representar um filtro genérico. Essa classe abstrata estará no arquivo “src/abstract_filter.h”.

abstract_filter.h
```c++
#ifndef ABSTRACTFILTER_H
#define ABSTRACTFILTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class AbstractFilter 
{
    public:
        virtual cv::Mat filter(const cv::Mat &input) = 0;
};

#endif // ABSTRACTFILTER_H
```
Vamos agora criar uma classe abstrata para representar um filtro genérico. Essa classe abstrata estará no arquivo “src/abstract_filter.cpp”.

src/grayscale_filter.cpp
```c++
#include “grayscale_filter.h”

using namespace cv;

Mat GrayscaleFilter::filter(const Mat &input) 
{
    Mat output;
    cvtColor(input, output, CV_BGR2GRAY);
    return output;
}
```

Agora vamos adicionar mais outros três filtros para dar um sentido de completude à nossa aplicação:
* Filtro de cartoonização
* Filtro detector de bordas
* Filtro retrô

#### Filtro de detecção de bordas:

src/edge_filter.h
```c++
#ifndef EDGEFILTER_H
#define EDGEFILTER_H

#include “abstract_filter.h”

class EdgeFilter : public AbstractFilter 
{
    public:
        cv::Mat filter(const cv::Mat &input);
};

#endif // EDGEFILTER_H
```

src/edge_filter.cpp
```c++
#include “edge_filter.h”

using namespace cv;

Mat EdgeFilter::filter(const Mat &input)
{
    Mat output;
    cvtColor(input, output, CV_BGR2GRAY);
    Canny(output, output, 10, 100, 3);
    return output;
}
```

#### Filtro de cartoonização

src/cartoon_filter.h
```c++
#ifndef CARTOONFILTER_H
#define CARTOONFILTER_H

#define NUM_COLORS 8

#include “abstract_filter.h”

class CartoonFilter : public AbstractFilter
{
    public:
        cv::Mat filter(const cv::Mat &input);
};
#endif // CARTOONFILTER_H
```

src/cartoon_filter.cpp
```c++
#include “cartoon_filter.h”
using namespace cv;
Mat CartoonFilter::filter(const Mat &input)
{
    Mat output = input.clone();
    Mat edges;
    cvtColor(output, edges, CV_BGR2GRAY);
    Canny(edges, edges, 10, 100, 3);
    cvtColor(edges, edges, CV_GRAY2BGR);
    
    for (int i = 0; i < output.rows; i++)
    {
        uchar *data = output.ptr<uchar>(i);
        for (int j = 0; j < output.cols * output.channels(); j++)
        {
            data[j] = data[j] / NUM_COLORS * NUM_COLORS + NUM_COLORS / 2;
        }
    }
    output -= edges; 
    return output;
}

```

#### Filtro retrô

src/retro_filter.h
```c++
#ifndef RETROFILTER_H
#define RETROFILTER_H

#include “abstract_filter.h”

class RetroFilter : public AbstractFilter {
public:
    cv::Mat filter(const cv::Mat &input);
};

#endif // RETROFILTER_H
```

src/retro_filter.cpp
```c++
#include “retro_filter.h”

using namespace cv;

Mat RetroFilter::filter(const Mat &input) {
    Mat output;
    cvtColor(input, output, CV_BGR2GRAY);
    Mat noise = Mat::zeros(output.size(), output.type());
    randn(noise, 20, 40);
    output += noise;
    return output;
}
```

Agora que temos todos os filtros implementados, vamos modificar nosso main.cpp para que seja possível passar outro argumento para o programa, indicando o filtro a ser usado.

src/main.cpp
```c++
#include <iostream>

#include “grayscale_filter.h”
#include “edge_filter.h”
#include “cartoon_filter.h”
#include “retro_filter.h”

using namespace std;
using namespace cv;

enum FilterType
{
    GRAYSCALE = 0, EDGE = 1, CARTOON = 2, RETRO = 3
}

Mat applyFilter(const Mat &img, FilterType filterType)
{
    AbstractFilter *filter;
    switch (filterType) 
    {
    case EDGE:
        filter = new EdgeFilter;
        break;
    case CARTOON:
        filter = new CartoonFilter;
        break;
    case RETRO:
        filter = new RetroFilter;
        break;
    default:
        filter = new GrayscaleFilter;
    }
    Mat filtered = filter->filter(img);
    delete filter;
    return filtered;
}

int main(int argc, char **argv) 
{
    if (argc != 3)
    {
        cerr << “Usage: <PATH_TO_IMAGE> <FILTER 0=GRAYSCALE,1=EDGE,2=CARTOON,3=RETRO>” << endl;
        exit(-1);
    }
    Mat img = imread(argv[1]);
    if (!img.data)
    {
        cerr << “Could not read image: “ << argv[1] << endl;
        exit(-1);
    }
    FilterType filterType = (FilterType)atoi(argv[2]);
    img = applyFilter(img, filterType);
    imshow(“image”, img);
    waitKey(0);
    return 0;
}

```
