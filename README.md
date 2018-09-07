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

## Criando o serviço

Agora que temos nosso programa, queremos transformá-lo em um serviço de modo a aceitar requisições de outros programas e usuários. Para isto usaremos o Boost.Asio. 
Boost.Asio (Asynchronous Input Output) é uma biblioteca dentre as bibliotecas do Boost usada para programação em rede. 

* Vamos alterar nosso main para que agora ele receba como parâmetro unicamente a porta que o nosso programa irá escutar.

src/main.cpp
```c++
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << “Usage: <PORT>” << endl;
        exit(-1);
    }
    int port = atoi(argv[1]);
    return 0;
}
```

Agora dentro do próprio *main* vamos criar uma classe chamada “Server”. Esta classe será responsável por aceitar e manipular novas requisições feitas ao nosso programa.

```c++
class Server
{
    public:
        Server(boost::asio::io_service &ioservice, short port) : _acceptor(ioservice, tcp::endpoint(tcp::v4(), port)) {
            startAccept();
        }
    private:
        tcp::acceptor _acceptor;
};
```
“io_service” e um objeto que nos permite se comunicar com o sistema operacional. Criamos um membro da classe chamado “_acceptor”,  que é responsável por escutar requisições em uma determinada porta, e recebe como argumentos em seu construtor o serviço que permite acessar o SO e a porta a qual ele irá escutar.

* Vamos agora implementar o método “startAccept”.

```c++
class Server 
{
    ...
    private:
        ...
        void startAccept() {
            Connection *connection = new Connection(_acceptor.get_io_service());
            _acceptor.async_accept(connection->socket(), 
                boost::bind(&Server::handleAccept, this, connection, boost::placeholders::error));
        }
};
```
O método “async_accept” irá abrir um determinado socket e, uma vez aberto, irá chamar uma função de callback. A função “boost::bind” é usada para permitir passar um método da própria classe como argumento, coisa que não seria permitida em C++ puro.

* Vamos agora implementar o método “handleAccept”.

```c++
class Server
{
    ...
    private:
        ...
        void handleAccept(Connection *connection, const boost::system::error_code &error) {
            if (error) delete connection;
            else connection->start();
            startAccept();
        }
};
```
Neste método, primeiro checamos se houve algum erro durante a abertura do socket. Se houve, deletamos a conexão que criamos anteriormente em “startAccept”. Caso contrário, iniciamos a conexão. De qualquer forma, devemos voltar a escutar por novas requisições, e portanto, devemos chamar o método “startAccept” novamente, simulando um loop infinito.

Agora vamos implementar o método “start” de Connection. Inicialmente ele irá ler o que o cliente escreveu.

```c++
class Connection 
{
    public:
        ...
        void start()
        {
           _socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH), boost::bind(&Connection::handleRead, this, boost::placeholders::error, boost::placeholders::bytes_transferred));
        }
};
```
Este método lê alguma coisa do socket de forma assíncrona através do método “async_read_some” e salva o resultado em “_data”. MAX_LENGTH indica que ele ira ler no máximo MAX_LENGTH bytes. Após a leitura, o método “handleRead” é chamado, recebendo como parâmetros se ocorreu algum erro e quantos bytes conseguiram ser lidos.

Agora vamos implementar o método “handleRead”.
```c++
class Connection 
{
    private:
        ...
        void handleRead(const boost::system::error_code &error, size_t bytesTransferred)
        {
            _socket.async_write_some(boost::asio::buffer(_data, bytesTransferred), boost::bind(&Connection::handleWrite, this, boost::placeholders::error));
        }
};
```
Após a leitura, iremos iniciar a escrita. O que iremos escrever será exatamente o que foi lido, armazenado em “_data”. Um método de callback é registrado para indicar o fim da escrita.

Para finalizar nosso example simples, vamos simplesmente criar um objeto do tipo “Server” dentro do nosso main:
```c++
...
int main(int argc, char **argv) 
{
    ...
    try 
    {
        boost::asio::io_service ioservice;
        Server server(ioservice, port);
        ioservice.run();
    }
    catch (exception &e) 
    {
        cerr << “ERROR: “ << e.what() << endl;
        exit(-1);
    }
}
```
O método “run” do ioservice é que irá efetivamente fazer uma chamada ao sistema operacional para fazer com que nosso programa escute em uma porta. 

