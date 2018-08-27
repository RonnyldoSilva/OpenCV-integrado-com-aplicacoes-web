#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "grayscale_filter.h"
#include "edge_filter.h"
#include "cartoon_filter.h"
#include "retro_filter.h"

using namespace std;
using namespace cv;
using namespace boost::asio::ip;

enum FilterType {
	GRAYSCALE = 0, EDGE = 1, CARTOON = 2, RETRO = 3
};

Mat applyFilter(const Mat &img, FilterType filterType) {
	AbstractFilter *filter;
	switch (filterType) {
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

class Connection {
public:
	Connection(boost::asio::io_service &ioservice) : _socket(ioservice) {
	}

	tcp::socket& socket() {
		return _socket;
	}
	
	void start() {
		_socket.async_read_some(boost::asio::buffer(_data, MAX_LENGTH), 
			boost::bind(&Connection::handleRead, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}
private:
	static const int MAX_LENGTH = 4069;
	tcp::socket _socket;
	char _data[MAX_LENGTH];

	vector<string> split(size_t bytesTransferred) {
		stringstream ss;
		for (size_t i = 0; i < bytesTransferred; i++) {
			ss << _data[i];
		}
		vector<string> tokens;
		string token;
		while (getline(ss, token, ',')) {
			tokens.push_back(token);
		}
		return tokens;
	}

	void handleRead(const boost::system::error_code &error, size_t bytesTransferred) {
		if (error) {
			delete this;
		} else {
			cout << "Read: " << _data << endl;
			vector<string> tokens = split(bytesTransferred);
			if (tokens.size() != 3) {
				_data[0] = 0;
			} else {
				try {
					string inputFilename = tokens[0];
					string outputFilename = tokens[1];
					FilterType filterType = (FilterType)atoi(tokens[2].c_str());
					Mat img = imread(inputFilename);
					if (!img.data) {
						cerr << "Could not read image: " << inputFilename << endl;
						_data[0] = 0;
					} else {
						img = applyFilter(img, filterType);
						imwrite(outputFilename, img);
						_data[0] = 1;	
					}
				} catch (...) {
					_data[0] = 0;
				}
			}
			_data[1] = 0;
			/*boost::asio::async_write(_socket, 
				boost::asio::buffer(_data, bytesTransferred),
				boost::bind(&Connection::handleWrite, this,
					boost::asio::placeholders::error));*/
			_socket.async_write_some(boost::asio::buffer(_data, 2),
				boost::bind(&Connection::handleWrite, this,
					boost::asio::placeholders::error));
		}
	}

	void handleWrite(const boost::system::error_code &error) {
		delete this;
	}

};

class Server {
public:
	Server(boost::asio::io_service &ioservice, int port) : _acceptor(ioservice, tcp::endpoint(tcp::v4(), port)) {
		startAccept();
	}
private:
	tcp::acceptor _acceptor;

	void startAccept() {
		Connection *connection = new Connection(_acceptor.get_io_service());
		_acceptor.async_accept(connection->socket(),
			boost::bind(&Server::handleAccept, this, connection, 
				boost::asio::placeholders::error));
	}

	void handleAccept(Connection *connection, const boost::system::error_code &error) {
		if (error) {
			delete connection;
		} else {
			connection->start();
		}
		startAccept();
	}

};

int main(int argc, char **argv) {
	if (argc != 2) {
		cerr << "Usage: <PORT>" << endl;
		exit(-1);
	}
	int port = atoi(argv[1]);
	try {
		boost::asio::io_service ioservice;
		Server server(ioservice, port);
		ioservice.run();
	} catch (exception &e) {
		cerr << "ERROR: " << e.what() << endl;
		exit(-1);
	}
	/*if (argc != 3) {
		cerr << "Usage: <PATH_TO_IMAGE> <FILTER 0=GRAYSCALE,1=EDGE,2=CARTOON,3=RETRO>" << endl;
		exit(-1);
	}
	Mat img = imread(argv[1]);
	if (!img.data) {
		cerr << "Could not read image: " << argv[1] << endl;
		exit(-1);
	}
	FilterType filterType = (FilterType)atoi(argv[2]);
	img = applyFilter(img, filterType);
	imshow("image", img);
	waitKey(0);*/
	return 0;
}