## Testando nosso serviço criado

Agora podemos testar nosso serviço criado. Compile o programa, rode-o na porta 9000 e abra um terminal telnet:

<img src="https://github.com/RonnyldoSilva/OpenCV-integrado-com-aplicacoes-web/blob/master/images/telnet.png">

## Adaptando nossa aplicação ao serviço

* Agora que temos um serviço rodando, vamos fazer algo útil com ele! Vamos fazer com que o serviço aplique filtros. A lógica vai ser muito semelhante a aplicação anterior, só que ao invés do caminho da imagem e do tipo de filtro ser passado como argumentos do programa, será passado pelo socket.
* Iremos convencionar que o cliente sempre irá mandar os dados no seguinte formato:
* <CAMINHO_DA_IMAGEM_DE_ENTRADA>,<CAMINHO_DA_IMAGEM_DE_SAIDA>,<TIPO_DE_FILTRO>
* O servidor irá mandar de volta “1”, em caso de sucesso, e “0”, em caso de erro

No método “handleRead”, da classe “Connection”, primeiro converta o resultado para uma string e aplique um Split por virgula:
```c++
class Connection 
{
    private:
        ...
        vector<string> split(size_t bytesTransferred) 
        {
            stringstream ss;
            for (size_t i = 0; i < bytesTransferred; i++) ss << _data[i];
            vector<string> tokens;
            string token;
            while (getline(ss, token, ‘,’)) tokens.push_back(token);
            return tokens;
        } 
        
        void handleRead(...) {
            cout << “Read “ << _data << endl;
            vector<string> tokens = split(bytesTransferred);
            if (tokens.size() != 3) {
                _data[0] = 0;
            } 
            else 
            {
                try
                {
                    Mat img = imread(tokens[0]);
                    if (!img.data)
                    {
                        _data[0] = 0;
                    }
                    else
                    {
                        FilterType filterType = (FilterType)atoi(tokens[2].c_str());
                        img = applyFilter(img, filterType);
                        imwrite(tokens[1], img);
                        _data[0] = 1;
                    }
                } 
                catch (...)
                {
                    _data[0] = 0;
                }
                _data[1] = 0;
                bytesTransferred = 2;
            ...
};
```
Agora teste usando Telnet e veja nossa aplicação funcionando!

## Criando um servidor HTTP

* Nossa aplicação já sabe se comunicar na rede!
* Porém, ela sabe se comunicar usando o protocolo HTTP...
* Seria oneroso e contraprodutivo para nós implementar todo o protocolo
* Vamos implementar outro servidor em uma plataforma específica para fazer servidores HTTP que servira como middleware entre o cliente e a nossa aplicação contendo OpenCV.

<img src="https://github.com/RonnyldoSilva/OpenCV-integrado-com-aplicacoes-web/blob/master/images/servidorHTTP.png">    

* Usaremos o Node para desenvolver nosso servidor HTTP.
* No terminal, digite:
```shell
sudo apt-get install npm nodejs-legacy
```
* Em um diretório acima de “SmartFilter”, crie outra pasta chamada “SmartFilterHTTP”. 
* Entre nesta pasta e digite:
```shell
npm init
```
* Este comando irá criar um arquivo de configuração chamado “package.json”, obrigatório para executar aplicações Node.

<img src="https://github.com/RonnyldoSilva/OpenCV-integrado-com-aplicacoes-web/blob/master/images/npmInit.png">

* Perceba o campo “entry point”. Ele é importantíssimo. Indica o primeiro arquivo a ser chamado durante a execução da nossa aplicação (o main). Guarde seu nome (index.js). 
* Vamos agora instalar algumas bibliotecas adicionais que irão nos ajudar durante o nosso desevolvimento. Podemos instalá-la através do Node Package Manager (NPM), que exerce a mesma função do Maven para Java.
```shell
npm install express body-parser multer uuid
```
